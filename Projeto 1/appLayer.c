/*Non-Canonical Input Processing*/

#define CHUNK_SIZE 256												//Número de bytes do ficheiro a ser enviado de cada vez
#include "dataLink.h"
#include "utilities.h"

int main(int argc, char** argv) {
    fflush(NULL);
    int fd = open(argv[1], O_RDWR | O_NOCTTY );

    struct termios oldtio,newtio;
    
	
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

    
    fflush(NULL);
	
	
/*-------------------------------------------------------------
---------------------------------------------------------------
---------------------------------------------------------------
----------------Início da Comunicação--------------------------
---------------------------------------------------------------
---------------------------------------------------------------
---------------------------------------------------------------
*/

	
	//Tipo de comunicação: Sender (1) or Receiver (0)
	int com_type = check_arguments(argc, argv);	
  	struct timeval start, stop;
	double secs = 0;
	FILE *file;
	
	printf("\n");
    
    if(com_type) {
			

		if((file=openfile(argv[3], com_type))==NULL)exit(1);
		if(LLOPEN(fd , com_type)==-1) exit(1);										//Estabelecimento da comunicação
		gettimeofday(&start, NULL);													//Inicia relógio
        fflush(NULL);
        
		
		
		char buffer[CHUNK_SIZE];
		char *payload;
		char *stuffed;
		int size;
		short sequence_number = 0;
		char *control = control_frame( argv[3] , file , 1 , &size);					//START frame

		stuffed = stuffing (control, &size);									
	

		if(LLWRITE(fd, stuffed, size) == -1 )										//Send file info
			exit(1);		
		
		printf("A enviar...\n");

		int enviado=0;
		int file_size = getFileSize(file);
		
		while ( (size = fread(buffer, sizeof(char), CHUNK_SIZE, file)) > 0){		//Lê fragmento do ficheiro
				enviado = enviado + size;
				loading(enviado,file_size);											//Barra de progresso
				
				
				payload = header(buffer, &size, sequence_number++);					//Adiciona campo de controlo, número de sequência, tamanho da payload
				stuffed = stuffing(payload, &size);									//Transparência e adiciona BCC2
				
				if(LLWRITE(fd, stuffed, size)==-1)									//Envia o trama
					exit(1);	
				
		}		
		
		control = control_frame( argv[3] , file , 0 , &size);						//END frame
		stuffed = stuffing (control, &size);
		
		if(LLWRITE(fd, stuffed, size) == -1 )										//Send END frame
			exit(1);	
			
		gettimeofday(&stop, NULL);													//Para o relógio
		printf("\n\nFicheiro enviado!\n");
		secs = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
		printf("\nEstatística:\n");
		printf("Tempo de envio = %.2fs\n",secs);
		printf("Débito = %.0f B/s\n\n", file_size/secs);
        
    }
	
	if(!com_type){
		
		if(LLOPEN(fd , com_type)==-1) exit(1);										//Estabelecimento da comunicação
		gettimeofday(&start, NULL);													//Inicia o relógio
		
		char stuffed [CHUNK_SIZE+(int)CHUNK_SIZE/2];								
		char *payload;
		char *buffer;
		int length;
		int received_file_size=0;
		char *filename;
		
		//Leitura da informação do ficheiro
		while(1){
			length = LLREAD(fd, stuffed);	
		
			buffer = verify_bcc2(stuffed, &length);	
			if(buffer == NULL)
					send_REJ(fd);
			else{
				filename = get_info(buffer, &received_file_size);
				if(filename!=NULL){
					printf("File Name: %s\nFile Size: %d\n", filename, received_file_size);
					send_RR(fd);
					file=openfile(filename, com_type);	
					break;
				}
				else
					send_REJ(fd);
			}
		}
		
		//Receção do ficheiro
		printf("A receber...\n");
		while( (length = LLREAD(fd, stuffed) )> 0){
			
			loading((int)getFileSize(file),received_file_size);
			if(stuffed[0]==END){
				
				buffer = verify_bcc2(stuffed, &length);	
				if(buffer == NULL)
					send_REJ(fd);
				else{
					send_RR(fd);
					break;
				}
			}	
			else{
				payload = verify_bcc2(stuffed, &length);								//Faz destuff e verifica o BCC2
				
				if(payload == NULL)
						send_REJ(fd);
				else{
					buffer = remove_header(payload, &length);							//Remove Header 
					send_RR(fd);
					fwrite(buffer,1,length,file);
				}
				
			}
			
		}	
		
		gettimeofday(&stop, NULL);	
		printf("\n\n%ld de %d bytes recebidos\nFicheiro recebido com sucesso!\n\n", getFileSize(file),received_file_size);
		secs = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
		printf("\nEstatística:\n");
		printf("Tempo de envio = %.2fs\n",secs);
		int file_size = getFileSize(file);
		printf("Débito = %.0f B/s\n\n", received_file_size/secs);
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
