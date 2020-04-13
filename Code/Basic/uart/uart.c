#include "uart.h"
//------------------------------------------------------------------------------------------------//
xdata uchar uart4_buffer[uart4_buffer_size];
pdata ushort uart4_idx1, uart4_idx2;

bit uart1_busy, uart4_busy;
//------------------------------------------------------------------------------------------------//
#if LOGRANK_UART1 >= 1
//-----------重写putchar函数-----------//
//----用于printf函数向串口1发送数据----//
char putchar(char ch)
{
    uart1_send8(ch);
    return 1;
}
void uart1_send8(uchar dat)
{
    while (uart1_busy)
        ;
    uart1_busy = true;
    SBUF = dat;
    while (uart1_busy)
        ;
}
#endif
//void uart1_sendstr8(uchar *p,uchar n)
//{
//	if(n)
//		while(n--)
//			uart1_send8(*p++);
//	else
//		while(*p)
//			uart1_send8(*p++);
//}

//void uart2_send8(uchar dat)
//{
//    while (uart2_busy);
//	uart2_busy=true;
//    S2BUF = dat;
//    while (uart2_busy);
//}
//void uart2_sendstr8(uchar *p,uchar n)
//{
//	if(n)
//		while(n--)
//			uart2_send8(*p++);
//	else
//		while(*p)
//			uart2_send8(*p++);
//}

//void uart3_send8(uchar dat)
//{
//    while (uart3_busy);
//	uart3_busy=true;
//    S3BUF = dat;
//    while (uart1_busy);
//}
//void uart3_sendstr8(uchar *p,uchar n)
//{
//	if(n)
//		while(n--)
//			uart3_send8(*p++);
//	else
//		while(*p)
//			uart3_send8(*p++);
//}

void uart4_send8(uchar dat)
{
    while (uart4_busy)
        ;
    uart4_busy = true;
    S4BUF = dat;
    while (uart4_busy)
        ;
}
//void uart4_sendstr8(uchar *p,uchar n)
//{
//	if(n)
//		while(n--)
//			uart4_send8(*p++);
//	else
//		while(*p)
//			uart4_send8(*p++);
//}
//------------------------------------------------------------------------------------------------//