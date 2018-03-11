import csv
import matplotlib.pyplot as plt
import numpy as np


_sbox=(
0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16)
def sbox(inp):
    return _sbox[inp]

def subbytes(inp):
    return sbox(inp)

def sbox_xor(inp,key):
     return subbytes(inp ^ key)

def DPA():
	#size of a oct
	b256=256
	#key's length
	lenkey=16
	#message input
	plain_text=list()
	#traces captured and wrote  by  oscilloscope
	trace=list()
	#sub=[[[]*256]*16]
	sub_0=[[[] for _ in range(b256)]for _ in range(lenkey)]
	sub_1=[[[] for _ in range(b256)]for _ in range(lenkey)]
	#open file (try finally)
	with open('aes_traces_dpa.csv') as f:
		read_data=f.read().strip('\r')
	#del '\n'
	data=read_data.split('\n')
	#get number of trace 
	#nbtrace=np.shape(data)[0]/2
	nbtrace=5500
	#print nbtrace
	for i in range(nbtrace):
		#build lists of plain_text,turn string to int
		plain_text.append(map(int ,data[2*i].strip().split(',')))
		#build list of trace ,turn string to float
		trace.append(map(float, data[2*i+1].strip().split(',')))

	
	
	#calcule lenkey(16) guess
	for k in range(lenkey):
		#try keyguess from 0 to 255 
		for keyguess in range(b256):
			#seperate trace ,depend on 1 or 0	
			for i in range(nbtrace):
				#1
				if(sbox_xor(plain_text[i][k],keyguess)>>7):
					sub_1[k][keyguess].append(trace[i])
				#0
				else:
					sub_0[k][keyguess].append(trace[i])
	
	#initial bestguess for cipher key
	bestguess=[0]*lenkey
	for k in range(lenkey):
		b=-100
		for keyguess in range(b256):
			#the abs subtract of sub0's average and sub1's average
			sub_mean=map(abs,np.subtract(np.average(sub_0[k][keyguess],0),np.average(sub_1[k][keyguess],0)))		
			#get values for graph
			#plt.plot(sub_mean)
			#calcule max
			a=max(sub_mean)
			if(b<a):
				b=a
				bestguess[k]=keyguess
	
	nbcorrect=0
	answer=[0x00, 0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb,0xcc,0xdd,0xee ,0xff]	
	for i in range(16):
		if(bestguess[i]==answer[i]):
			nbcorrect=nbcorrect+1
		bestguess[i]=hex(bestguess[i])
	print 1.0*nbcorrect/16
	print bestguess
	#show graph
	#plt.axis((0, 2200, 0, 0.001))
	#plt.grid()
	#plt.show()
	
	f.closed

DPA(),    
