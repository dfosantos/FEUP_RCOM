#include "dataLink.h"
#define BCC_ERROR_PROBABILITY 50	//Probabilidade de erro na leitura de BCC
#define BCC2_ERROR_PROBABILITY 0	//Probabilidade de erro na leitura de BCC2

int flag=0;
int TIMEOUT=0;
int Nr=0;



void time_out() {
	printf("Timeout number: %d\n", TIMEOUT+1);
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
        printf("Esperando emissor...\n");
    }
	else
		printf("Esperando recetor... ");
    while(TIMEOUT<=3) {

        if(com_type) {
			
            send_SET(fd);
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
                } else if(c == FLAG) {  //if FLAG received stay in the same state
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
                } else if(c == FLAG) {  //if FLAG received go back to previous state
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
                        send_UA(fd, com_type);
                    }
                    printf("Ligação Estabelecida\n");
                    return 1;
                } else {
                    state = 0;//else go back to beggining
                }
                break;
            }
        }
    }

    printf("TIMEOUT - Ligação Não Estabelecida\n");
    return -1;
}
FILE *openfile(char* filename, int com_type){

	FILE *file;

	if(com_type) file=fopen(filename, "rb");
	
	
	else file=fopen(filename, "wb");
	
	if(file==NULL){
		printf("Erro a abrir o ficheiro %s\n", filename);
		return NULL;
	}
	
	return file;
}
int LLWRITE(int fd, char *buffer, int length) {
		
    fflush(NULL);
    TIMEOUT = 0;
    char *trama = malloc((length+5)*sizeof(char));
    char controlo, controloREJ;
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

	
    
    //ESPERAR PELO ACK
    (void) signal(SIGALRM, time_out);
    TIMEOUT = 0;
    while(TIMEOUT<=3) {
		
		written = write(fd, trama, length + 5);
		written = written-5;
	
		
        alarm(3);
        flag=0;
		state=0;
        while(state != 5 && flag==0 ) {

			printf("flag == %d\n",flag);
            read(fd, &c, 1);
			
	//printf("error not here\n");
            switch (state) {
            case 0://expecting flag
                if(c == FLAG) {
                    state = 1;
                }//else stay in same state
                break;
            case 1://expecting A

                if(c == A_T) {
                    state = 2;
                } else if(c == FLAG) { //if FLAG received stay in the same state
                    state = 1;

                } else
                    state=0;//else go back to beggining
                break;

            case 2:

                if(c == RR0 || c == RR1) {//Expecting RR
					
                    state = 3;
				}
				else if( c == REJ0 || c == REJ1){
					printf("REJ recebido\n");
					state = 0;
					flag = 1;
				
                } else if(c == FLAG) {  //if FLAG received go back to previous state
                    state = 1;
                } else {//else go back to beggining
                    state = 0;
                }
                break;
            case 3://Expecting BCC
                if (c == (A_T^controlo)) {
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

    int length=0;
	int random;
	int state = 0;
	unsigned char c;
	char controlo;
	
   if(Nr==0) {		
        
        controlo = RR0;
				
		
    }
    else if(Nr==1) {
        
        controlo = RR1;
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
		            } else if(c == FLAG) { //if FLAG received stay in the same state
		                state = 1; 
						
		            } else
		                state=0;//else go back to beggining
		            break;

		        case 2://Expecting RR
					
		            if(c == RR0 || c == RR1) {
		                state = 3;
		            } else if(c == FLAG) { //if FLAG received go back to previous state
		                state = 1;
		            } else {//else go back to beggining
		                state = 0;
		            }
		            break;
		        case 3://Expecting BCC
				
					if(BCC_ERROR_PROBABILITY != 0){
						random = rand();
						random = (random % (100/BCC_ERROR_PROBABILITY)) + 1;
						//printf("random = %d\n",random);
						if(random == 1)
							c = !c;
					}
					
		            if (c == ((A_T^controlo))) {
		                state = 4;
		            } else {
						printf("Erro no BCC\n");
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
					
					
		            break;
		        }
		    }
   	

return length;



}

int LLCLOSE(int fd, int com_type) {

	char address;
	int state = 0;
	char c;
	int receiveUA = 0;
	
	
	if(com_type)
		send_DISC(fd, com_type);
		
	
	
	
	while(TIMEOUT<=3 || !com_type) {
		
		if(com_type) {
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
                else{
                    address=A_T;
                }
                if(c == address) {
                    state = 2;
				}else if(c==FLAG){ //if FLAG received stay in same state
                    state = 1;

                } else
                    state=0;//else go back to beggining
                break;

            case 2:
              
                if(c == DISC) { //expecting DISC
                    state = 3;
                } else if(c == FLAG) {  //if FLAG received go back to previous state
                    state = 1;
                } 
				else if(c == UA && receiveUA == 1){
					state = 3;
				} else {//else go back to beggining
                    state = 0;
                }
                break;
            case 3://Expecting BCC
                if (c == address^DISC) {
                    state = 4;
                } 
				else if(receiveUA == 1 && c == UA^address){
					state == 4;
				}
				else {
                    state = 0;//else go back to beggining
                }
                break;
            case 4://Expecting FLAG
                if (c == FLAG) {
                    state = 5;
                    if (!com_type && receiveUA == 0) {
                        send_DISC(fd, com_type);
						
						state = 0;
						receiveUA = 1;
					}
					else if(com_type){
						send_UA(fd, com_type);
					
						printf("Ligação encerrada!\n");
					return 1; 
					}
					else if(receiveUA == 1){
						
						printf("Ligação encerrada!\n");
						return 1;
					}
					
                } else {
                    return -1;
                }
                break;
            }
        }
    }



    return -1;
}

void send_UA(int fd, int com_type) {

    char trama_UA[5];
    trama_UA[0]=FLAG;
	if(com_type){
		trama_UA[1]=A_T;	
	}
	else{
		trama_UA[1]=A_R;
	}
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

void send_DISC(int fd, int com_type) {

    char trama[5];
    trama[0]=FLAG;
	if(com_type)
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

void send_RR(int fd){
	
	char controlo;
	if(Nr==0) {
        
		Nr = 1;
        controlo = RR0;
    }
    else if(Nr==1) {
       	Nr = 0;
        controlo = RR1;
    }
	
	char trama[5];
    trama[0]=FLAG;
    trama[1]=A_T;
    trama[2]=controlo;
    trama[3]=(A_T^controlo);
    trama[4]=FLAG;

    write(fd, trama, 5);
    fflush(NULL);
	
	
}
void send_REJ(int fd){
	char controlo;
	if(Nr==0) {
        
				Nr=!Nr;
        controlo = REJ0;
    }
    else if(Nr==1) {
       Nr=!Nr;
        controlo = REJ1;
    }
	
	char trama[5];
    trama[0]=FLAG;
    trama[1]=A_T;
    trama[2]=controlo;
    trama[3]=(A_T^controlo);
    trama[4]=FLAG;

    write(fd, trama, 5);
    fflush(NULL);
	
}

unsigned char* verify_bcc2(unsigned char* control_message, int* length){
	unsigned char* destuffed_message = destuffing(control_message, length);

	int i=0;
	int random;
	unsigned char control_bcc2 = 0x00;
	for(; i<*length-1; i++){
		control_bcc2 ^= destuffed_message[i];
		
	}
	if(BCC2_ERROR_PROBABILITY != 0){
						random = rand();
						random = (random % (100/BCC2_ERROR_PROBABILITY)) + 1;
					
						if(random == 1)
							control_bcc2 = !control_bcc2;
					}
	if(control_bcc2 != destuffed_message[*length-1]){
		*length = -1;
		return NULL;
	}
	*length = *length-1;
	unsigned char* data_message = (unsigned char*) malloc(*length);
	for(i=0; i<*length; i++){
			data_message[i] = destuffed_message[i];
	}
	free(destuffed_message);
	

	return data_message;
}

unsigned char* destuffing(unsigned char* msg, int* length){
	
	unsigned int array_length = 133;
	unsigned char* str = (unsigned char*) malloc(array_length);
	int i=0;
	int new_length = 0;

	for(; i<*length; i++){
		new_length++;
		if(new_length >= array_length){
			array_length = array_length+ (array_length/2);
			str = (unsigned char *) realloc(str, array_length);
		}
		if(msg[i] == 0x7d){
			if(msg[i+1] == 0x5e){
				str[new_length-1] = FLAG;
				i++;
			}
	 		else if(msg[i+1] == 0x5d){
				str[new_length -1] = 0x7d;
				i++;
			}
		}
		else{
			str[new_length-1] = msg[i];
		}

	}
	*length = new_length;
	
	return str;
}

unsigned char* stuffing(unsigned char* msg, int* length){
	
	unsigned char* str;
	unsigned char* msg_aux;
	int i=0;
	int j=0;
	unsigned int array_length = *length;

	str = (unsigned char *) malloc(array_length);
	msg_aux = (unsigned char *) malloc(array_length);
	char BCC2=0x00;
	for(; i < *length; i++, j++){
		BCC2^=msg[i];
		msg_aux[i] = msg[i];
	}

	array_length++;
	msg_aux = (unsigned char*) realloc(msg_aux,array_length);
	msg_aux[array_length-1/*index = length-1*/]=BCC2;
	
	for(i=0,j=0; i < *length+1; i++, j++){
		
		if(j >= array_length){
			array_length = array_length+(array_length/2);
			str = (unsigned char*) realloc(str, array_length);

		}
		if(msg_aux[i] ==  FLAG){
			str[j] = 0x7d;
			str[j+1] = 0x5e;
			j++;
		}
		else if(msg_aux[i] == 0x7d){
			str[j] = 0x7d;
			str[j+1]= 0x5d;
			j++;
		}
		else{
			str[j] = msg_aux[i];
		}
	}
	


	
	*length=j;
	
	return str;
}

long getFileSize(FILE* file) {

	long currentPosition = ftell(file);

	
	if (fseek(file, 0, SEEK_END) == -1) {
		printf("ERROR: Could not get file size.\n");
		return -1;
	}

	
	long size = ftell(file);

	
	fseek(file, 0, currentPosition);

	
	return size;
}
