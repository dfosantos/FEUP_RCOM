#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <strings.h>
#include <ctype.h> 

#define SERVER_PORT 21
#define SERVER_ADDR "192.168.28.96"
#define STRING_LENGTH 50

typedef char url_content[256];

typedef struct URL {
/*	url_content user; // string to user
	url_content password; // string to password
*/	url_content host; // string to host
	url_content ip; // string to IP
/*	url_content path; // string to path
	url_content filename; // string to filename
*/	int port; // integer to port
} url;

int main(int argc, char** argv){

   	char host[STRING_LENGTH];
	memset(host, 0, STRING_LENGTH); //poe host a 0

    struct hostent *h;
    h=getIp(host);

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

  int getIp(url* url){
    struct hostent *h;

    if ((h=gethostbyname(url->host)) == NULL) {  
            herror("gethostbyname");
			return 1;
        }

        printf("Host name  : %s\n", h->h_name);
        printf("IP Address : %s\n",inet_ntoa(*((struct in_addr *)h->h_addr)));
	char* ip = inet_ntoa(*((struct in_addr *) h->h_addr));
	strcpy(url->ip, ip);

	return 0;
}