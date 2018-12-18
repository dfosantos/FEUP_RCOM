#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <netdb.h> 
#include <sys/types.h>
#include <netinet/in.h> 
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int getIp(char* host, char* ip);

char* copyStringTillChar(char* str, char chr); 

int processURL(char* user, char* password, char*host, char*ip, char*path, char*filename, char* urlName);
