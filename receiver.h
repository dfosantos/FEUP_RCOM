#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>


#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

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

#define C_I 0x01

#define TRAMA_S 0
#define TRAMA_I 1

#define ERROR_BCC1 1
#define ERROR_BCC2 2


//------------------------------------------------Data Link Layer--------------------------------

int LLOPEN(int fd); //lê trama SET e envia trama UA.

int LLREAD(int fd, char *buffer);

void send_UA(int fd);

int LLCOSE(int fd);

//-----------------------------------------------Application Link Layer-------------------------



int main(int argc, char** argv);
