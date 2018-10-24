/*Non-Canonical Input Processing*/
#include "transmitter.h"

volatile int STOP=FALSE;
int flag=0;
int TIMEOUT=0;

void time_out(){
	TIMEOUT++;
	flag=1;
	printf("timeout!!!!!\n");
	printf("FLAG=%d\n",flag);
}
int LLOPEN(int fd){	

	
    (void) signal(SIGALRM, time_out);

	unsigned char c;//last char received
	int state = 0;
	printf("Receiving UA...\n");
  

  	while(TIMEOUT<3){
  		send_SET(fd);
		alarm(3);  		
		flag=0;
  		printf("FLAG=%d, TIMEOUT=%d\n",flag,TIMEOUT);
  		
		while(state != 5 && flag==0 ){
  			
			
			if(read(fd, &c, 1)>0){
				printf("State %d - char: 0x%X\n", state, c);
    	
 		   switch (state) {
 		     case 0://expecting flag
 		       if(c == FLAG){
 		         state = 1;
 		       }//else stay in same state
 		       break;
 		     case 1://expecting A
 		       if(c == A_T){
 		         state = 2;
 		       }else if(c == FLAG){//if not FLAG instead of A
 		         state = 1;
				
 		       }else 
					state=0;//else stay in same state
 		       break;
 		     case 2://Expecting C_SET
 		       if(c == UA){
 		         state = 3;
 		       }else if(c == FLAG){//if FLAG received
 		         state = 1;
 		       }else {//else go back to beggining
 		         state = 0;
 		       }
 		       break;
 		     case 3://Expecting BCC
 		       if (c == A_T^UA){
 		         state = 4;
 		       }else {
 		         state = 0;//else go back to beggining
 		       }
 		       break;
 		     case 4://Expecting FLAG
 		       if (c == FLAG){
 		         state = 5;
 		         printf("Ligacao Estabelecida! :)");
 		         return fd; 
 		       }else{
 		         state = 0;//else go back to beggining
 		       }
 		       break;
 		   }
    	}
   	 }
    }
    printf("TIMEOUT - Ligacao Não Estabelecida\n");
    return -1;
}

void send_SET(int fd){

	char trama_SET[5];
	trama_SET[0]=FLAG;
	trama_SET[1]=A_T;
	trama_SET[2]=SET;
	trama_SET[3]=A_T^SET;
	trama_SET[4]=FLAG;
	
	write(fd, trama_SET, 5);
	fflush(NULL);
	
	
}

int LLWRITE(int fd, char *buffer, int length){
	
	int n;
	int tramaI=1;
 
	int res= write(fd, buffer, length);

		if(res<0){
				tramaI=0;
				return -1;
		}
	
		return n;
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


int main(int argc, char** argv){
	fflush(NULL);
    int fd,c, res;

/*	//OPEN FILE 	
	FILE *ptr;
	ptr = fopen("penguin.txt","rb");
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

    
    /*testing*/
    
    
	LLOPEN(fd);
	

    	
    	
 		
	
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
