#include "LCD1602.h"
//------------------------------------------------------------------------------------------------//
//������ģ
code uchar LCD1602Word_Temperature[8]={0x10,0x06,0x09,0x08,0x08,0x09,0x06,0x00};//���϶ȵ�λ,CGRAM=0
code uchar LCD1602Word_Heart[8]={0x00,0x0A,0x1F,0x1F,0x0E,0x04,0x00,0x00};//����,CGRAM=1
//------------------------------------------------------------------------------------------------//
static void LCD1602Wait(void); //�ȴ�LCD1602����
static void LCD1602LoadWord(uchar Idx,const uchar *Word);//������ģ��CGRAM
static void LCD1602SetCGRAM(uchar idx); //0x00-0x07,�޸�CGRAMλ��(������ģRAMλ��)
//------------------------------------------------------------------------------------------------//
void LCD1602Init(void) //��ʼ��LCD1602
{
	LCD1602_EN = 0;//���ֲ�����ʱ����EN
    LCD1602WriteCmdOrData(LCD1602Data8Dis2Mat5x7, 0);
    LCD1602WriteCmdOrData(LCD1602NotCur, 0);
    LCD1602WriteCmdOrData(LCD1602CurRight, 0);
    LCD1602WriteCmdOrData(LCD1602CleanScr, 0);
	//����������ģ
	LCD1602LoadWord(0,LCD1602Word_Temperature);
	LCD1602LoadWord(1,LCD1602Word_Heart);
	LCD1602WriteLine("Ready",false);
#if LOGRANK_UART1 >= 2
    printf("LOG#:LCD1602Init ok\r\n");
#endif
}
void LCD1602WriteLine(uchar *Str,bool isLine2)//��1/2��д���ַ���
{
	uchar i;
	isLine2?LCD1602SetDDRAM(0x40):LCD1602SetDDRAM(0x00);
	for(i=0;Str[i]&&i!=40;++i)
		LCD1602WriteCmdOrData(Str[i],1);
}
uchar LCD1602ReadIdx(void) //��ȡ��ǰDDRAMλ��(���λ��)
{
    uchar Idx;
    LCD1602Wait();
    LCD1602_RS = 0;
    LCD1602_RW = 1;
    LCD1602_EN = 1;
	LCD1602_DATA=0xFF;
	delay_us(40);//�ȴ���
	Idx=LCD1602_DATA;
    LCD1602_EN = 0;
    return Idx;
}
uchar LCD1602ReadData(void) //��ȡDDRAM/CGRAM����
{
    uchar Data;
    LCD1602Wait();
    LCD1602_RS = 1;
    LCD1602_RW = 1;
    LCD1602_EN = 1;
	LCD1602_DATA=0xFF;
	delay_us(40);//�ȴ���
	Data=LCD1602_DATA;
	LCD1602_EN = 0;
    return Data;
}
void LCD1602SetDDRAM(uchar idx) //0x00-0x0F-0x27,0x40-0x4F-0x67,�޸�DDRAMλ��(���λ��)
{
    idx |= 0x80;
    LCD1602WriteCmdOrData(idx, 0);
}
void LCD1602WriteCmdOrData(uchar CmdOrData, bool isData) //д������/DDRAM or CGRAM����
{
    LCD1602Wait();
    LCD1602_RS = isData;
    LCD1602_RW = 0;
    LCD1602_DATA = CmdOrData;
    LCD1602_EN = 1;
    LCD1602_EN = 0;
}
//------------------------------------------------------------------------------------------------//
static void LCD1602LoadWord(uchar Idx,const uchar *Word)//������ģ��CGRAM
{
	uchar i;
	delay_ms(1);
	LCD1602SetCGRAM(Idx);
	delay_ms(1);
	for(i=0;i!=8;++i)
		LCD1602WriteCmdOrData(Word[i],1);
}
static void LCD1602SetCGRAM(uchar idx) //0x00-0x07,�޸�CGRAMλ��(������ģRAMλ��)
{
    idx &= 0x07, idx <<= 3, idx |= 0x40;
    LCD1602WriteCmdOrData(idx, 0);
}
static void LCD1602Wait(void) //�ȴ�LCD1602����
{
    bool isBusy;
    LCD1602_RS = 0;
    LCD1602_RW = 1;
    do
    {
        LCD1602_EN = 1;
		LCD1602_Busy=1;
		delay_us(40);//�ȴ���
		isBusy=LCD1602_Busy;
		LCD1602_EN = 0;
    } while (isBusy);
}
//------------------------------------------------------------------------------------------------//