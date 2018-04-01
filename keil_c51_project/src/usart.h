
#ifndef __USART_H
#define __USART_H


void uartInit(void);//串口初始化
void uartSendByte(uint8_t _data);	//串口发送一字节数据
void printCom(uint8_t *_data); //打印串口字符串数据


#endif
