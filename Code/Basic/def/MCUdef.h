#ifndef _MCUdef_H_
#define _MCUdef_H_
//------------------------------------------------------------------------------------------------//
//Basic System Head File
#include <STC8.h>
#include <intrins.h>

#include <limits.h>
#include <stddef.h>
#include <stdio.h>
//------------------------------------------------------------------------------------------------//
//Basic User Head File
#include "pindef.h"
//------------------------------------------------------------------------------------------------//

//Clock (hz)
#define _CPU_CLOCK_HZ_ (22118400UL)

//------------------------------------------------------------------------------------------------//
//---------------�ж�ISR------------------//

#define _ISR1_  //��ʱ��0
#define _ISR4_  //����1
#define _ISR15_ //����4

//------------------------------------------------------------------------------------------------//

//����1��ӡ��־����,0:����־,1:ERR,2:ERR+LOG#,3:ERR+LOG#+LOG
#define LOGRANK_UART1 3

//------------------------------------------------------------------------------------------------//

//����stdbool.h
#ifndef __bool_true_false_are_defined
#define true 1
#define false 0
typedef bit bool; //λѰַ�Ĵ���ר��,��֧��C99,�Ͻ����ڹ����ṹ��
#endif

//Type Def,����stdint.h
typedef unsigned char uchar;   //uchar->u8
typedef unsigned short ushort; //ushort->u16
typedef unsigned int uint;     //uint->u16[u32]
typedef unsigned long ulong;   //uchar->u32
//����
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed long int32_t;
typedef unsigned long uint32_t;

//BOOL EXIT 
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

//Necessary Define Function
#define ioread_b(x) ((x) = true, _nop_, _nop_, (x)) //׼˫��ģʽ,���ô�������1����
#define ioread_p(x) ((x) = 0xFF, _nop_, _nop_, (x)) //׼˫��ģʽ,���ô�������8����
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define ABS(x) ((x) < 0 ? (-(x)) : (x))
#ifdef __STC8F_H_
#define MCURST()                                \
    do                                          \
    {                                           \
        IAP_CONTR &= ~SWBS, IAP_CONTR |= SWRST; \
    } while (false) //����STC8���ø�λ�Ĵ�����λ
#else
#define MCURST_OLD() (*((void(code *)(void))0x0000))() //ͨ�ú���ָ�븴λ
#endif
//------------------------------------------------------------------------------------------------//
#endif