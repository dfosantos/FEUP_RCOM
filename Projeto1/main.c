/*Non-Canonical Input Processing*/
#include "main.h"
#include "DataTransparency.h"
#include "dataLink.c"
#define CHUNK_SIZE 50 	//Número de caracteres do ficheiro a ser enviado de cada vez
extern int Nr; //Variável que controla RR0/RR1 
extern int Ns; //Variável que controla REJ0/REJ1



int LLWRITE(int fd, char *buffer, int length) {
		
    fflush(NULL);
    TIMEOUT = 0;
    char *trama = malloc((length+5)*sizeof(char));
    char controlo;
    char c;
    int i, written, state = 0;

    if(Nr==0) {
        
        controlo = RR0;
    }
    else if(Nr==1) {
        
        controlo = RR1;
    }


    //Prepara bytes iniciais
    trama[0] = FLAG;
    trama[1] = A_T;
    trama[2] = controlo;
    trama[3] = trama[1]^trama[2];
    trama[length+4] = FLAG ;

    for(i = 4 ; i < length + 4 ; i++) {
        trama[i]=buffer[i-4];
    }

	
    written = write(fd, trama, length + 5);
	written = written-5;
    //ESPERAR PELO ACK
    (void) signal(SIGALRM, time_out);
    TIMEOUT = 0;
    while(TIMEOUT<3) {

        alarm(3);
        flag=0;

        while(state != 5 && flag==0 ) {
			
            read(fd, &c, 1);
			

            switch (state) {
            case 0://expecting flag
                if(c == FLAG) {
                    state = 1;
                }//else stay in same state
                break;
            case 1://expecting A

                if(c == A_T) {
                    state = 2;
                } else if(c == FLAG) { //if not FLAG instead of A
                    state = 1;

                } else
                    state=0;//else stay in same state
                break;

            case 2://Expecting RR

                if(c == controlo) {
                    state = 3;
                } else if(c == FLAG) { //if FLAG received
                    state = 1;
                } else {//else go back to beggining
                    state = 0;
                }
                break;
            case 3://Expecting BCC
                if (c == A_T^controlo) {
                    state = 4;
                } else {
                    state = 0;//else go back to beggining
                }
                break;
            case 4://Expecting FLAG
                if (c == FLAG) {
                    state = 5;
                    return written;

                } else {
                    state = 0;//else go back to beggining
                }
                break;
            }
        }
    }
    free(trama);
    printf("TIMEOUT - Escrita não Realizada\n");
    return -1;

}



FILE *openfile(char* filename, int com_type){

	FILE *file;
	if(com_type) file=fopen(filename, "rb");
	else file=fopen(filename, "wb");
	
	if(file==NULL){
		printf("Erro a abrir o ficheiro %s", filename);
		return NULL;
	}
	
	return file;
}

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
	
	if(com_type){
		file=openfile("teste.txt", com_type);
	}
	
	
    
    if(com_type) {
        fflush(NULL);
        //Preparar buffer start
		
      
		char buffer[CHUNK_SIZE] = "\0";
		int size;
		int stuffing;

		while ( (size = fread(buffer, sizeof(char), CHUNK_SIZE, file)) > 0){
		
			
		
				if((stuffing = DataTransparency(buffer, size, com_type)) == 1){
					size=size+stuffing;
				}
				printf("String depois do stuffing : %s\n", buffer);

				int dataWritten = LLWRITE(fd,buffer,CHUNK_SIZE);
			
				if( dataWritten != CHUNK_SIZE){

					break;
				}

		}		
        
    }
	
	if(!com_type){

		char buffer [CHUNK_SIZE];
		
		int length;
		
		while( (length = LLREAD(fd, buffer) )> 0){

			printf("buffer antes do stuffing: %s\n", buffer);
			DataTransparency(buffer, length, com_type);
			printf("buffer depois do stuffing: %s\n", buffer);
			//IF para testar buffer
			send_RR(fd);
	

		}	


	
		printf("%s\n", buffer);
		
		
	}
	
	LLCLOSE(fd, com_type);



   



    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }




    close(fd);
    return 0;
}
