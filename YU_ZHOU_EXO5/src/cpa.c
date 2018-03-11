#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
unsigned char s[256] = 
 {
    0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
    0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
    0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
    0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
    0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
    0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
    0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
    0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
    0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
    0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
    0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
    0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
    0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
    0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
    0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
    0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
 };
unsigned char sbox_xor(unsigned char pt,unsigned char keyguess){
	return s[pt^keyguess];
}
//count how many 1 presented in binary for a number
unsigned int numbit1(int n){
	int nb=0;
	while(n>0){
		if(n&1==1){
			nb++;}
		n=n>>1;}
	return nb;
}
//build hamming weight
unsigned char HW(unsigned char w){
	return numbit1(w);
	}
float correlationCoefficient (unsigned char ***hyp , float **t_traces, float hmean,float *tmean ,int bnum,int k,int size_trace,int size_line){
			int tnum,j;
			float *sum,*sum1,*sum2,*tdiff,hdiff,inter,cpaoutput,maxcpa;
			//tdiff
			tdiff=(float *) malloc(size_trace*sizeof(float));
			//sum
			sum=(float *) malloc(size_trace*sizeof(float));
			//sum1
			sum1=(float *) malloc(size_trace*sizeof(float));
			//sum2
			sum2=(float *) malloc(size_trace*sizeof(float));
			// intial sum,sum1,sum2 by 0
			for(j=0;j<size_trace;j++){
					sum[j]=0;
					sum1[j]=0;
					sum2[j]=0;
				}
			//k is keyguess
			for(tnum=0;tnum<size_line;tnum++){
				hdiff=hyp[tnum][bnum][k]-hmean;
				for(j=0;j<size_trace;j++){
					tdiff[j]=t_traces[tnum][j]-tmean[j];
					sum[j]+=(float)hdiff*tdiff[j];
					sum1[j]+=(float)hdiff*hdiff;
					sum2[j]+=(float)tdiff[j]*tdiff[j];
					}
			}
			cpaoutput=-100;
			for(j=0;j<size_trace;j++){
				// (EXY - EX*EY)/(sqrt(EXX - EX*EX)/sqrt(EYY - EY*EY))
				inter=(float)fabs(sum[j])/sqrt((float)sum1[j]*sum2[j]);
				if(cpaoutput<inter)
					cpaoutput=inter;}
			maxcpa=cpaoutput;
			free(sum);
			free(sum1);
			free(sum2);
			free(tdiff);	
			return maxcpa;
		}



int main(){	
	//encryption key
	int bestguess[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	FILE *f=fopen("aes_traces_cp.csv","r");
		if(f==NULL){
			printf("Can't open this file \n");
			exit(EXIT_FAILURE);
			}
		else{
		FILE *fp=fopen("scores.csv","w+");
		int size_plain=16;
		int size_trace=2200;
		int size_line=500;
		int i,j,k,line,bnum,tnum;
		unsigned char c,buffer[32], *hw,*keyguess,***hyp, **t_plaintexts;
		float **t_traces,*tmean,hmean,cpaoutput,*maxcpa;

		t_plaintexts=(unsigned char **) malloc(size_line*sizeof(unsigned char *)); 
		t_traces=(float **) malloc(size_line*sizeof(float*));   
		for(i=0;i<size_line;i++){
			t_plaintexts[i]=(unsigned char *) malloc(size_plain*sizeof(unsigned char ));
			t_traces[i]=(float *) malloc(size_trace*sizeof(float ));
			}

		//line number
		for(line=0;line<size_line;line++){
			j=0,i=0;
			//get one line of plaintexts
			while(1){
				c=fgetc(f);
				//remove useless information(noisy)
				if(c<0) continue;
				if(c ==','|| c== '\n'){
				t_plaintexts[line][j]=atoi(buffer);
					if(c=='\n') break;
				j++;
				bzero(buffer,32);
				i=0;
				}
				else{				
					buffer[i]=c;
				i++;}
				}
			j=0,i=0;
			//get one line of the traces
			while(1){
				c=fgetc(f);
				//remove useless information(noisy)
				if(c<0) continue;
				if(c ==','|| c== '\n'){
				t_traces[line][j]=atof(buffer);
					if(c=='\n') break;
				j++;
				bzero(buffer,32);
				i=0;
				}
				else{	
					buffer[i]=c;
				i++;}
				}
			
		
		}
	
	hw=(unsigned char *) malloc(256*sizeof(unsigned char));
	maxcpa=(float*) malloc(256*sizeof(float));
	keyguess=(unsigned char *) malloc(256*sizeof(unsigned char));
	
	//tmean
	tmean=(float *) malloc(size_trace*sizeof(float));
	for(i=0;i<size_trace;i++){tmean[i]=0;}

	//stocker hamming weight 1000*16*256
	hyp=(unsigned char ***) malloc(size_line*sizeof(unsigned char **));
	for(i=0;i<size_line;i++){
		hyp[i]=(unsigned char **) malloc(size_plain*sizeof(unsigned char*));
		for(j=0;j<size_plain;j++){
			hyp[i][j]=(unsigned char *) malloc(256*sizeof(unsigned char));
			}
		}	
	
	fprintf(fp,"key's position \t keyguess \t maxabscpa\n");
	for(bnum=0;bnum<size_plain;bnum++){
		for(k=0;k<256;k++){
			hmean=0;
			cpaoutput=0;
			maxcpa[k]=0;
			keyguess[k]=k;
			//hw,calcule haming model,hamming weight
			for(tnum=0;tnum<size_line;tnum++){
				hyp[tnum][bnum][k]=HW(sbox_xor(t_plaintexts[tnum][bnum],keyguess[k]));
				hmean+=hyp[tnum][bnum][k];
				//printf("tnum %d  bnum %d k %d hyp %d keyguess %d plain %d \n",tnum,bnum,k,hyp[tnum][bnum][k],keyguess[k],t_plaintexts[tnum][bnum]);
			}
			//calcule hmean
			hmean=(float)hmean/size_line;
			//calcule tmean-------------------------------------
			for(j=0;j<size_trace;j++){
				for(i=0;i<size_line;i++){
					tmean[j]+=t_traces[i][j];
						}
					}
			for(j=0;j<size_trace;j++){
				tmean[j]=(float)tmean[j]/size_line;}
			
			//get max abs j (sample'number)
			maxcpa[k]=correlationCoefficient (hyp ,t_traces, hmean,tmean ,bnum,k,size_trace,size_line);	
			fprintf(fp,"%d \t %d \t %f \n",bnum,k,maxcpa[k]);
			}

		cpaoutput=-100;
		//get max i(size_line:traces' number)
		for(i=0;i<256;i++){
			if(cpaoutput < maxcpa[i]){
				cpaoutput=maxcpa[i];
				bestguess[bnum]=i;
				}
			}
			}
		//free espace
		free(hw);
		free(keyguess);
		free(tmean);
		free(maxcpa);
		for(i=0;i<size_line;i++){
			free(t_plaintexts[i]);
			free(t_traces[i]);}
		free(t_plaintexts);
		free(t_traces);
		for(i=0;i<size_line;i++){
			for(j=0;j<size_plain;j++){
				free(hyp[i][j]);
				}
			}
		for(i=0;i<size_line;i++){
				free(hyp[i]);}
		free(hyp);
		
		fclose(fp);
		}

	fclose(f);
	int i,nb=0;
	
	unsigned int answer[]={0x00, 0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb,0xcc,0xdd,0xee ,0xff};	
	for(i=0;i<16;i++){
		if(answer[i]==bestguess[i])
			nb++;
		printf("%x ",bestguess[i]);}
		printf("\n");	
		printf("proba %f \n",(float)nb/16);
	return 0;
}
