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

int LLWRITE(int fd, char *buffer, int length);

int LLOPEN(int fd); //envia trama SET e recebe trama UA.
int LLREAD(int fd, char *buffer);
int LLCOSE(int fd);

void send_SET(int fd);
void send_UA(int fd);
void send_DISC(int fd);


//-------------------------------------------Application Link Layer --------------------------

int main(int argc, char** argv);
