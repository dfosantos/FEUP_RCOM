#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include <netinet/in.h>

typedef struct FTP
{
    int fd_control; 
    int fd_data; 
} ftp;

int connectToSocket(ftp* ftp, const char* ip, int port);
int login(ftp* ftp, const char* user, const char* password);
int retrieve(ftp* ftp, const char* filename);
int download(ftp* ftp, const char* filename);
int changeDirectory(ftp* ftp, const char* path);
int passiveMode(ftp* ftp);
int disconnect(ftp* ftp);
int receiveCommand(ftp* ftp, char* str, size_t size);
int sendCommand(ftp* ftp, const char* str, size_t size);
