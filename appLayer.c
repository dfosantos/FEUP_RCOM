/*Non-Canonical Input Processing*/
#include "appLayer.h"
#include "dataLink.c"
#define CHUNK_SIZE 50	//Número de caracteres do ficheiro a ser enviado de cada vez
extern int Nr; //Variável que controla RR0/RR1 
extern int Ns; //Variável que controla REJ0/REJ1




int main(int argc, char** argv) {
    fflush(NULL);
    int fd;
	
	//Tipo de comunicação: Sender (1) or Receiver (0)
	int com_type;

	//status = 0	||	START
	//status = 1	||	MIDDLE
	//status = 2	||	STOP
	int status = 0;
  
    struct termios oldtio,newtio;
 
	
    if ( (argc < 2) ||
            (
			(strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS1", argv[1])!=0) )) {
        printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
        exit(1);
    }
	
	if((argc < 3) ||
			 ( (strcmp("sender", argv[2])!=0) && (strcmp("receiver", argv[2])!=0))){
				printf("Specify if 'sender' or 'receiver'\n");
				exit(1);
			}

	else{
		if(strcmp("sender", argv[2])==0){
			com_type = 1;
		}
		else if(strcmp("receiver", argv[2])==0){
			com_type = 0;
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

    printf("New termios structure set\n");

    fflush(NULL);
	
	
/*-------------------------------------------------------------
---------------------------------------------------------------
---------------------------------------------------------------
----------------Início da Comunicação--------------------------
---------------------------------------------------------------
---------------------------------------------------------------
---------------------------------------------------------------
*/
  	if(LLOPEN(fd , com_type)==-1) return -1;		//Estabelecimento da comunicação
	
	FILE *file;
	
	
		file=openfile("penguin.gif", com_type);
	
	
    
    if(com_type) {
        fflush(NULL);
        //Preparar buffer start
		
      
		char buffer[CHUNK_SIZE];
		char *stuffed;
		int size;
		
		int h=0;
		printf("A enviar...\n\n");
		while ( (size = fread(buffer, sizeof(char), CHUNK_SIZE, file)) > 0){

				
				
				stuffed = byte_stuffing(buffer, &size);

		//		printf("Stuffed: %s\nSize: %d", stuffed, size);
				LLWRITE(fd, stuffed, size);
				

		}		
        
    }
	
	if(!com_type){

		char buffer [CHUNK_SIZE];
		char *destuffed;
		
		int length;
		
		while( (length = LLREAD(fd, buffer) )> 0){

			
			destuffed = byte_destuffing(buffer, &length);
	
				send_RR(fd);			
			
			fwrite(destuffed,1,length,file);
			

			if(length<CHUNK_SIZE)break;
		}	

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
