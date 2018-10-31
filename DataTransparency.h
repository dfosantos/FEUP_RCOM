#define FlAG 0x7E
#define REPLACE7E 0x7D
#define REPLACE7E2 0x5E
#define FLAG2 0x7D
#define REPLACE7D 0x5D
#include <stdlib.h>
#include "appLayer.h"

int stuffing=0;
char *DataTransparency(char *databuffer,int length, int com_type){
	int j,i=0;
	stuffing = 0;

	char BCC2=0x00;
	
	
	
	int stuffing=0;
	char *shiftbuffer = malloc (sizeof(char));
	char *cpybuffer = malloc(sizeof(char)*length);

	strcpy(cpybuffer,databuffer);
	
	
	if(com_type){
		for (i=0;i<length;i++){
			
			//Cálculo BCC2
			
			BCC2^=cpybuffer[i];
		

			if(cpybuffer[i]==FLAG){
				printf("Encontrou flag\n");
				stuffing++;
				for(j = 0 ; j<=length-i  ; j++){
					
					shiftbuffer = realloc(shiftbuffer, j+1);
					shiftbuffer[j]=cpybuffer[i+j+1];
				}
				length++;
				cpybuffer = realloc(cpybuffer, length);
				cpybuffer[i] = REPLACE7E;
				cpybuffer[i+1] = REPLACE7E2;
			
				
				for(j=0;j<sizeof(shiftbuffer);j++){
					cpybuffer[i+j+2]=shiftbuffer[j];
								
				}			
			}
			else if(cpybuffer[i]==FLAG2){
				stuffing++;
				for(j = 0 ; j<=length-i  ; j++){
					
					shiftbuffer = realloc(shiftbuffer, j+1);
					shiftbuffer[j]=cpybuffer[i+j+1];
				}
				length++;
				cpybuffer = realloc(cpybuffer, length);
				cpybuffer[i+1] = REPLACE7D;
				
				
				for(j=0;j<sizeof(shiftbuffer);j++){
					cpybuffer[i+j+2]=shiftbuffer[j];
								
				}			
			
			}	
		}

		//length++;
		//cpybuffer = realloc(cpybuffer, length);
		//printf("tamanho: %d\n",length);
		
		//cpybuffer[i] = BCC2;
		//printf("caracter na posição %d : %c\n",i,cpybuffer[i]);
		//printf("CPYBUFFER:%s\n",cpybuffer);
		

	}
	else if(!com_type){
		for (i=0;i<length;i++){
				
			
			
			if(cpybuffer[i] == FLAG2 && cpybuffer[i+1] == REPLACE7E2 ){		//Destuffing 7E
				
				
				for(j = 0 ; j<=length-i  ; j++){
					
					shiftbuffer = realloc(shiftbuffer, j+1);
					shiftbuffer[j]=cpybuffer[i+j+2];
				}
				
				cpybuffer[i] = FLAG;
				length--;

				for(j=0;j<sizeof(shiftbuffer);j++){
					cpybuffer[i+j+1]=shiftbuffer[j];		
				}		
				cpybuffer = realloc (cpybuffer,length*sizeof(char));
				
			}	

			else if(cpybuffer[i] == FLAG2 && cpybuffer[i+1] == REPLACE7D){		//Destuffing 7D

				for(j = 0 ; j<=length-i  ; j++){
					
					shiftbuffer = realloc(shiftbuffer, j+1);
					shiftbuffer[j]=cpybuffer[i+j+2];
				}
				
				cpybuffer[i] = FLAG2;
				length--;

				for(j=0;j<sizeof(shiftbuffer);j++){
					cpybuffer[i+j+1]=shiftbuffer[j];		
				}		
				cpybuffer = realloc (cpybuffer,length*sizeof(char));
			

			}
		}

	}
	//printf("cpybuffer: %s\n",cpybuffer);
	
	return cpybuffer;
}
