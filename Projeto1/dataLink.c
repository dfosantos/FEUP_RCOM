
int flag=0;
int TIMEOUT=0;
int rej=0;

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
        printf("Esperando SET...\n");
    }

    while(TIMEOUT<3) {

        if(com_type) {
            send_SET(fd);
            printf("Esperando UA...\n");
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

int LLREAD(int fd, char *buffer) {

    int length=0;;
	int state = 0;
	unsigned char c;
	char controlo;
	
   if(rej==0) {		
        
        controlo = RR0;
				
		
    }
    else if(rej==1) {
        
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

int LLCLOSE(int fd, int com_type) {

	char address;
	int state = 0;
	char c;
	int receiveUA = 0;
	
	
	if(com_type){
		send_DISC(fd, com_type);
		printf("DISC enviado\n");
	}
	
	
	
	while(TIMEOUT<3 || !com_type) {
		
		if(com_type) {
            
            printf("À espera de DISC...\n");
			
			if(com_type){
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
                if(com_type) {
                    address=A_R;
                }
                else{
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
						printf("DISC enviado\nRecebendo UA...\n"); 
						state = 0;
						receiveUA = 1;
					}
					else if(com_type){
						send_UA(fd, com_type);
						printf("DISC recebido. UA enviado.\n");
						printf("Ligação encerrada!\n");
					return 1; 
					}
					else if(receiveUA == 1){
						printf("UA recebido\n");
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
	if(rej==0) {
        
        controlo = RR0;
    }
    else if(rej==1) {
       
        controlo = RR1;
    }
	
	char trama[5];
    trama[0]=FLAG;
    trama[1]=A_T;
    trama[2]=controlo;
    trama[3]=A_T^controlo;
    trama[4]=FLAG;

    write(fd, trama, 5);
    fflush(NULL);
	
}





