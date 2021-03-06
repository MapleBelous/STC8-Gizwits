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
    //内部
    Init_rst();       //低压复位&看门狗初始化
    Init_io();        //IO口初始化
    Init_timer0();    //定时器0初始化
#if LOGRANK_UART1 >= 1
    Init_uart1();     //串口1-USB调试初始化
#endif
    Init_uart4();     //串口4-Wifi ESP8266初始化
	Init_wkt(1.0,false);//设置掉电唤醒计时器,单位ms.允许0.5ms-min(0),16384ms-max(32767);每次增长0.5ms
    Init_interrupt(); //中断系统初始化-开关 优先级
#if LOGRANK_UART1 >= 2
        printf("LOG#:Internal Init Finish\r\n");
#endif
	
    //外部
    GizwitsInit();   //初始化Gizwits内存
    DS18B20STInit(); //初始化DS18B20内存&读取当前DS18B20分辨率
	//LCD1602Init();    ////初始化LCD1602
#if LOGRANK_UART1 >= 2
    printf("LOG#:External Init Finish\r\n");
#endif
	
	uart4_idx1 = uart4_idx2 = 0;//初始化完毕,开始接收串口4信息
}
void Init_rst(void)
{
    //看门狗寄存器 // WDT_FLAG - EN_WDT CLR_WDT IDL_WDT WDT_PS[2:0]
    WDT_CONTR = WDT_CONTR & ~EN_WDT;//开关看门狗
	//WDT_CONTR |= 0x07;//看门狗分频系数最大
    //低压复位/RST引脚 配置寄存器 // - ENLVR - P54RST - - LVDS[1:0]
    RSTCFG = RSTCFG & ~0x40 & ~0x10;
    RSTCFG &= 0xFC; //LVDS[1:0] 低压检测门槛电压:00 2.2V/01 2.4V/10 2.7V/11 3.0V
}
void Init_io(void)
{
    P3M1 &= 0xFE, P3M0 &= 0xFE; //P3.0为准双向口
    P3M1 &= 0xFD, P3M0 |= 0x02; //P3.1为推挽输出

    P0M1 &= 0xFB, P0M0 &= 0xFB; //P0.2为准双向口
    P0M1 &= 0xF7, P0M0 |= 0x08; //P0.3为推挽输出

    P3M1 &= 0xBF, P3M0 |= 0x40; //P3.6为推挽输出
}
void Init_timer0(void)
{
    AUXR |= T0x12; //定时器时钟1T模式
    TMOD &= 0xF0;  //设置定时器模式
    TL0 = 0x9A;    //设置定时初值
    TH0 = 0xA9;    //设置定时初值
    TF0 = 0;       //清除TF0标志
    TR0 = 1;       //定时器0开始计时
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
    AUXR |= T2R; //开启定时器2
}
#endif
void Init_uart4(void)
{
    //使用定时器4 自动重载
    ulong BTL = UART_16(9600, 1);
    S4CON = 0x50;   // [~S4SM0] [S4ST4] S4SM2 [S4REN] S4TB8 S4RB8 S4TI S4RI
    T4T3M |= T4x12; // T4R T4_C/T [T4x12] T4CLKO T3R T3_C/T T3x12 T3CLKO
    T4L = BTL & 0xFF;
    T4H = BTL >> 8;
    uart4_busy = false;
    //uart4_idx1 = uart4_idx2 = 0;
    T4T3M |= T4R; //开启定时器4
    //共用定时器2
    //	S4CON = 0x10;// S4SM0 S4ST4 S4SM2 S4REN S4TB8 S4RB8 S4TI S4RI
    //	uart4_busy=false;
    //	uart4_idx=0;//重置缓冲区位置
}
void Init_wkt(float ms,bool en)//设置掉电唤醒计时器,单位ms.允许0.5ms-min(0),16384ms-max(32767);每次增长0.5ms
{
	pdata ushort T;
	if(en==false)
		return;
	T = ((ms*2.0)-1);
	T &= 0x7FFF;
	WKTCL = T&0x00FF;
	WKTCH = (T>>8)&0x007F;
	WKTCH |= 0x80;//开启掉电唤醒计时器
}
void Init_interrupt(void)
{
    ET0 = 1;                 //开启定时器0中断
    ES = 1, IE2 &= ~ET2;     //开启串口1中断,关闭定时器2中断
    IE2 |= ES4, IE2 &= ~ET4; //开启串口4中断,关闭定时器4中断

    PT0 = 1, IPH |= PT0H; //定时器0优先级3,系统时间尽可能精确,最高优先级
    PS = 1, IPH &= ~PSH;  //串口1优先级1,不能在串口4ISR中阻塞

    EA = 1; //开启总中断开关
}

//------------------------------------------------------------------------------------------------//