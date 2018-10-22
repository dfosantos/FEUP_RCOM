/*Non-Canonical Input Processing*/
#include "receiver.h"

int LLOPEN(int fd){	

	
	unsigned char c;//last char received
	int state = 0;
	printf("Receiving SET...\n");
  
	  	
  		
		while(state != 5){
  			
			read(fd, &c, 1);
			printf("State %d - char: 0x%X\n", state, c);
    	
 		   switch (state) {
 		     case 0://expecting flag
 		       if(c == FLAG){
 		         state = 1;
 		       }//else stay in same state
 		       break;
 		     case 1://expecting A
 		       if(c == A_R){
 		         state = 2;
 		       }else if(c == FLAG){//if not FLAG instead of A	
 		         state = 1;
 		       }else{
			 state=0;
		       }//else stay in same state
 		       break;
 		     case 2://Expecting C_SET
 		       if(c == SET){
 		         state = 3;
 		       }else if(c == FLAG){//if FLAG received
 		         state = 1;
 		       }else {//else go back to beggining
 		         state = 0;
 		       }
 		       break;
 		     case 3://Expecting BCC
 		       if (c == A_R^UA){
 		         state = 4;
 		       }else {
 		         state = 0;//else go back to beggining
 		       }
 		       break;
 		     case 4://Expecting FLAG
 		       if (c == FLAG){
 		         state = 5;
 		         printf("Ligacao Estabelecida! :)");
				 send_UA(fd);				
 		         return fd;
 		       }else{
 		         state = 0;//else go back to beggining
 		       }
 		       break;
 		   }
    	
   	 }
    
    return -1;
}

void send_UA(int fd){

	char trama_UA[5];
	trama_UA[0]=FLAG;
	trama_UA[1]=A_R;
	trama_UA[2]=UA;
	trama_UA[3]=A_R^UA;
	trama_UA[4]=FLAG;
	
	write(fd, trama_UA, 5);
	fflush(NULL);
	
}
int LLREAD(int fd, char *buffer){
	
	int tramaI=1;
 	int length;
	int res;
	int trama_UA[4];
	trama_UA[0]=FLAG;
	trama_UA[1]=A_R;
	trama_UA[2]=UA;
	trama_UA[3]=A_R^UA;
	trama_UA[4]=FLAG;
	
	
	
		if(res<0){
				tramaI=0;
			
				return -1;
		}
	
		return length;
}

int LLCOSE(int fd){
		
		if(DISC)
			return -1;
		if(DISC)
			return -1;
		if(UA)
			return -1;
	
		return 1;
}

int main(int argc, char** argv)
{
	fflush(NULL);
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];

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
    if (fd <0) {perror(argv[1]); exit(-1); }

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

	LLOPEN(fd);
	
	
	
	


  /* 
    O ciclo WHILE deve ser alterado de modo a respeitar o indicado no guião 
  */



    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}
