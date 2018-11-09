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