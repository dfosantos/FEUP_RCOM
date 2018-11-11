#include "utilities.h"

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

void delay(int milliseconds){
    long pause;
    clock_t now,then;

    pause = milliseconds*(CLOCKS_PER_SEC/1000);
    now = then = clock();
    while( (now-then) < pause )
        now = clock();
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

int check_arguments(int argc, char** argv){
	
	int com_type;
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
	
	return com_type;
}

void loading(int current_size, int total_size){
	
	static short progress=0;
	int i;
	if(progress == 0){
		for(i=0 ; i < 100 ; i++)	printf("\x1B[33m\u2591\x1B[0m");						//IMPRIME BACKGROUND
		for(i=0 ; i < 100 ; i++)	printf("\b");											//VOLTA AO INICIO DA LINHA
	}
	
	
	for(i = 0 ; i < ((int)((float)current_size/(float)total_size*100)) - progress ; i++)
		printf("\x1B[32m\u2593\x1B[0m");													//IMPRIME BARRA
	
	progress=(int)((float)current_size/(float)total_size*100);
}







