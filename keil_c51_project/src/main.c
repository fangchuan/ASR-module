#include "config.h"
#include "string.h"

/************************************************************************************/
//	nAsrStatus 用来在main主程序中表示程序运行的状态，不是LD3320芯片内部的状态寄存器
//	LD_ASR_IDLE:		表示没有在作ASR识别
//	LD_ASR_RUNING：		表示LD3320正在作ASR识别中
//	LD_ASR_FOUND_OK:		表示一次识别流程结束后，有一个识别结果
//	LD_ASR_FOUND_NONE:	表示一次识别流程结束后，没有识别结果
//	LD_ASR_ERROR:		表示一次识别流程中LD3320芯片内部出现不正确的状态
/***********************************************************************************/

void mcuInit(); 
void ProcessInt0(); //识别处理函数
void  delay(unsigned long uldata);
void 	userHandle(uint8_t dat);//用户执行操作函数
void Delay200ms();
void ledTest(void);//单片机工作指示

sbit LED=P4^2;//信号指示灯
uint8_t idata nAsrStatus=0;	

extern volatile uint8_t  sRecog[VOCIE_MAX_NUMBER][VOCIE_LENGTH];
extern uint8_t   rxBuffer[20];
extern uint8_t   rxFlag;
/***********************************************************
* 名    称： void  main(void)
* 功    能： 主函数	程序入口
* 入口参数：  
* 出口参数：
* 说    明： 					 
* 调用方法： 
**********************************************************/ 
void  main(void)
{
    uint8_t idata nAsrRes;
    uint8_t i=0;
    ledTest();
    mcuInit();
    LD3320Reset();
    uartInit(); /*串口初始化*/
    nAsrStatus = LD_ASR_IDLE;		//	初始状态：没有在作ASR

    while(1)
    {
        switch(nAsrStatus)
        {
        case LD_ASR_RUNING:
            break;
        case LD_ASR_ERROR:
            nAsrStatus = LD_ASR_IDLE;
            break;
        case LD_ASR_IDLE:
        {
            nAsrStatus=LD_ASR_RUNING;
            if (runASR()==0)	/*	启动一次ASR识别流程：ASR初始化，ASR添加关键词语，启动ASR运算*/
            {
                nAsrStatus = LD_ASR_ERROR;
            }
            break;
        }
        case LD_ASR_FOUND_OK: /*	一次ASR识别流程结束，去取ASR识别结果*/
        {
            nAsrRes = LD3320GetResult();		/*获取结果*/
            userHandle(nAsrRes);//用户执行函数
            nAsrStatus = LD_ASR_IDLE;
            break;
        }
        case LD_ASR_FOUND_NONE:
        default:
        {
//            printCom("NONE\r\n");
            nAsrStatus = LD_ASR_IDLE;
            break;
        }
        }// switch

        if(rxFlag == 1)
        {
            uint8_t returnValue;
            uint8_t index = rxBuffer[0]-0x30;
            uint8_t command[VOCIE_LENGTH] = {0};
            rxFlag= 0 ;
            printCom(rxBuffer+1);
            memcpy(command, rxBuffer+1, 19);
            memcpy(sRecog[index], command , VOCIE_LENGTH);
            memset(rxBuffer, 0, 20);
            returnValue = runASR();
            while(!returnValue)
            {
                returnValue = LD3320ASRAddCommand();
            }
        }
    }// while

}
/***********************************************************
* 名    称： 	 LED灯测试
* 功    能： 单片机是否工作指示
* 入口参数： 无 
* 出口参数：无
* 说    明： 					 
**********************************************************/
void ledTest(void)
{
    LED=~ LED;
    Delay200ms();
    LED=~ LED;
    Delay200ms();
    LED=~ LED;
    Delay200ms();
    LED=~ LED;
    Delay200ms();
    LED=~ LED;
    Delay200ms();
    LED=~ LED;
}
/***********************************************************
* 名    称： void mcuInit()
* 功    能： 单片机初始化
* 入口参数：  
* 出口参数：
* 说    明： 					 
* 调用方法： 
**********************************************************/ 
void mcuInit()
{
    P0 = 0xff;
    P1 = 0xff;
    P2 = 0xff;
    P3 = 0xff;
    P4 = 0xff;

    P1M0=0XFF;	//P1端口设置为推挽输出功能，即提高IO口驱动能力，从驱动继电器模块工作
    P1M1=0X00;

    LD_MODE = 0;		//	设置MD管脚为低，并行模式读写
    IE0=1;
    EX0=1;
    EA=1;
}
/***********************************************************
* 名    称：	延时函数
* 功    能：
* 入口参数：  
* 出口参数：
* 说    明： 					 
* 调用方法： 
**********************************************************/ 
void Delay200us()		//@22.1184MHz
{
    unsigned char i, j;
    _nop_();
    _nop_();
    i = 5;
    j = 73;
    do
    {
        while (--j);
    } while (--i);
}

void  delay(unsigned long uldata)
{
    unsigned int j  =  0;
    unsigned int g  =  0;
    while(uldata--)
        Delay200us();
}

void Delay200ms()		//@22.1184MHz
{
    unsigned char i, j, k;

    i = 17;
    j = 208;
    k = 27;
    do
    {
        do
        {
            while (--k);
        } while (--j);
    } while (--i);
}
/***********************************************************
* 名    称： 中断处理函数
* 功    能：
* 入口参数：  
* 出口参数：
* 说    明： 					 
* 调用方法： 
**********************************************************/ 
void ExtInt0Handler(void) interrupt 0  
{ 	
    ProcessInt0();				/*	LD3320 送出中断信号，包括ASR和播放MP3的中断，需要在中断处理函数中分别处理*/
}
/***********************************************************
* 名    称：用户执行函数 
* 功    能：识别成功后，执行动作可在此进行修改 
* 入口参数： 无 
* 出口参数：无
* 说    明： 					 
**********************************************************/
void 	userHandle(uint8_t dat)
{

    switch(dat)		   /*对结果执行相关操作,客户修改*/
    {
    case INDEX_APOLLO:

        printCom("0\r\n"); /*text.....*/
        break;
    case INDEX_ROBOT:

        printCom("1\r\n"); /*text.....*/
        break;
    case INDEX_FORWARD:
        printCom("2\r\n"); /*text.....*/
        break;
    case INDEX_BACKWARD:
        printCom("3\r\n"); /*text.....*/
        break;
    case INDEX_LEFT:
        printCom("4\r\n"); /*text.....*/
        break;
    case INDEX_RIGHT:
        printCom("5\r\n"); /*text.....*/
        break;
    case INDEX_STOP:
        printCom("6\r\n"); /*text.....*/
        break;
    case INDEX_SPEED_UP:
        printCom("7\r\n"); /*text.....*/
        break;
    case INDEX_SPEED_DOWN:
        printCom("8\r\n"); /*text.....*/
        break;
    case INDEX_TURN_ON:
        printCom("9\r\n"); /*text.....*/
        break;
    case INDEX_TURN_OFF:
        printCom("10\r\n"); /*text.....*/
        break;
    case INDEX_11:
        printCom("11\r\n"); /*text.....*/
        break;
    case INDEX_12:
        printCom("12\r\n"); /*text.....*/
        break;
    case INDEX_13:
        printCom("13\r\n"); /*text.....*/
        break;
    case INDEX_14:
        printCom("14\r\n"); /*text.....*/
        break;
    case INDEX_15:
        printCom("15\r\n"); /*text.....*/
        break;
    case INDEX_16:
        printCom("16\r\n"); /*text.....*/
        break;
    case INDEX_17:
        printCom("17\r\n"); /*text.....*/
        break;
    case INDEX_18:
        printCom("18\r\n"); /*text.....*/
        break;
    case INDEX_19:

        printCom("19\r\n"); /*text.....*/
        break;
    case INDEX_20:
        printCom("20\r\n"); /*text.....*/
        break;
    case INDEX_21:
        printCom("21\r\n"); /*text.....*/
        break;
    case INDEX_22:
        printCom("22\r\n"); /*text.....*/
        break;
    case INDEX_23:
        printCom("23\r\n"); /*text.....*/
        break;
    case INDEX_24:
        printCom("24\r\n"); /*text.....*/
        break;
    case INDEX_25:
        printCom("25\r\n"); /*text.....*/
        break;
    case INDEX_26:
        printCom("26\r\n"); /*text.....*/
        break;
    case INDEX_27:
        printCom("27\r\n"); /*text.....*/
        break;
    case INDEX_28:
        printCom("28\r\n"); /*text.....*/
        break;
    case INDEX_29:
        printCom("29\r\n"); /*text.....*/
        break;
    case INDEX_30:
        printCom("30\r\n"); /*text.....*/
        break;
    case INDEX_31:
        printCom("31\r\n"); /*text.....*/
        break;
    case INDEX_32:
        printCom("32\r\n"); /*text.....*/
        break;
    case INDEX_33:
        printCom("33\r\n"); /*text.....*/
        break;
    case INDEX_34:
        printCom("34\r\n"); /*text.....*/
        break;

    default:printCom("NONE\r\n"); /*text.....*/
        break;
    }
}	 
