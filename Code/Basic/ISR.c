#include "GizwitsHandle.h"
#include "MCUdef.h"
#include "delay.h"
#include "uart.h"
//------------------------------------------------------------------------------------------------//
#ifdef _ISR0_
void INT0_ISR(void) interrupt 0
{
}
#endif
#ifdef _ISR1_
void TM0_ISR(void) interrupt 1
{
    ++(GizwitsST.GizwitsSysMs); //����ϵͳʱ�� ms
}
#endif
#ifdef _ISR2_
void INT1_ISR(void) interrupt 2
{
}
#endif
#ifdef _ISR3_
void TM1_ISR(void) interrupt 3
{
}
#endif
#ifdef _ISR4_
void UART1_ISR(void) interrupt 4
{
    if (TI)
        TI = 0, uart1_busy = false;
}
#endif
#ifdef _ISR5_
void ADC_ISR(void) interrupt 5
{
}
#endif
#ifdef _ISR6_
void LVD_ISR(void) interrupt 6
{
}
#endif
#ifdef _ISR7_
void PCA_ISR(void) interrupt 7
{
}
#endif
#ifdef _ISR8_
void UART2_ISR(void) interrupt 8
{
}
#endif
#ifdef _ISR9_
void SPI_ISR(void) interrupt 9
{
}
#endif
#ifdef _ISR10_
void INT2_ISR(void) interrupt 10
{
}
#endif
#ifdef _ISR11_
void INT3_ISR(void) interrupt 11
{
}
#endif
#ifdef _ISR12_
void TM2_ISR(void) interrupt 12
{
}
#endif
#ifdef _ISR13_
void INT4_ISR(void) interrupt 16
{
}
#endif
#ifdef _ISR14_
void UART3_ISR(void) interrupt 17
{
}
#endif
#ifdef _ISR15_
void UART4_ISR(void) interrupt 18
{
    if (S4CON & S4RI)
    {
        S4CON &= ~S4RI;
        LED_GREEN = 1;
        if ((uart4_idx2 + 1) % uart4_buffer_size == uart4_idx1) //���ڻ���������,������ǰ����
        {                                                       //���ܴ�ӡ��־,���ܵ��¶�ջ����
            LED_GREEN = 0;                                      //���̵Ʊ�ʾ���ڻ�������������
            return;
        }
        uart4_buffer[uart4_idx2] = S4BUF;
        if (uart4_idx2 + 1 == uart4_buffer_size)
            uart4_idx2 = 0;
        else
            ++uart4_idx2;
    }
    else
        S4CON &= ~S4TI, uart4_busy = false;
}
#endif
#ifdef _ISR16_
void TM3_ISR(void) interrupt 19
{
}
#endif
#ifdef _ISR17_
void TM4_ISR(void) interrupt 20
{
}
#endif
#ifdef _ISR18_
void CMP_ISR(void) interrupt 21
{
}
#endif
#ifdef _ISR19_
void PWM_ISR(void) interrupt 22
{
}
#endif
#ifdef _ISR20_
void PWMFD_ISR(void) interrupt 23
{
}
#endif
#ifdef _ISR21_
void I2C_ISR(void) interrupt 24
{
}
//------------------------------------------------------------------------------------------------//
#endif
