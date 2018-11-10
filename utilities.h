#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


FILE *openfile(char* filename, int com_type);			//Lê ou cria um ficheiro 
void delay(int milliseconds);							//self-explanatory
long getFileSize(FILE* file);							//Retorna o tamanho do ficheiro
int check_arguments(int argc, char** argv);				//Verifica se os argumentos estão certos e imprime informação em caso contrário