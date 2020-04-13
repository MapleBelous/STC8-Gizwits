#ifndef _uart_H_
#define _uart_H_
#include "MCUdef.h"
#include "pindef.h"
//------------------------------------------------------------------------------------------------//
//串口接收缓冲区大小
#define uart1_buffer_size 
#define uart2_buffer_size 
#define uart3_buffer_size 
#define uart4_buffer_size 1000
//------------------------------------------------------------------------------------------------//
extern xdata uchar uart1_buffer[],uart2_buffer[],uart3_buffer[],uart4_buffer[];//串口接收缓冲区
extern pdata ushort uart4_idx1,uart4_idx2;//串口缓冲区索引

extern bit uart1_busy,uart2_busy,uart3_busy,uart4_busy;//串口发送正忙标志
//------------------------------------------------------------------------------------------------//

void uart1_send8(uchar dat);
//void uart1_sendstr8(uchar *p,uchar n);

//void uart2_send8(uchar dat);
//void uart2_sendstr8(uchar *p,uchar n);

//void uart3_send8(uchar dat);
//void uart3_sendstr8(uchar *p,uchar n);

void uart4_send8(uchar dat);
//void uart4_sendstr8(uchar *p,uchar n);

//------------------------------------------------------------------------------------------------//
#endif