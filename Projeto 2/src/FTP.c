	#include "FTP.h"


int ftpConnect(ftp* ftp, const char* ip, int port) {
	int socketfd;
	char rd[1024];

	socketfd = connectSocket(ip, port);

	ftp->control_socket_fd = socketfd;
	ftp->data_socket_fd = 0;

	ftpRead(ftp, rd, sizeof(rd));

	return 0;
}
static int connectSocket(const char* ip, int port) {
	int sockfd;
	struct sockaddr_in server_addr;

	// server address handling
	bzero((char*) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip); 
	server_addr.sin_port = htons(port); 

	// open an TCP socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	// connect to the server
	connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr));

	return sockfd;
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