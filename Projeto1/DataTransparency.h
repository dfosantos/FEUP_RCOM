#define FlAG 0x7E
#define replace7E 0x7D
#define replace7E2 0x5E
#define FLAG2 0x7D
#define replace7D 0x5D
#include <stdlib.h>

char *DataTransparency(char *databuffer,int length, int com_type){
	int i=0;
	char c;
	char aux, aux2;
	char BCC2=0x00;
	char *newbuffer = malloc((length)*sizeof(char));
	if(com_type){
		for (i=0;i<length;i++){
			c=databuffer[i];
			BCC2^⁼c;
			newbuffer[i]=c;
			if(c==FlAG){
				length++;
				char *newbuffer = realloc(newbuffer, length);
				newbuffer[i]=replace7E;
				newbuffer[i+1]=replace7E2;
				i++;
			}
			else if(c==FLAG2){
				length++;
				char *newbuffer = realloc(newbuffer, length);
				newbuffer[i+1]=replace7D;
				i++;
			}
		}
	}
	else{
		for (i=0;i<length;i++){
			c=databuffer[i];
			aux=databuffer[i+1];
			aux2=databuffer[i+2];
			newbuffer[i]=c;
			newbuffer[i+1]=aux;
			if(databuffer[i]==FLAG2&&aux==replace7D){
				newbuffer[i+1]=aux2;
				length--;
				char *newbuffer = realloc(newbuffer, length);
			}
			else if (databuffer[i]==replace7E&&aux==replace7E2){
				newbuffer[i]=FlAG;
				newbuffer[i+1]=aux2;
				length--;
				char *newbuffer = realloc(newbuffer, length);
			}
			BCC2^=newbuffer[i];
		}			
	}
return newbuffer;
}
