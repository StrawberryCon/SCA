#//include "hal.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
uint16_t addition_modulo(uint16_t a, uint16_t b)
{
	return (a+b)%65536;
}

uint16_t multiple_modulo(uint16_t a, uint16_t b)
{
	
	
	if(!a && !b) return 1;
	else if (!a) return (((uint32_t)(a*65536))%65537)%65536;
             else if(!b) return (((uint32_t)(65536*b))%65537)%65536;
                   else return (((uint32_t)(a*b))%65537)%65536;
}


uint16_t xor(uint16_t a, uint16_t b)
{
	return a^b;
}
static 
void generatekey(uint16_t *key){
	uint16_t *tmp=(uint16_t *) malloc(8*sizeof(uint16_t));
	int i,pos=25,r=0;
	for(i=0;i<8;i++){
		tmp[i]=key[i];
	}
	/*décaler 25 bits de positions*/
	for(i=0;i<8;i++){
		key[i]=tmp[(pos/16)%8]<<(pos%16);
		pos=pos+16;
		key[i]+=tmp[(pos/16)%8]>>(16-pos%16);
	}

	
}

void chiffre_tour(uint16_t* plaintext, uint16_t* key)
{	int i;
	//uint16_t *c = (uint16_t *)malloc(4*sizeof(uint16_t));
	uint16_t c[4]={0,0,0,0};
	uint16_t tmp[2] = {0,0};

	key[0] = multiple_modulo(plaintext[0], key[0]);
	key[1] = addition_modulo(plaintext[1], key[1]);
	key[2] = addition_modulo(plaintext[2], key[2]);
	key[3] = multiple_modulo(plaintext[3], key[3]);

	tmp[0] = xor(key[0], key[2]);
	tmp[1] = xor(key[1], key[3]);
	tmp[0] = multiple_modulo(tmp[0], key[4]);
	tmp[1] = addition_modulo(tmp[0], tmp[1]);
	tmp[1] = multiple_modulo(tmp[1], key[5]);
	tmp[0] = addition_modulo(tmp[0], tmp[1]);
	
	c[0] = xor(key[0], tmp[1]);
	c[1] = xor(key[2], tmp[1]);
	c[2] = xor(key[1], tmp[0]);
	c[3] = xor(key[3], tmp[0]);
	
	for(i=0;i<4;i++){
		plaintext[i]=c[i];}
	
}

static
void lastround(uint16_t* plaintext, uint16_t* key){
	int i;
	uint16_t *c = (uint16_t *)malloc(4*sizeof(uint16_t));
	c[0]=multiple_modulo(plaintext[0],key[0]);
	c[1]=addition_modulo(plaintext[2],key[1]);
	c[2]=addition_modulo(plaintext[1],key[2]);
	c[3]=multiple_modulo(plaintext[3],key[3]);
	for(i=0;i<4;i++){
		plaintext[i]=c[i];}
	}
int main(void){
	int i,j,tour;
	srand(time(NULL));
	uint16_t b16=pow(2,16)-1;
	uint16_t plaintext[4]={0x0532,0x0a64,0x14c8,0x19fa};
	//uint16_t plaintext[4]={rand()%(b16+1),rand()%(b16+1),rand()%(b16+1),rand()%(b16+1)};
	//uint16_t key[16]={rand()%(b16+1),rand()%(b16+1),rand()%(b16+1),rand()%(b16+1),
	//		  rand()%(b16+1),rand()%(b16+1),rand()%(b16+1),rand()%(b16+1)};
	uint16_t key[16]={0x0064,0x00c8,0x012c,0x0190,0x01f4,0x0258,0x02bc,0x0320};
	uint16_t *subkey=(uint16_t *) malloc(6*sizeof(uint16_t));
	uint16_t *lastsubkey=(uint16_t *) malloc(4*sizeof(uint16_t));
	uint16_t *subkey_all=(uint16_t *) malloc(52*sizeof(uint16_t));
	/*printf("plaintext\n");
	for(j=0;j<4;j++){
		printf("%x ",plaintext[j]);}
			printf("\n");*/
	/*calculer touts les sous clés*/
	for(i=0;i<7;i++){
		for(j=0;j<8;j++){
			subkey_all[j+i*8]=key[j];
			}
		generatekey(key);
	}
	printf("toutes les clés\n");
	for(j=0;j<52;j++){
		printf("%x ",subkey_all[j]);
			if(j%6==5)
			printf("\n");
			}
	printf("\n");
	/*calculer touts les sous clés*/
	printf("toutes les étapes de chiffrement\n");
	for(tour=0;tour<9;tour++){
		if(tour==8){
			//printf("clé \n");
			for(j=0;j<4;j++){
				printf("%x ",plaintext[j]);}
			printf("\n");
			for(j=0;j<4;j++){
				lastsubkey[j]=subkey_all[tour*6+j];
			//	printf("%x ",lastsubkey[j]);
				}
			//		printf("\n");
				lastround(plaintext, lastsubkey);

				break;
		}
		else{	
			for(j=0;j<4;j++){
				printf("%x ",plaintext[j]);}
			printf("\n");
			//printf("clé \n");
			for(j=0;j<6;j++){
				subkey[j]=subkey_all[tour*6+j];
			//	printf("%x ",subkey[j]);
					}
			//	printf("\n");
			chiffre_tour(plaintext,subkey);
			//printf("plaintext \n");
			
		}
	}
	printf("Sortie finale \n");
	for(j=0;j<4;j++){
			printf("%x ",plaintext[j]);}
			printf("\n");
	
	
	return 0;
	}
	
	
