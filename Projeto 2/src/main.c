#include <stdio.h>
#include "URL.h"
#include "FTP.h"

int main(int argc, char *argv[])
{
        char* user, password, ip, path, filename, host;
        int port=21;
        if( processURL( &user, &password, &host, &ip, &path, &filename, argv[1]) != 0)
                return -1;


        if(getIp(&host, &ip)) {
                printf("ERROR: Cannot find ip \n");
                return -1;
        }                    

        return 0;

}
