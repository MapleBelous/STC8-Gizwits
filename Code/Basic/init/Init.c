#include "Init.h"
//------------------------------------------------------------------------------------------------//
static void Init_rst(void);
static void Init_io(void);
static void Init_timer0(void);
static void Init_uart1(void);
static void Init_uart4(void);
static void Init_wkt(float,bool);
static void Init_interrupt(void);
//------------------------------------------------------------------------------------------------//

void Init(void)
{
    //�ڲ�
    Init_rst();       //��ѹ��λ&���Ź���ʼ��
    Init_io();        //IO�ڳ�ʼ��
    Init_timer0();    //��ʱ��0��ʼ��
#if LOGRANK_UART1 >= 1
    Init_uart1();     //����1-USB���Գ�ʼ��
#endif
    Init_uart4();     //����4-Wifi ESP8266��ʼ��
	Init_wkt(1.0,false);//���õ��绽�Ѽ�ʱ��,��λms.����0.5ms-min(0),16384ms-max(32767);ÿ������0.5ms
    Init_interrupt(); //�ж�ϵͳ��ʼ��-���� ���ȼ�
#if LOGRANK_UART1 >= 2
        printf("LOG#:Internal Init Finish\r\n");
#endif
	
    //�ⲿ
    GizwitsInit();   //��ʼ��Gizwits�ڴ�
    DS18B20STInit(); //��ʼ��DS18B20�ڴ�&��ȡ��ǰDS18B20�ֱ���
	//LCD1602Init();    ////��ʼ��LCD1602
#if LOGRANK_UART1 >= 2
    printf("LOG#:External Init Finish\r\n");
#endif
	
	uart4_idx1 = uart4_idx2 = 0;//��ʼ�����,��ʼ���մ���4��Ϣ
}
void Init_rst(void)
{
    //���Ź��Ĵ��� // WDT_FLAG - EN_WDT CLR_WDT IDL_WDT WDT_PS[2:0]
    WDT_CONTR = WDT_CONTR & ~EN_WDT;//���ؿ��Ź�
	//WDT_CONTR |= 0x07;//���Ź���Ƶϵ�����
    //��ѹ��λ/RST���� ���üĴ��� // - ENLVR - P54RST - - LVDS[1:0]
    RSTCFG = RSTCFG & ~0x40 & ~0x10;
    RSTCFG &= 0xFC; //LVDS[1:0] ��ѹ����ż���ѹ:00 2.2V/01 2.4V/10 2.7V/11 3.0V
}
void Init_io(void)
{
    P3M1 &= 0xFE, P3M0 &= 0xFE; //P3.0Ϊ׼˫���
    P3M1 &= 0xFD, P3M0 |= 0x02; //P3.1Ϊ�������

    P0M1 &= 0xFB, P0M0 &= 0xFB; //P0.2Ϊ׼˫���
    P0M1 &= 0xF7, P0M0 |= 0x08; //P0.3Ϊ�������

    P3M1 &= 0xBF, P3M0 |= 0x40; //P3.6Ϊ�������
}
void Init_timer0(void)
{
    AUXR |= T0x12; //��ʱ��ʱ��1Tģʽ
    TMOD &= 0xF0;  //���ö�ʱ��ģʽ
    TL0 = 0x9A;    //���ö�ʱ��ֵ
    TH0 = 0xA9;    //���ö�ʱ��ֵ
    TF0 = 0;       //���TF0��־
    TR0 = 1;       //��ʱ��0��ʼ��ʱ
}
#if LOGRANK_UART1 >= 1
void Init_uart1(void)
{
    ulong BTL = UART_16(115200, 1);
    SCON = 0x40;                 // SM0/FE [SM1] SM2 REN TB8 RB8 TI RI
    PCON &= 0x3F;                // [~SMOD ~SMOD0] LVDF POF GF1 GF0 PD IDL
    AUXR = AUXR | T2x12 | S1ST2; // T0x12 T1x12 UART_M0x6 T2R T2_C/T [T2x12] EXTRAM [S1ST2]
    T2L = BTL & 0xFF;
    T2H = BTL >> 8;
    uart1_busy = false;
    AUXR |= T2R; //������ʱ��2
}
#endif
void Init_uart4(void)
{
    //ʹ�ö�ʱ��4 �Զ�����
    ulong BTL = UART_16(9600, 1);
    S4CON = 0x50;   // [~S4SM0] [S4ST4] S4SM2 [S4REN] S4TB8 S4RB8 S4TI S4RI
    T4T3M |= T4x12; // T4R T4_C/T [T4x12] T4CLKO T3R T3_C/T T3x12 T3CLKO
    T4L = BTL & 0xFF;
    T4H = BTL >> 8;
    uart4_busy = false;
    //uart4_idx1 = uart4_idx2 = 0;
    T4T3M |= T4R; //������ʱ��4
    //���ö�ʱ��2
    //	S4CON = 0x10;// S4SM0 S4ST4 S4SM2 S4REN S4TB8 S4RB8 S4TI S4RI
    //	uart4_busy=false;
    //	uart4_idx=0;//���û�����λ��
}
void Init_wkt(float ms,bool en)//���õ��绽�Ѽ�ʱ��,��λms.����0.5ms-min(0),16384ms-max(32767);ÿ������0.5ms
{
	pdata ushort T;
	if(en==false)
		return;
	T = ((ms*2.0)-1);
	T &= 0x7FFF;
	WKTCL = T&0x00FF;
	WKTCH = (T>>8)&0x007F;
	WKTCH |= 0x80;//�������绽�Ѽ�ʱ��
}
void Init_interrupt(void)
{
    ET0 = 1;                 //������ʱ��0�ж�
    ES = 1, IE2 &= ~ET2;     //��������1�ж�,�رն�ʱ��2�ж�
    IE2 |= ES4, IE2 &= ~ET4; //��������4�ж�,�رն�ʱ��4�ж�

    PT0 = 1, IPH |= PT0H; //��ʱ��0���ȼ�3,ϵͳʱ�価���ܾ�ȷ,������ȼ�
    PS = 1, IPH &= ~PSH;  //����1���ȼ�1,�����ڴ���4ISR������

    EA = 1; //�������жϿ���
}

//------------------------------------------------------------------------------------------------//