#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


#define TRANSMITTER 1
#define RECEIVER 0


#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define OPEN 0
#define READ 1
#define WRITE 2

#define FLAG 0x7E
#define C_T 0x03
#define C_R 0x07
#define A_T 0x03
#define A_R 0x01
#define BCC_T 0x00
#define BCC_R 0x04

#define UA 0x07
#define SET 0x03
#define DISC 0x0A
#define START 2
#define END 3

#define RR0 0x05
#define RR1 0x25
#define REJ0 0x01
#define REJ1 0x21


#define TRAMA_S 0
#define TRAMA_I 1

#define ERROR_BCC1 1
#define ERROR_BCC2 2

void time_out();



//-------------------------------------------------Data Link Layer --------------------------
//Envia o trama. Retorna nr de chars escritos. -1 em caso de erro
int LLWRITE(int fd, char *buffer, int length);	



//Estabelece a comunicação.Retorna 1 ou -1(erro)
int LLOPEN(int fd, int com_type); //Estabelece a comunicação

//Retorna número de chars de Data lidos
int LLREAD(int fd, char *buffer);

//Funções de stuffing, destuffing e a função que verifica o BCC2
unsigned char* stuffing(unsigned char* msg, int* length);
unsigned char* destuffing(unsigned char* msg, int* length);
unsigned char* verify_bcc2(unsigned char* control_message, int* length);

//Encerra a comunicação. Retorna 1 ou -1 (erro)
int LLCLOSE(int fd, int com_type);


FILE *openfile(char* filename, int com_type);
void send_SET(int fd);
void send_UA(int fd, int com_type);
void send_DISC(int fd, int com_type);
void send_REJ(int fd);
void send_RR(int fd);

long getFileSize(FILE* file);


//-------------------------Main APP--------------------------

int main(int argc, char** argv);
