/*Non-Canonical Input Processing*/
#include "dataLink.h"


#define CHUNK_SIZE 50	//Número de caracteres do ficheiro a ser enviado de cada vez
extern int Nr; //Variável que controla RR0/RR1 




int main(int argc, char** argv) {
    fflush(NULL);
    int fd;
	
	//Tipo de comunicação: Sender (1) or Receiver (0)
	int com_type;
	
  
    struct termios oldtio,newtio;
 

    if ( (argc < 2) ||
            (
			(strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS1", argv[1])!=0) )) {
        printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
        exit(1);
    }
	
	if((argc < 3) ||
			 ( (strcmp("sender", argv[2])!=0) && (strcmp("receiver", argv[2])!=0))){
				printf("Specify if 'sender' or 'receiver' and file name\n");
				exit(1);
			}
	if(argc < 4 && strcmp(argv[2],"sender") == 0){
		printf("Specify file name\n");
		exit(1);
	}
	else{
		if(strcmp("sender", argv[2])==0){
			com_type = 1;
		}
		else if(strcmp("receiver", argv[2])==0){
			com_type = 0;
		}
		else{
			printf("Specify if 'sender' or 'receiver'\n");
		}
	}


    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {
        perror(argv[1]);
        exit(-1);
    }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
        perror("tcgetattr");
        exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 1;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 0;   /* blocking read until 5 chars received */



    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }

    //printf("New termios structure set\n\n");
    fflush(NULL);
	
	
/*-------------------------------------------------------------
---------------------------------------------------------------
---------------------------------------------------------------
----------------Início da Comunicação--------------------------
---------------------------------------------------------------
---------------------------------------------------------------
---------------------------------------------------------------
*/
	printf("\n");
  	struct timeval start, stop;
	double secs = 0;
	

	if(LLOPEN(fd , com_type)==-1) exit(1);		//Estabelecimento da comunicação
	gettimeofday(&start, NULL);

	FILE *file;
    
    if(com_type) {
			

		file=openfile(argv[3], com_type);
		
        fflush(NULL);
        
		
		int file_name_size = strlen(argv[3]);
		char buffer[CHUNK_SIZE];
		char *stuffed;
		int size;
		
				
		stuffed = stuffing(argv[3], &file_name_size);		//Stuff name
		if(LLWRITE(fd, stuffed, file_name_size)==-1)		//Send name
			exit(1);				
		
		
		int file_size=getFileSize(file);					//Get file size
		printf("\nFile size = %d bytes\n\n",file_size);		
		
		
	
		sprintf(buffer, "%d", file_size);
		size = (int)strlen(buffer);

		stuffed = stuffing(buffer, &size);			//Stuff size
		
		if(LLWRITE(fd, stuffed, size)==-1)			//Send size
			exit(1);				
		printf("A enviar...");

		
		while ( (size = fread(buffer, sizeof(char), CHUNK_SIZE, file)) > 0){
			
				stuffed = stuffing(buffer, &size);
				if(LLWRITE(fd, stuffed, size)==-1)
					exit(1);
		}		
		printf("Ficheiro enviado!\n");
		gettimeofday(&stop, NULL);	
		secs = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
		printf("\nEstatística:\n");
		printf("Tempo de envio = %.2fs\n",secs);
		printf("Débito binário = %.0f KB/s\n\n", file_size/secs);
        
    }
	
	if(!com_type){
		
		
		char buffer [CHUNK_SIZE];
		char *destuffed;
		int length;
		int received_file_size=0;
		//Read file name
		while(1){
			length = LLREAD(fd, buffer);	
		
			destuffed = verify_bcc2(buffer, &length);		
			if(destuffed == NULL)
					send_REJ(fd);
			else{
				send_RR(fd);
				printf("\nFile Name: %s\n",destuffed);
				file=openfile(destuffed, com_type);	
				break;
			}
			
		}
		
		//Read file size
		while(received_file_size == 0){
			length = LLREAD(fd, buffer);					
			destuffed = verify_bcc2(buffer, &length);
		
			if(destuffed == NULL)
					send_REJ(fd);
			else{
				send_RR(fd);
				received_file_size = atoi(destuffed);
				printf("Size: %d\n\n",received_file_size);				
			}
		}
		
	
		printf("A receber...\n\n");
		
		
		while( (length = LLREAD(fd, buffer) )> 0){
			
			destuffed = verify_bcc2(buffer, &length);
				
				if(destuffed == NULL)
					send_REJ(fd);
				else{
					send_RR(fd);
					fwrite(destuffed,1,length,file);
				}
				

			if(length<CHUNK_SIZE && length>0)break;
		}	
		
		printf("%ld de %d bytes recebidos\nFicheiro recebido com sucesso!\n\n", getFileSize(file),received_file_size);
		gettimeofday(&stop, NULL);		
		secs = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
		printf("\nEstatística:\n");
		printf("Tempo de envio = %.2fs\n",secs);
		printf("Débito binário = %.0f KB/s\n\n", received_file_size/secs);
		fclose(file);
		
	}
	
	
	LLCLOSE(fd, com_type);
	
	

   



    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }




    close(fd);
    return 0;
}
