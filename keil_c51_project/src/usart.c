
#include "config.h"


#define   FOSC     22118400L         //System frequency

uint32_t  baudRate = 9600;           //UART baudrate
uint8_t   rxBuffer[20];
uint8_t   rxCount = 0;
uint8_t   rxFlag =  0;
/************************************************************************
函 数 名： 串口初始化
功能描述： STC10L08XE 单片机串口初始化函数
返回函数： none
其他说明： none
**************************************************************************/
void uartInit(void)
{
    SCON = 0x50;            //8-bit variable UART
    TMOD = 0x20;            //Set Timer1 as 8-bit auto reload mode
    TH1 = TL1 = -(FOSC/12/32/baudRate); //Set auto-reload vaule
    TR1 = 1;                //Timer1 start run
    ES = 1;                 //Enable UART interrupt
    EA = 1;                 //Open master interrupt switch
}
/***********************************************************
* 名    称： 
* 功    能： 
* 入口参数： 无 
* 出口参数：无
* 说    明： 					 
**********************************************************/
void UartIsr() interrupt 4 using 1
{	
	if(RI)
	{	
		RI = 0;             //Clear receive interrupt flag
        rxBuffer[rxCount++] = SBUF;          //P0 show UART data
		if(rxCount == 20)	  
			rxCount = 0;
	}
	if(rxBuffer[rxCount-2]== '\r' && rxBuffer[rxCount-1] == '\n')
	 {
	 	rxBuffer[rxCount-2] = 0;
		rxBuffer[rxCount-1]=0;
		rxCount= 0 ;
		rxFlag=1;
	 } 


}
/************************************************************************
功能描述： 	串口发送一字节数据
入口参数：	DAT:带发送的数据
返 回 值： 	none
其他说明：	none
**************************************************************************/
void uartSendByte(uint8_t _data)
{
	ES  =  0;
	TI=0;
	SBUF = _data;
	while(TI==0);
	TI=0;
	ES = 1;
}
/************************************************************************
功能描述： 串口发送字符串数据
入口参数： 	*_data：字符串指针
返 回 值： none
**************************************************************************/
void printCom(uint8_t *_data)
{
	while(*_data)
	{
	 	uartSendByte(*_data++);
	}	
}

