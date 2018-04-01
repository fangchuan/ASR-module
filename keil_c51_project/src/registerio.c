
#include "config.h"

#define LD_INDEX_PORT		(*((volatile unsigned char xdata*)(0x8100))) 
#define LD_DATA_PORT		(*((volatile unsigned char xdata*)(0x8000))) 


//评估板上 MCU的P2.0 连接到 LD芯片的A0
//         MCU的P2.1 连接到 LD芯片的CSB
//         MCU的RD、WR 连接 LD芯片的RD、WR (xdata 读写时自动产生低信号)
//
//0x8100的二进制是10000001 00000000		CSB=0 A0=1
//                 ^     ^
//0x8000的二进制是10000000 00000000		CSB=0 A0=0
//                 ^     ^		
void LD_WriteReg( unsigned char address, unsigned char dataout )
{
	LD_INDEX_PORT  = address;         
	LD_DATA_PORT = dataout;          
}

unsigned char LD_ReadReg( unsigned char address )
{
	LD_INDEX_PORT = address;         
	return (unsigned char)LD_DATA_PORT;     
}

