#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <netdb.h> 
#include <sys/types.h>
#include <netinet/in.h> 
#include <string.h>
#include <netdb.h>
#include <regex.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>


typedef struct URL {
	char* user; // string to user
	char* password; // string to password
	char* host; // string to host
	char* ip; // string to IP
	char* path; // string to path
	char* filename; // string to filename
	int port; // integer to port
} url;

int getIp(url* url);

char* copyStringTillChar(char* str, char chr) ;

int processURL(url*, char* urlName);
