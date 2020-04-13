#ifndef _uart_H_
#define _uart_H_
#include "MCUdef.h"
#include "pindef.h"
//------------------------------------------------------------------------------------------------//
//���ڽ��ջ�������С
#define uart1_buffer_size 
#define uart2_buffer_size 
#define uart3_buffer_size 
#define uart4_buffer_size 1000
//------------------------------------------------------------------------------------------------//
extern xdata uchar uart1_buffer[],uart2_buffer[],uart3_buffer[],uart4_buffer[];//���ڽ��ջ�����
extern pdata ushort uart4_idx1,uart4_idx2;//���ڻ���������

extern bit uart1_busy,uart2_busy,uart3_busy,uart4_busy;//���ڷ�����æ��־
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