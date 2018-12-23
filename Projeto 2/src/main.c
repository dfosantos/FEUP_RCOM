#include "URL.h"
#include "FTP.h"

int main(int argc, char** argv){


	if(argc != 2){
		printf("Wrong number of arguments\n");
		return 1;
	}

	url url;
	initURL(&url);

	if(parseURL(&url,argv[1]) != 0){
		perror("Error parsing URL\n");
		return 1;
	}

	getIpByHost(&url);


	ftp ftp;

	if(connectToSocket(&ftp, url.ip, url.port) != 0){
		perror("Connection error\n");
		return 1;
	}

	const char* user = strlen(url.user) ? url.user : "anonymous";
	char* password;
	if (strlen(url.password)) {
		password = url.password;
	} else {
		char buf[100];
		printf("Default username: anonymous\n");
		printf("Password: ");
		while (strlen(fgets(buf, 100, stdin)) < 1)
			printf("\nIncorrect input, please try again: ");
		password = (char*) malloc(strlen(buf));
		strncat(password, buf, strlen(buf) - 1);
	}

	if(login(&ftp,user,password) != 0){
		perror("Login error\n");
		return 1;
	}

	if(passiveMode(&ftp) != 0){
		perror("Couldn't enter passive mode\n");
		return 1;
	}

	if(changeDirectory(&ftp,url.path) != 0){
		perror("Couldn't change directory\n");
		return 1;
	}

	if(retrieve(&ftp,url.filename)!= 0){
		perror("Error retrieving file\n");
		return 1;
	}

	if(download(&ftp,url.filename)!= 0){
		perror("Download error\n");
		return 1;
	}
	
		
	if(disconnect(&ftp)!= 0){
		perror("Disconnect Error\n");
		return 1;
	}
	printf("Download complete\n");
	return 0;
}
