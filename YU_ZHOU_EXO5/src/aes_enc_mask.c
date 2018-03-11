/* aes_enc.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008  Daniel Otte (daniel.otte@rub.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * \file     aes_enc.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2008-12-30
 * \license  GPLv3 or later
 * 
 */

#include <stdint.h>
#include <string.h>
#include "aes.h"
#include "gf256mul.h"
#include "aes_sbox.h"
#include "aes_enc.h"
#include <avr/pgmspace.h>
/*bibliothèque pour créer un offset aléatoire */
#include <stdlib.h>
#include <time.h>

/*définir des masques*/
uint8_t mask[16]={0x03,0x0c,0x35,0x3a,0x50,0x5f,0x66,0x69,
		  0x96,0x99,0xa0,0xaf,0xc5,0xca,0xf3,0xfc};

uint8_t swap(uint8_t *T,uint8_t i,uint8_t j){
	uint8_t tmp;
	tmp=T[i];
	T[i]=T[j];
	T[j]=tmp;
	}

void aes_shiftcol(void* data, uint8_t shift){
	uint8_t tmp[4];
	tmp[0] = ((uint8_t*)data)[ 0];
	tmp[1] = ((uint8_t*)data)[ 4];
	tmp[2] = ((uint8_t*)data)[ 8];
	tmp[3] = ((uint8_t*)data)[12];
	((uint8_t*)data)[ 0] = tmp[(shift+0)&3];
	((uint8_t*)data)[ 4] = tmp[(shift+1)&3];
	((uint8_t*)data)[ 8] = tmp[(shift+2)&3];
	((uint8_t*)data)[12] = tmp[(shift+3)&3];
}

#define GF256MUL_1(a) (a)
#define GF256MUL_2(a) (gf256mul(2, (a), 0x1b))
#define GF256MUL_3(a) (gf256mul(3, (a), 0x1b))

static
void aes_enc_round(aes_cipher_state_t* state, const aes_roundkey_t* k,uint8_t *shuffle0,uint8_t mask_offset[16][16],uint8_t r){
	uint8_t tmp[16], t;
	uint8_t mask_tmp[16],mask_compensation[16];
	uint8_t i;

	/* masksubBytes */
	/*shuffle0 pour le premier SubByte*/
	if(r==1){
		for(i=0; i<16; ++i){
			/*shuffle0 pour 1è tour */
			/*masque subBytes MSB(x)=SB(x xor m_i) xor m_(i+1) */
			tmp[shuffle0[i]] = pgm_read_byte(aes_sbox+(state->s[shuffle0[i]]^mask_offset[r-1][shuffle0[i]]))^mask_offset[r][shuffle0[i]];

			}
		}
	else{
		for(i=0; i<16; ++i){
			/*masque subBytes MSB(x)=SB(x xor m_i) xor m_(i+1) */
			tmp[i] = pgm_read_byte(aes_sbox+(state->s[i]^mask_offset[r-1][i]))^mask_offset[r][i];

			}
	}

	/* shiftRows */
	aes_shiftcol(tmp+1, 1);
	aes_shiftcol(tmp+2, 2);
	aes_shiftcol(tmp+3, 3);

	/* mixColums */
	for(i=0; i<4; ++i){
		t = tmp[4*i+0] ^ tmp[4*i+1] ^ tmp[4*i+2] ^ tmp[4*i+3];
		state->s[4*i+0] =
			  GF256MUL_2(tmp[4*i+0]^tmp[4*i+1])
			^ tmp[4*i+0]
			^ t;
		state->s[4*i+1] =
			  GF256MUL_2(tmp[4*i+1]^tmp[4*i+2])
			^ tmp[4*i+1]
			^ t;
		state->s[4*i+2] =
			  GF256MUL_2(tmp[4*i+2]^tmp[4*i+3])
			^ tmp[4*i+2]
			^ t;
		state->s[4*i+3] =
			  GF256MUL_2(tmp[4*i+3]^tmp[4*i+0])
			^ tmp[4*i+3]
			^ t;
	}
	/*mask temporaire*/
	for(i=0; i<16; ++i){
		mask_tmp[i]=mask_offset[r][i];
		}
	/*Construction la Compensation des masques*/
	/* shiftRows */
	aes_shiftcol(mask_tmp+1, 1);
	aes_shiftcol(mask_tmp+2, 2);
	aes_shiftcol(mask_tmp+3, 3);

	/* mixColums */
	for(i=0; i<4; ++i){
		t = mask_tmp[4*i+0] ^ mask_tmp[4*i+1] ^ mask_tmp[4*i+2] ^ mask_tmp[4*i+3];
		mask_compensation[4*i+0] =
			  GF256MUL_2(mask_tmp[4*i+0]^mask_tmp[4*i+1])
			^ mask_tmp[4*i+0]
			^ t;
		mask_compensation[4*i+1] =
			  GF256MUL_2(mask_tmp[4*i+1]^mask_tmp[4*i+2])
			^ mask_tmp[4*i+1]
			^ t;
		mask_compensation[4*i+2] =
			  GF256MUL_2(mask_tmp[4*i+2]^mask_tmp[4*i+3])
			^ mask_tmp[4*i+2]
			^ t;
		mask_compensation[4*i+3] =
			  GF256MUL_2(mask_tmp[4*i+3]^mask_tmp[4*i+0])
			^ mask_tmp[4*i+3]
			^ t;
	}
	/* xor compensation ,xor mask_offset[r]*/
	for(i=0;i<16;i++){
		state->s[i]^=mask_compensation[i];
		state->s[i]^=mask_offset[r][i];
	}


	/* addKey */
	for(i=0; i<16; ++i){
		state->s[i] ^= k->ks[i];
	}

}

static
void aes_enc_lastround(aes_cipher_state_t* state,const aes_roundkey_t* k,uint8_t *shuffle10,uint8_t mask_offset[16][16],uint8_t r){
	uint8_t i;
	uint8_t mask_compensation[16];
	/* subBytes */
	for(i=0; i<16; ++i){
		state->s[shuffle10[i]] = pgm_read_byte(aes_sbox+(state->s[shuffle10[i]]^mask_offset[r-1][shuffle10[i]]))^mask_offset[r][shuffle10[i]];
	}
	/* shiftRows */
	aes_shiftcol(state->s+1, 1);
	aes_shiftcol(state->s+2, 2);
	aes_shiftcol(state->s+3, 3);

	/*Construction la Compensation des masques*/
	/*mask temporaire*/
	for(i=0; i<16; ++i){
		mask_compensation[i]=mask_offset[r][i];
		}
	/*Construction la Compensation des masques*/
	/* shiftRows */
	aes_shiftcol(mask_compensation+1, 1);
	aes_shiftcol(mask_compensation+2, 2);
	aes_shiftcol(mask_compensation+3, 3);
		
	/*démasquer: xor compensation ,xor mask_offset[r]*/
	for(i=0;i<16;i++){
		state->s[i]^=mask_compensation[i];
	}

	/* keyAdd */
	for(i=0; i<16; ++i){
		state->s[i] ^= k->ks[i];
	}
}

void aes_encrypt_core(aes_cipher_state_t* state, const aes_genctx_t* ks, uint8_t rounds){
	uint8_t i;
	/*Déclarer masque avec offset*/
	uint8_t mask_offset[16][16];
	/*créer un tableau aléatoire entier [0,15]*/
	uint8_t offset[16];
	/*le premier tour,ne protège que SUbBytes*/
	uint8_t shuffle0[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
	/*le dernier tour,ne protège que SUbBytes*/
	uint8_t shuffle10[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
	srand(time(NULL));
	for(i=0;i<16;i++){
		/*Permuter shuffle0 */
		swap(shuffle0,rand()%16,rand()%16);
		/*Permuter shuffle10 */
		swap(shuffle10,rand()%16,rand()%16);
		/*Initialiser offset */
		offset[i] = rand()%16;
		/*remplir mask_offset */
		mask_offset[i][0]=mask[offset[i]];
		mask_offset[i][1]=mask[(offset[i]+1)%16];
		mask_offset[i][2]=mask[(offset[i]+2)%16];
		mask_offset[i][3]=mask[(offset[i]+3)%16];
		mask_offset[i][4]=mask[(offset[i]+4)%16];
		mask_offset[i][5]=mask[(offset[i]+5)%16];
		mask_offset[i][6]=mask[(offset[i]+6)%16];
		mask_offset[i][7]=mask[(offset[i]+7)%16];
		mask_offset[i][8]=mask[(offset[i]+8)%16];
		mask_offset[i][9]=mask[(offset[i]+9)%16];
		mask_offset[i][10]=mask[(offset[i]+10)%16];
		mask_offset[i][11]=mask[(offset[i]+11)%16];
		mask_offset[i][12]=mask[(offset[i]+12)%16];
		mask_offset[i][13]=mask[(offset[i]+13)%16];
		mask_offset[i][14]=mask[(offset[i]+14)%16];
		mask_offset[i][15]=mask[(offset[i]+15)%16];
		}


	for(i=0; i<16; ++i){
		/*ajoute des masks*/
		state->s[i]^=mask_offset[0][i];
		state->s[i]^= ks->key[0].ks[i];
	}
	i=1;
	for(;rounds>1;--rounds){
		aes_enc_round(state, &(ks->key[i]),shuffle0,mask_offset,i);
		++i;
	}
	aes_enc_lastround(state, &(ks->key[i]),shuffle10,mask_offset,i);
}
