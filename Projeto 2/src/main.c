#include "URL.h"
#include "FTP.h"

#define SERVER_PORT 21
#define SERVER_ADDR "192.168.28.96"
#define STRING_LENGTH 50

int main(int argc, char** argv){

   	char host[STRING_LENGTH];
	memset(host, 0, STRING_LENGTH); //poe host a 0
	url url;
		url.port = SERVER_PORT;

    struct hostent *h;
    if (getIp(&url)) {
		printf("ERROR: Cannot find ip\n");
		return -1;
	}

	int	sockfd;
	struct	sockaddr_in server_addr;
	char	buf[] = "Mensagem de teste na travessia da pilha TCP/IP\n";  
	int	bytes;
	
	/*server address handling*/
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);	/*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(SERVER_PORT);		/*server TCP port must be network byte ordered */
    
	/*open an TCP socket*/
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
    		perror("socket()");
        	exit(0);
    	}
	/*connect to the server*/
    	if(connect(sockfd, 
	           (struct sockaddr *)&server_addr, 
		   sizeof(server_addr)) < 0){
        	perror("connect()");
		exit(0);
	}
    	/*send a string to the server*/
	bytes = write(sockfd, buf, strlen(buf));
	printf("Bytes escritos %d\n", bytes);

	close(sockfd);
	exit(0);
}
