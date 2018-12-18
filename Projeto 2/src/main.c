#include "URL.h"
#include "FTP.h"

#define SERVER_PORT 21
#define SERVER_ADDR "192.168.28.96"
#define STRING_LENGTH 50

int main(int argc, char** argv){

  if(argc != 2){
		printf("Only 1 argument is allowed in the following format:\n");
		printf("As a user: ftp://[<user>:<password>@]<host>/<url-path>\n");
		printf("Anonymous Usage: ftp://<host>/<url-path>\n\n");
		return 1;
	}


}
