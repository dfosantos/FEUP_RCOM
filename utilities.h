#include <time.h>
#include <stdio.h>

FILE *openfile(char* filename, int com_type);
void delay(int milliseconds);
long getFileSize(FILE* file);