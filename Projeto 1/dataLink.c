#include "dataLink.h"

int flag=0;
int timeout=0;
int Nr=0;



void time_out() {
	timeout++;
    flag=1;
}

int LLOPEN(int fd, int com_type) {

    if(com_type)
    (void) signal(SIGALRM, time_out);
    char address, address2;
    char c;								
    int state = 0;

    if(!com_type) {
        printf("Esperando emissor...\n");
    }
	else
		printf("Esperando recetor...\n");
    while(timeout<=TIMEOUTS) {

        if(com_type) {
			
            send_SET(fd);
            alarm(3);
			flag=0;
        }


        while(state != 5 && flag==0 ) {

            read(fd, &c, 1);


            switch (state) {
                
            case 0:									//FLAG
                if(c == FLAG) {
                    state = 1;
                }//else stay in same state
                break;
            case 1:									//A_R/A_T
                if(com_type) {
                    address=A_R;
                }
                else {
                    address=A_T;
                }
                if(c == address) {
                    state = 2;
                } else if(c == FLAG) {  
                    state = 1;

                } else
                    state=0;
                break;

            case 2:									//UA/SET
                if(com_type) {
                    address2=UA;
                }
                else {
                    address2=SET;
                }
                if(c == address2) {
                    state = 3;
                } else if(c == FLAG) { 
                    state = 1;
                } else {
                    state = 0;
                }
                break;
            case 3:									//BCC
                if (c == (address^address2)) {
                    state = 4;
                } else {
                    state = 0;
                }
                break;
            case 4:									//FLAG
                if (c == FLAG) {
                    state = 5;
                    if (!com_type) {
                        send_UA(fd, com_type);
                    }
                    printf("Ligação Estabelecida\n");
                    return 1;
                } else {
                    state = 0;
                }
                break;
            }
        }
    }

    printf("timeout - Ligação Não Estabelecida\n");
    return -1;
}

int LLWRITE(int fd, char *buffer, int length) {
		
    fflush(NULL);
    timeout = 0;
    char *trama = malloc((length+5)*sizeof(char));
    char controlo;
    char c;
    int i, written, state = 0;

    if(Nr==0) {
        Nr=1;
        controlo = RR0;
    }
    else if(Nr==1) {
        Nr=0;
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
    timeout = 0;
    while(timeout<=TIMEOUTS) {
		
		written = write(fd, trama, length + 5);
		written = written-5;
	
		
        alarm(3);
        flag=0;
		state=0;
        while(state != 5 && flag==0 ) {

			
            read(fd, &c, 1);
			

            switch (state) {
            case 0:										//FLAG
                if(c == FLAG) {
                    state = 1;
                }//else stay in same state
                break;
            case 1:										//A_T/A_R

                if(c == A_T) {
                    state = 2;
                } else if(c == FLAG) { 
                    state = 1;

                } else
                    state=0;//else go back to beggining
                break;

            case 2:										//RR/REJ

                if(c == RR0 || c == RR1) {
					
                    state = 3;
				}
				else if( c == REJ0 || c == REJ1){
					
					state = 0;
					flag = 1;
				
                } else if(c == FLAG) {  				//FLAG
                    state = 1;
                } else {
                    state = 0;
                }
                break;
            case 3:										//BCC
                if (c == (A_T^controlo)) {
                    state = 4;
                } else {
                    state = 0;
                }
                break;
            case 4:										//FLAG
                if (c == FLAG) {
                    state = 5;
                    return written;

                } else {
                    state = 0;
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
	char c;
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
		        case 0:									//FLAG
		            if(c == FLAG) {
		                state = 1;
		            }
		            break;
		        case 1:									//A

		            if(c == A_T) {
		                state = 2;
		            } else if(c == FLAG) {
		                state = 1; 
						
		            } else
		                state=0;
		            break;

		        case 2:									//RR
					
		            if(c == controlo) {
		                state = 3;
		            } else if(c == FLAG) { 
		                state = 1;
		            } else {
		                state = 0;
		            }
		            break;
		        case 3:									//BCC
				
					if(BCC_ERROR_PROBABILITY != 0){		//Introdução manual de erro de leitura
						random = rand();
						random = (random % (100/BCC_ERROR_PROBABILITY)) + 1;

						if(random == 1)
							c = !c;
					}
					
		            if (c == ((A_T^controlo))) {
		                state = 4;
		            } else 
		                state = 0;//else go back to beggining
		            
		            break;
		        case 4:									//FLAG
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
if(TRANSMISSION_DELAY)   	
	delay(TRANSMISSION_DELAY*2);
return length;



}

int LLCLOSE(int fd, int com_type) {

	char address;
	int state = 0;
	char c;
	int receiveUA = 0;
	
	
	if(com_type)
		send_DISC(fd, com_type);
		
	
	
	
	while(timeout<=TIMEOUTS || !com_type) {
		
		if(com_type) {
			alarm(3);
			flag=0;
        }


        while(state != 5 && flag==0 ) {
			
            read(fd, &c, 1);
		
            switch (state) {
            case 0:									//FLAG
                if(c == FLAG) {
                    state = 1;
                }
                break;
            case 1:									//A
                if(com_type) {
                    address=A_R;
                }
                else if(!com_type){
                    address=A_T;
                }
                if(c == address) {
                    state = 2;
				}else if(c==FLAG){ 
                    state = 1;

                } else
                    state=0;
                break;

            case 2:
              
                if(c == DISC) { 					//DISC
                    state = 3;
                } else if(c == FLAG) { 
                    state = 1;
                } 
				else if(c == UA && receiveUA == 1){
					state = 3;
				} else {
                    state = 0;
                }
                break;
            case 3:									//BCC
			
                if (c == (address^DISC)) {
                    state = 4;
                } 
				else if((receiveUA == 1) && (c == (UA^address))){
					
					state = 4;
				}
				else {
                    state = 0;
                }
				
                break;
            case 4:									//FLAG
			
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
	char address;
    trama_UA[0]=FLAG;
	if(com_type){
		address = A_T;
		trama_UA[1]=address;	
	}
	else{
		address = A_R;
		trama_UA[1] = address;
	}
    trama_UA[2]=UA;
    trama_UA[3]=address^UA;
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
	char address;
    trama[0]=FLAG;
	if(com_type){
		address = A_T;
		trama[1] = address;
	}
	else{
		address = A_R;
		trama[1] = address;
	}
    trama[2]=DISC;
    trama[3]=address^DISC;
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
        
        controlo = REJ0;
    }
    else if(Nr==1) {
       
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

char* verify_bcc2(char* control_message, int* length){

	char* destuffed_message = destuffing(control_message, length);
	
	int i;
	int random;
	char control_bcc2 = 0x00;
	for(i=0; i<*length-1; i++){
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
	char* data_message = (char*) malloc(*length);
	for(i=0; i<*length; i++){
			data_message[i] = destuffed_message[i];
	}
	free(destuffed_message);
	

	return data_message;
}

char* destuffing(char* msg, int* length){
	

	char* str = (char*) malloc(*length);
	int i;
	int new_length = 0;

	
	
	for(i=0; i<*length; i++){
		new_length++;
	
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

char* stuffing(char* payload, int* length){
	
	char* str;
	char* msg_aux;
	int i;
	int j;
	
	
	str = (char *) malloc(*length);
	msg_aux = (char *) malloc(*length);
	char BCC2=0x00;
	
	for(i=0,j=0; i < *length; i++, j++){
		BCC2^=payload[i];
		msg_aux[i] = payload[i];
	}

	
	msg_aux = (char*) realloc(msg_aux,(*length)+1);
	msg_aux[*length/*index = length-1*/]=BCC2;

	for(i=0,j=0; i < *length+1; i++, j++){
		

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

char* control_frame(char* filename, FILE *file, int start, int* frame_size){
	
	
	int file_name_size	= strlen(filename);
	int file_size = getFileSize(file);							//Get file size
	if(start)
		printf("\nFile size = %d bytes\n\n",file_size);	
	int i = 0;
	char file_size_in_string[30];
	sprintf(file_size_in_string, "%d", file_size);
	
	*frame_size = 5 + file_name_size + strlen(file_size_in_string);
	char *control_frame = malloc(*frame_size);
	
	if(start)
		control_frame[i++] = START;
	else
		control_frame[i++] = END;
	
	
	control_frame[i++] = 0x00;
	control_frame[i++] = (char)strlen(file_size_in_string);

	
	for(; i < strlen(file_size_in_string)+3 ; i++){
		
		control_frame[i] = file_size_in_string[i-3];		
	}
	
	
	control_frame[i++] = 0x01;
	control_frame[i++] = (char) file_name_size;
	
	int j;
	for( j=i ; i<file_name_size+j ; i++ ){
		
		control_frame[i] = filename[i-j];
	}	
	
	return control_frame;
}

char* get_info(char* control, int* file_size){
	
	if(control[0]!=0x01)return NULL;
	int pos = 4+control[2];
	int filename_size = control[4+control[2]];
	
	char *buffer = malloc(100);
	
	char* size = malloc(control[2]);
	int i;
	
	for(i=0 ; i < filename_size ; i++ ){
		buffer[i] = control[pos+1+i];
	}
	
	
	for(i=0 ; i < control[2] ; i++ )
		size[i] = control[i+3];
	
	
	*file_size = atoi(size);
	return buffer;
}

char* header(char* buffer, int* length, short sequence_number){
	
	char* str = malloc((*length)+4);
	
	str[0] = 0x00; 
	str[1] = (char)sequence_number;
	str[2] = (char)(*length)/256;
	str[3] = (char)(*length)%256;
	
	int i;
	for(i = 0 ; i < *length ; i++ ){
		str[i+4] = buffer[i];
	}

	*length = *length + 4;
	return str;
	
}

char* remove_header(char* buffer, int* length){
	
	char* str = malloc(2 * (*length));
	int i;
	

	
	for(i = 0 ; i < *length - 4 ; i++)
		str[i] = buffer[i+4];
	
	*length = *length-4;
	return str;
	
	
}
