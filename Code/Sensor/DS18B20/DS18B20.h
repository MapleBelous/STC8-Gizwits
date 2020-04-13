#ifndef _DS18B20_H_
#define _DS18B20_H_
#include "GizwitsHandle.h"
#include "MCUdef.h"
#include "delay.h"
#include "string.h"
//DS18B20�¶ȴ�����,������Χ-55~125
//RAM 0~1:�¶ȼĴ���[2^-4,2^6]ʣ��Ϊ����λ
//    2~3:TH\TL���µ��±�����ֵ(��E2C),[2^0,2^6]ʣ��Ϊ����λ
//      4:���üĴ���(��E2C),0~4/7����,5-R0,6-R1
//        Mode=00,9λ,���ת����ʱ93.75ms;Mode=01,10λ,���ת����ʱ187.5ms;
//        Mode=10,11λ,���ת����ʱ375ms;Mode=11,12λ,���ת����ʱ750ms;
//    5~7:����
//      8:CRCУ��
//------------------------------------------------------------------------------------------------//
typedef struct DS18B20ActST //DS18B20����ָʾ��
{
    ushort TemperatureData : 12;                       //�������һ���¶�ֵ
    uchar TemperatureHigh : 1;                         //���¾���
    uchar TemperatureLow : 1;                          //���¾���
    ushort TemperatureHighData1, TemperatureHighData2; //���¾�����ֵ,���¾���ȡ����ֵ
    ushort TemperatureLowData1, TemperatureLowData2;   //���¾�����ֵ,���¾���ȡ����ֵ
    uchar ResolutionMode : 2;                          //��ǰDS18B20�ֱ���
} DS18B20ActST;
extern xdata DS18B20ActST DS18B20ST; //DS18B20����ָʾ��
extern code ushort DS18B20ConvertTMaxTime[];
//------------------------------------------------------------------------------------------------//
extern void DS18B20STInit(void);                      //��ʼ��DS18B20�ڴ�&��ȡ��ǰDS18B20�ֱ���
extern bool DS18B20Set(char TH, char TL, uchar Mode); //���ò�����TH/TL/Mode
extern bool DS18B20ConvertTemperature(void);          //��ʼת���¶�ֵ
extern bool DS18B20GetTemperature(void);              //��ȡ�¶�ֵ
//------------------------------------------------------------------------------------------------//
//DEF
#define DS18B20MaxT 125                                                              //DS18B20����¶�
#define DS18B20MinT -55                                                              //DS18B20����¶�
#define TemperatureH TemperatureWaring(30)                                           //���±����¶�
#define TemperatureL TemperatureWaring(22)                                           //���±����¶�
#define TemperatureT (1 << (DS18B20ST.ResolutionMode))                               //��������ֹ����ֵ�� 0.5 ��
#define TemperatureWaring(x) ((x - (DS18B20MinT)) << (1 + DS18B20ST.ResolutionMode)) //�¶�ת��Ϊ�޷�����

//ROM order
#define DS18B20ReadROM 0x33     //��64λROM��ַ(����DS18B20����)
#define DS18B20MatchROM 0x55    //ROMƥ��,��֮�󷢳�64λROM����,�õ�ַ�����DS18B20��Ӧ
#define DS18B20SearchROM 0xF0   //ȷ��������DS18B20������ʶ��64λROM��ַ
#define DS18B20SkipROM 0xCC     ////����64λROM��ַ
#define DS18B20AlarmSearch 0xEC //�¶�Խ�޵�DS18B20��Ӧ

//RAM order
#define DS18B20ConvertT 0x44        //����DS18B20�¶�ת��
#define DS18B20ReadRAM 0xBE         //��9�ֽ�RAM
#define DS18B20WriteRAM 0x4E        //д��TH\TL\Mode
#define DS18B20CopyRAM 0x48         //����RAM��TH\TL��E2PROM
#define DS18B20CopyEEPROM 0xB8      //��E2PROM����ȡTH\TL��RAM
#define DS18B20ReadPowerSupply 0xB4 //��ȡ��ǰ����ģʽ,1:��ӵ�Դ;0:��������
//------------------------------------------------------------------------------------------------//
#endif