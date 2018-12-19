#include <stdio.h>
#include "URL.h"
#include "FTP.h"

int main(int argc, char *argv[]){
        
        
        /************* URL PROCESS *************/

        url url;

	url.port = 21;

        if( processURL( &url, argv[1]) )
                return -1;


        if(getIp(&url)) {
                printf("ERROR: Cannot find ip \n");
                return -1;
        } 


	/************* FTP CLIENT PROCESS *************/

        return 0;

}
