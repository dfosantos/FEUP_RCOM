	#include "FTP.h"


int ftpConnect(ftp* ftp, const char* ip, int port) {

	int socketfd;
	struct sockaddr_in server_addr;
	char rd[1024];



	// server address handling
	bzero((char*) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip); 					/*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(port); 							/*server TCP port must be network byte ordered */
	socketfd = connectSocket(ip, port);

	// open an TCP socket
	socketfd = socket(AF_INET, SOCK_STREAM, 0);

	if ((socketfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
    		perror("ftpConnect.socket()");	
        	exit(0);
    	}
	/*connect to the server*/
    	if(connect(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        	perror("ftpConnect.connect()");
		exit(0);
	}

	// connect to the server
	connect(socketfd, (struct sockaddr *) &server_addr, sizeof(server_addr));



	ftp->control_socket_fd = socketfd;
	ftp->data_socket_fd = 0;

	ftpRead(ftp, rd, sizeof(rd));

	return 0;
}

int ftpRead(ftp* ftp, char* str, size_t size) {
	FILE* fp = fdopen(ftp->control_socket_fd, "r");

	do {
		memset(str, 0, size);
		str = fgets(str, size, fp);
		printf("%s", str);
	} while (!('1' <= str[0] && str[0] <= '5') || str[3] != ' ');

	return 0;
}