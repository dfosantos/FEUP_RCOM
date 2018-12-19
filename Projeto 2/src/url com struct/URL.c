#include "URL.h"

int getIp(url* url){

  struct hostent *h;

    if ((h=gethostbyname(url->host)) == NULL) {  
        herror("gethostbyname");
        return 1;
    }

    printf("Host name  : %s\n", h->h_name);
    printf("IP Address : %s\n",inet_ntoa(*((struct in_addr *)h->h_addr)));

	char* ipAddress = inet_ntoa(*((struct in_addr *) h->h_addr));
	strcpy(url->ip, ipAddress);

	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
char* copyStringTillChar(char* str, char chr) {
	// using temporary string to process substrings
	char* aux = (char*) malloc(strlen(str));

    strcpy(aux, strchr(str, chr));

	// calculating length to copy element
	int i = strlen(str) - strlen(aux);

	aux[i] = '\0'; // termination char in the end of string
	strncpy(aux, str, i);
	strcpy(str, str + strlen(aux) + 1);

	return aux;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////

int processURL(url*url, char* urlName){
	char* aux, *copyThis;
    char* pathName;

	copyThis = (char*) malloc(strlen(urlName));
	aux = (char*) malloc(strlen(urlName));

	memcpy(aux, urlName, strlen(urlName));

/*
*   ftp://<host>/<url-path>  <-normal user
*
*   ftp://[<user>:password>@]<host>/<url-path>  <-anonymous user
*/

// removing 'ftp://' from string
strcpy(aux, aux + 6);

if (aux[0] == '['){     
	
    //removing '['
    strcpy(aux, aux + 1);

    //copy username
    strcpy(copyThis, copyStringTillChar(aux, ':'));
    memcpy(url->user, copyThis, strlen(copyThis));

    //copy password
    strcpy(copyThis, copyStringTillChar(aux, '@'));
    memcpy(url->password, copyThis, strlen(copyThis));
    
    //removing ']'
    strcpy(aux, aux + 1);
}

	//copy host
	strcpy(copyThis, copyStringTillChar(aux, '/'));
	memcpy(url->host, copyThis, strlen(copyThis));

            //copy path
            pathName = (char*) malloc(strlen(aux));
            int startPath = 1;
            while (strchr(aux, '/')) {
                copyThis = copyStringTillChar(aux, '/');

                if (startPath) {
                    startPath = 0;
                    strcpy(pathName, copyThis);
                } else {
                    strcat(pathName, copyThis);
                }

                strcat(pathName, "/");
            }
            strcpy(url->path, pathName);

	// saving filename
	strcpy(url->filename, aux);

	free(aux);
	free(copyThis);

	return 0;
}