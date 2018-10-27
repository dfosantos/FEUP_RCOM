/*Non-Canonical Input Processing*/
#include "main.h"




#define COM_TYPE RECEIVER

volatile int STOP=FALSE;
int flag=0;
int TIMEOUT=0;
int rej=0;
int isStart=0;
int isClose=0;


void time_out() {
    TIMEOUT++;
    flag=1;

}

int LLOPEN(int fd, int com_type) {

    if(com_type)
    (void) signal(SIGALRM, time_out);
    char address, address2;
    unsigned char c;//last char received
    int state = 0;

    if(!com_type) {
        printf("Receiving SET...\n");
    }

    while(TIMEOUT<3) {

        if(com_type) {
            send_SET(fd);
            printf("Receiving UA...\n");
            alarm(3);
            flag=0;
        }


        while(state != 5 && flag==0 ) {

            read(fd, &c, 1);


            switch (state) {
                
            case 0://expecting flag
                if(c == FLAG) {
                    state = 1;
                }//else stay in same state
                break;
            case 1://expecting A
                if(com_type) {
                    address=A_R;
                }
                else {
                    address=A_T;
                }
                if(c == address) {
                    state = 2;
                } else if(c == FLAG) { //if not FLAG instead of A
                    state = 1;

                } else
                    state=0;//else stay in same state
                break;

            case 2:
                if(com_type) {
                    address2=UA;
                }
                else {
                    address2=SET;
                }
                if(c == address2) {
                    state = 3;
                } else if(c == FLAG) { //if FLAG received
                    state = 1;
                } else {//else go back to beggining
                    state = 0;
                }
                break;
            case 3://Expecting BCC
                if (c == address^address2) {
                    state = 4;
                } else {
                    state = 0;//else go back to beggining
                }
                break;
            case 4://Expecting FLAG
                if (c == FLAG) {
                    state = 5;
                    if (!com_type) {
                        send_UA(fd);
                    }
                    printf("Ligacao Estabelecida! :)\n");
                    return fd;
                } else {
                    state = 0;//else go back to beggining
                }
                break;
            }
        }
    }

    printf("TIMEOUT - Ligacao Não Estabelecida\n");
    return -1;
}
void send_UA(int fd) {

    char trama_UA[5];
    trama_UA[0]=FLAG;
    trama_UA[1]=A_R;
    trama_UA[2]=UA;
    trama_UA[3]=A_R^UA;
    trama_UA[4]=FLAG;

    write(fd, trama_UA, 5);
    fflush(NULL);

}
void send_SET(int fd) {

    char trama[5];
    trama[0]=FLAG;
    trama[1]=A_T;
    trama[2]=SET;
    trama[3]=A_T^SET;
    trama[4]=FLAG;

    write(fd, trama, 5);
    fflush(NULL);


}

int LLWRITE(int fd, char *buffer, int length) {
    fflush(NULL);
    TIMEOUT = 0;
    char *trama = malloc((length+5)*sizeof(char));
    char controlo;
    char c;
    int i, written, state = 0;

    if(rej==0) {
        rej==1;
        controlo = RR1;
    }
    else if(rej==1) {
        rej=0;
        controlo = RR0;
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

            case 2://Expecting C_SET

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


int LLREAD(int fd, char *buffer) {

    int length=0;;
	int state = 0;
	unsigned char c;
	char controlo;
	
   if(rej==0) {		
        rej=1;
        controlo = RR1;
				
		
    }
    else if(rej==1) {
        rej=0;
        controlo = RR0;
    }

		
		while(state != 5) {

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

		        case 2://Expecting C_SET

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
					if( c == FLAG){
						state = 5;
					}
					else{
						buffer[length] = c;		
						length++;			
					}
					
					//printf("buffer[%d] : %c\n", length , buffer[length]);
		            break;
		        }
		    }
   	

return length;

}

void send_DISC(int fd) {

    char trama[5];
    trama[0]=FLAG;
	if(COM_TYPE)
		trama[1]=A_T;
	else{
		trama[1] = A_R;
	}
    trama[2]=DISC;
    trama[3]=A_T^DISC;
    trama[4]=FLAG;

    write(fd, trama, 5);
    fflush(NULL);

}
int LLCOSE(int fd) {

	char address;
	int state = 0;
	char c;
	if(COM_TYPE){
		send_DISC(fd);
	}
	
	if(COM_TYPE)
	while(TIMEOUT<3 || !COM_TYPE) {
		if(COM_TYPE) {
            
            printf("Receiving DISC...\n");
			
			if(COM_TYPE){
	            alarm(3);
			}

            flag=0;
        }


        while(state != 5 && flag==0 ) {

            read(fd, &c, 1);


            switch (state) {
            case 0://expecting flag
                if(c == FLAG) {
                    state = 1;
                }//else stay in same state
                break;
            case 1://expecting A
                if(COM_TYPE) {
                    address=A_R;
                }
                else {
                    address=A_T;
                }
                if(c == address) {
                    state = 2;
                } else if(c == FLAG) { //if not FLAG instead of A
                    state = 1;

                } else
                    state=0;//else stay in same state
                break;

            case 2:
              
                if(c == DISC) {
                    state = 3;
                } else if(c == FLAG) { //if FLAG received
                    state = 1;
                } else {//else go back to beggining
                    state = 0;
                }
                break;
            case 3://Expecting BCC
                if (c == address^DISC) {
                    state = 4;
                } else {
                    state = 0;//else go back to beggining
                }
                break;
            case 4://Expecting FLAG
                if (c == FLAG) {
                    state = 5;
                    if (!COM_TYPE) {
                        send_DISC(fd);
						                   
					}
					else if(COM_TYPE){
					send_UA(fd);
					printf("Ligação encerrada!");
					return 1; 
					}
                } else {
                    state = 0;//else go back to beggining
                }
                break;
            }
        }
    }



    return -1;
}
FILE *openfile(char* filename){

	FILE *file;
	if(COM_TYPE) file=fopen(filename, "rb");
	else file=fopen(filename, "wb");
	
	if(file==NULL){
		printf("Erro a abrir o ficheiro %s", filename);
		return NULL;
	}
	
	return file;
}

int main(int argc, char** argv) {
    fflush(NULL);
    int fd,c, res;


    /*«	//OPEN FILE
    	FILE *ptr;
    	ptr = fopen("penguin.gif","rb");
    	unsigned char *buffer;
    	unsigned long int filesize;

    	fseek(ptr,0,SEEK_END);
    	fread(buffer,   , 1 , ptr);
    */
    struct termios oldtio,newtio;
    char buf[255];
    int i, sum = 0, speed = 0;

    if ( (argc < 2) ||
            ((strcmp("/dev/ttyS0", argv[1])!=0) &&
             (strcmp("/dev/ttyS1", argv[1])!=0) )) {
        printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
        exit(1);
    }


    /*
      Open serial port device for reading and writing and not as controlling tty
      because we don't want to get killed if linenoise sends CTRL-C.
    */


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



    /*
      VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
      leitura do(s) próximo(s) caracter(es)
    */



    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");


    /*testing*/


    fflush(NULL);
   	if(LLOPEN(fd , 1)==-1) return -1;
	
    //IMPLEMENTAR DIVISÃO OCTETOS
   /* if(isStart) {
        fflush(NULL);
        //Preparar buffer start
        char teste [6];
        teste[0] = 'O';
        teste[1] = 'L';
        teste[2] = 'A';
        teste[3] = 'O';
        teste[4] = 'L';
        teste[5] = 'A';


        write(fd,teste,6);
        LLWRITE(fd,teste,6);
    }*/

    char buffer [20];
	int length = LLREAD(fd, buffer);
	int j;
	for(j=0 ; j<length ; j++){

		printf("char[%d] : %x\n", j , buffer[j]);
	}	
	
	//LLCLOSE(fd);



    /*
      O ciclo FOR e as instruções seguintes devem ser alterados de modo a respeitar
      o indicado no guião
    */




    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }




    close(fd);
    return 0;
}
