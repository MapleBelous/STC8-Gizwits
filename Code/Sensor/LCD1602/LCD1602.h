#ifndef _LCD1602_H_
#define _LCD1602_H_
#include "MCUdef.h"
#include "delay.h"
//------------------------------------------------------------------------------------------------//
extern void LCD1602Init(void); //��ʼ��LCD1602
extern void LCD1602WriteLine(uchar *Str,bool isLine2);//��1/2��д���ַ���
extern uchar LCD1602ReadIdx(void); //��ȡ��ǰDDRAMλ��(���λ��)
extern uchar LCD1602ReadData(void); //��ȡDDRAM/CGRAM����
extern void LCD1602SetDDRAM(uchar idx); //0x00-0x0F-0x27,0x40-0x4F-0x67,�޸�DDRAMλ��(���λ��)
extern void LCD1602WriteCmdOrData(uchar CmdOrData, bool isData); //д������/DDRAM or CGRAM����
//------------------------------------------------------------------------------------------------//
#define LCD1602CleanScr 0x01  //������Ļ,��귵�ص�ַ00Hλ��
#define LCD1602CurReturn 0x02 //��귵�ص�ַ00Hλ��

#define LCD1602CurLeft 0x04  //��orд����Զ�����,��Ļ���ƶ�***��ǰʵ����������Ч
#define LCD1602CurLeftScrMov 0x05     //��orд����Զ�����,��Ļ����***��ǰʵ����������Ч
#define LCD1602CurRight 0x06 //��orд����Զ�����,��Ļ���ƶ�
#define LCD1602CurRightScrMov 0x07    //��orд����Զ�����,��Ļ����

#define LCD1602CloseDisplay 0x08 //�ر���ʾ
#define LCD1602NotCur 0x0C       //����ʾ-�޹��
#define LCD1602CurNotBlink 0x0E  //����ʾ-�й��-��겻��˸
#define LCD1602CurBlink 0x0F     //����ʾ-�й��-�����˸

#define LCD1602CurMovLeft 0x10  //�������һλ
#define LCD1602CurMovRight 0x14 //�������һλ
#define LCD1602ScrMovLeft 0x18  //��Ļ����һλ
#define LCD1602ScrMovRight 0x1C //��Ļ����һλ

#define LCD1602Data8Dis1Mat5x7 0x30  //8λ���ݿ�,1����ʾ,5*7����
#define LCD1602Data8Dis1Mat5x10 0x34 //8λ���ݿ�,1����ʾ,5*10����***ʵ��5*10������Ч
#define LCD1602Data8Dis2Mat5x7 0x38  //8λ���ݿ�,2����ʾ,5*7����
#define LCD1602Data8Dis2Mat5x10 0x3C //8λ���ݿ�,2����ʾ,5*10����***ʵ��5*10������Ч
//------------------------------------------------------------------------------------------------//
#endif