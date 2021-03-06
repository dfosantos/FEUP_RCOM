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
#include <sys/time.h>
#include "utilities.h"

//--------------------------------------------------------------------------------------------------------------------------------------
#define BCC_ERROR_PROBABILITY 	0	//Probabilidade de erro na leitura de BCC ----------Valores entre 0-50	------------------------
#define BCC2_ERROR_PROBABILITY 	0		//Probabilidade de erro na leitura de BCC2	--------Valores entre 0-50 -------------------------
#define TRANSMISSION_DELAY 		0		//Tempo simulado de propagação dos pacotes (ms)	------------------------------------------------
#define TIMEOUTS				3		//Número de timeouts de 3s até encerrar a ligação
//--------------------------------------------------------------------------------------------------------------------------------------

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
#define START 0x01
#define END 0x02

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

int LLWRITE(int fd, char *buffer, int length);									//Envia o trama. Retorna nr de chars escritos. -1 em caso de erro

int LLOPEN(int fd, int com_type); 												//Estabelece a comunicação. Retorna 1 ou -1(erro)

int LLREAD(int fd, char *buffer);												//Retorna número de chars de Data lidos

int LLCLOSE(int fd, int com_type);												//Encerra a comunicação. Retorna 1 ou -1 (erro)

//Funções de stuffing, destuffing e a função que verifica o BCC2
char* stuffing(char* payload, int* length);										//Faz stuffing e adiciona BCC2
char* destuffing(char* msg, int* length);										//Faz destuffing e retira BCC2
char* verify_bcc2(char* control_message, int* length);							//Verifica BCC2
char* control_frame(char* filename, FILE *file, int start, int* frame_size);	//Cria pacote de controlo START/END
char* get_info(char* control, int* file_size);									//Extrai nome de ficheiro e respetivo tamanho em bytes
char* header(char* buffer, int* length, short sequence_number);					//Adiciona byte de controlo, número de sequência e tamanho do campo de dados
char* remove_header(char* buffer, int* length);									//Remove Header

FILE *openfile(char* filename, int com_type);
void send_SET(int fd);
void send_UA(int fd, int com_type);
void send_DISC(int fd, int com_type);
void send_REJ(int fd);
void send_RR(int fd);
