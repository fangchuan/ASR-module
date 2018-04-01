
#include "config.h"

extern void  delay(unsigned long uldata);
extern uint8_t idata nAsrStatus;
uint8_t idata ucRegVal;


void ProcessInt0(void);


volatile uint8_t  sRecog[VOCIE_MAX_NUMBER][VOCIE_LENGTH] = {
									"a bo luo",\		
									"ji qi ren",\
									 "qian jin",\
									 "hou tui",\
									 "zuo zhuan",\
									 "you zhuan",\
									 "ting zhi",\
									 "jia su", \
									 "jian su", \
									 "kai deng",\
									 "guan deng", \
									 "he jiu", \
									 "he he da", \
									 "dian nao", \
									 "zhu ban", \
									 "nei cun tiao", \
									 "ying pan", \
									 "kai pao", \
									 "da fei ji", \
									 "he jiu", \
									 "zhong xing", \
									 "wei wei wei", \
									 "ni hao"
								};
uint8_t  g_indexsBuffer[VOCIE_MAX_NUMBER] = {
							INDEX_APOLLO,\
							INDEX_ROBOT,\
							INDEX_FORWARD,\
							INDEX_BACKWARD,\
							INDEX_LEFT,\
							INDEX_RIGHT,\
							INDEX_STOP, \
							INDEX_SPEED_UP,\
							INDEX_SPEED_DOWN, \
							INDEX_TURN_ON, \
							INDEX_TURN_OFF, \
							INDEX_11, \
							INDEX_12, \
							INDEX_13, \
							INDEX_14, \
							INDEX_15, \
							INDEX_16, \
							INDEX_17, \
							INDEX_18, \
							INDEX_19, \
							INDEX_20, \
							INDEX_21,\
							INDEX_22, \
							INDEX_23, \
							INDEX_24, \
							INDEX_25, \
							INDEX_26, \
							INDEX_27, \
							INDEX_28, \
							INDEX_29, \
							INDEX_30, \
							INDEX_31, \
							INDEX_32, \
							INDEX_33, \
							INDEX_34
						 };	

/************************************************************************
功能描述： 	 复位LD模块
入口参数：	 none
返 回 值： 	 none
其他说明：	 none
**************************************************************************/
void LD3320Reset()
{
	RSTB=1;
	delay(5);
	RSTB=0;
	delay(5);
	RSTB=1;

	delay(5);
	CSB=0;
	delay(5);
	CSB=1;
	delay(5);
}
/************************************************************************
功能描述： LD模块命令初始化
入口参数： none
返 回 值： none
其他说明： 该函数为出厂配置，一般不需要修改；
					 有兴趣的客户可对照开发手册根据需要自行修改。
**************************************************************************/
void LD3320CommonInit()
{
	LD_ReadReg(0x06);  
	LD_WriteReg(0x17, 0x35); 
	delay(10);
	LD_ReadReg(0x06);  

	LD_WriteReg(0x89, 0x03);  
	delay(5);
	LD_WriteReg(0xCF, 0x43);   
	delay(5);
	LD_WriteReg(0xCB, 0x02);
	
	/*PLL setting*/
	LD_WriteReg(0x11, LD_PLL_11);       

	LD_WriteReg(0x1E,0x00);
	LD_WriteReg(0x19, LD_PLL_ASR_19); 
	LD_WriteReg(0x1B, LD_PLL_ASR_1B);		
  	LD_WriteReg(0x1D, LD_PLL_ASR_1D);
	delay(10);
	
    LD_WriteReg(0xCD, 0x04);
//	LD_WriteReg(0x17, 0x4c); 
	delay(5);
	LD_WriteReg(0xB9, 0x00);
	LD_WriteReg(0xCF, 0x4F); 
	LD_WriteReg(0x6F, 0xFF); 
}

/************************************************************************
功能描述： 	 LD模块 ASR功能初始化
入口参数：	 none
返 回 值： 	 none
其他说明：	 该函数为出厂配置，一般不需要修改；
					 有兴趣的客户可对照开发手册根据需要自行修改。
**************************************************************************/
void LD3320ASRInit()
{
	LD3320CommonInit();
	LD_WriteReg(0xBD, 0x00);
	LD_WriteReg(0x17, 0x48);
	delay( 10 );
	LD_WriteReg(0x3C, 0x80);    
	LD_WriteReg(0x3E, 0x07);
	LD_WriteReg(0x38, 0xff);    
	LD_WriteReg(0x3A, 0x07);
  LD_WriteReg(0x40, 0);          
	LD_WriteReg(0x42, 8);
	LD_WriteReg(0x44, 0);    
	LD_WriteReg(0x46, 8); 
	delay( 1 );
}

/************************************************************************
功能描述： 	中断处理函数
入口参数：	 none
返 回 值： 	 none
其他说明：	当LD模块接收到音频信号时，将进入该函数，
						判断识别是否有结果，如果没有从新配置寄
            存器准备下一次的识别。
**************************************************************************/
void ProcessInt0(void)
{
	uint8_t nAsrResCount=0;

	EX0=0;
	ucRegVal = LD_ReadReg(0x2B);
	LD_WriteReg(0x29,0) ;
	LD_WriteReg(0x02,0) ;
	if((ucRegVal & 0x10) &&
		LD_ReadReg(0xb2)==0x21 && 
		LD_ReadReg(0xbf)==0x35)			/*识别成功*/
	{	
		nAsrResCount = LD_ReadReg(0xba);
		if(nAsrResCount>0 && nAsrResCount<=4) 
		{
			nAsrStatus=LD_ASR_FOUND_OK;
		}
		else
	    {
			nAsrStatus=LD_ASR_FOUND_NONE;
		}	
	}															 /*没有识别结果*/
	else
	{	 
		nAsrStatus=LD_ASR_FOUND_NONE;
	}
		
  LD_WriteReg(0x2b, 0);
  LD_WriteReg(0x1C,0);/*写0:ADC不可用*/

	LD_WriteReg(0x29,0) ;
	LD_WriteReg(0x02,0) ;
	LD_WriteReg(0x2B,  0);
	LD_WriteReg(0xBA, 0);	
	LD_WriteReg(0xBC,0);	
	LD_WriteReg(0x08,1);	 /*清除FIFO_DATA*/
	LD_WriteReg(0x08,0);	/*清除FIFO_DATA后 再次写0*/


	EX0=1;
}

/************************************************************************
功能描述： 	运行ASR识别流程
入口参数：	none
返 回 值：  asrflag：1->启动成功， 0—>启动失败
其他说明：	识别顺序如下:
						1、runASR()函数实现了一次完整的ASR语音识别流程
						2、LD3320ASRInit() 函数实现了ASR初始化
						3、LD3320ASRAddCommand() 函数实现了添加关键词语到LD3320芯片中
						4、LD3320ASRStart()	函数启动了一次ASR语音识别流程					
						任何一次ASR识别流程，都需要按照这个顺序，从初始化开始
**************************************************************************/
uint8_t runASR(void)
{
	uint8_t i=0;
	uint8_t asrflag=0;
	for (i=0; i<5; i++)			//	防止由于硬件原因导致LD3320芯片工作不正常，所以一共尝试5次启动ASR识别流程
	{
		LD3320ASRInit();
		delay(50);
		if (LD3320ASRAddCommand()==0)
		{
			LD3320Reset();			//	LD3320芯片内部出现不正常，立即重启LD3320芯片
			delay(50);			//	并从初始化开始重新ASR识别流程
			continue;
		}
		delay(10);
		if (LD3320ASRStart() == 0)
		{
			LD3320Reset();			//	LD3320芯片内部出现不正常，立即重启LD3320芯片
			delay(50);			//	并从初始化开始重新ASR识别流程
			continue;
		}
		asrflag=1;
		break;					//	ASR流程启动成功，退出当前for循环。开始等待LD3320送出的中断信号
	}

	return asrflag;
}
/************************************************************************
功能描述：  检测LD模块是否空闲
入口参数：	none
返 回 值： 	flag：1-> 空闲
其他说明：	none
**************************************************************************/
uint8_t LD3320CheckStatu()
{
	uint8_t j;
	uint8_t flag = 0;
	for (j=0; j<10; j++)
	{
		if (LD_ReadReg(0xb2) == 0x21)
		{
			flag = 1;
			break;
		}
		delay(10);		
	}
	return flag;
}

/************************************************************************
功能描述： 	运行ASR
入口参数：	none
返 回 值： 	1：启动成功
其他说明：	none
**************************************************************************/
uint8_t LD3320ASRStart()
{
	EX0=0;
	LD_WriteReg(0x35, MIC_VOL);
	LD_WriteReg(0x1C, 0x09);
	LD_WriteReg(0xBD, 0x20);
	LD_WriteReg(0x08, 0x01);
	delay( 1 );
	LD_WriteReg(0x08, 0x00);
	delay( 1 );

	if(LD3320CheckStatu() == 0)
	{
		return 0;
	}
//	LD_WriteReg(0xB6, 0xa); //识别时间	 1S
//	LD_WriteReg(0xB5, 0x1E); //背景音段时间 300ms
	LD_WriteReg(0xB8, 10); //一次语音识别的周期：10S

//	LD_WriteReg(0x1C, 0x07); //配置双通道音频信号做为输入信号
	LD_WriteReg(0x1C, 0x0b); //配置麦克风做为输入信号


	LD_WriteReg(0xB2, 0xff);
	delay( 1);	
	LD_WriteReg(0x37, 0x06);
	delay( 1 );
    LD_WriteReg(0x37, 0x06);
		delay( 5 );
	LD_WriteReg(0x29, 0x10);
	
	LD_WriteReg(0xBD, 0x00);
	EX0=1;
	return 1;
}
/************************************************************************
功能描述： 向LD模块添加关键词
入口参数： none
返 回 值： flag：1->添加成功
其他说明： 用户修改.
					 1、根据如下格式添加拼音关键词，同时注意修改sRecog 和g_indexsBuffer 数组的长度
					 和对应变了k的循环置。拼音串和识别码是一一对应的。
					 2、开发者可以学习"语音识别芯片LD3320高阶秘籍.pdf"中
           关于垃圾词语吸收错误的用法，来提供识别效果。
					 3、”xiao jie “ 为口令，故在每次识别时，必须先发一级口令“小捷”
**************************************************************************/
uint8_t LD3320ASRAddCommand()
{
		uint8_t k, flag;
		uint8_t nAsrAddLength;
		
		flag = 1;
		for (k=0; k<VOCIE_MAX_NUMBER; k++)
		{
				
			if(LD3320CheckStatu() == 0)
			{
				flag = 0;
				break;
			}
			
			LD_WriteReg(0xc1, g_indexsBuffer[k] );
			LD_WriteReg(0xc3, 0 );
			LD_WriteReg(0x08, 0x04);
			delay(1);
			LD_WriteReg(0x08, 0x00);
			delay(1);
	
			for (nAsrAddLength=0; nAsrAddLength<VOCIE_LENGTH; nAsrAddLength++)
			{
				if (sRecog[k][nAsrAddLength] == 0)
					break;
				LD_WriteReg(0x5, sRecog[k][nAsrAddLength]);
			}
			LD_WriteReg(0xb9, nAsrAddLength);
			LD_WriteReg(0xb2, 0xff);
			LD_WriteReg(0x37, 0x04);
		}

	    return flag;
}
/************************************************************************
功能描述： 	获取识别结果
入口参数：	none
返 回 值： 	LD_ReadReg(0xc5 )；  读取内部寄存器返回识别码。
其他说明：	none
**************************************************************************/
uint8_t LD3320GetResult()
{		
	return LD_ReadReg(0xc5 );
}
