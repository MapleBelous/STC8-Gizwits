#ifndef _GizwitsHandle_H_
#define _GizwitsHandle_H_
#include "GizwitsMSG.h"
#include "MCUdef.h"
#include "uart.h"
#include <string.h>
//------------------------------------------------------------------------------------------------//
void GizwitsMainLoop(void); //Gizwits��ѭ������
void GizwitsInit(void);     //��ʼ��Gizwits�ڴ�

void GizwitsSend(uchar Mode); //����GizwitsSendBuffer�������ڵ����ݰ�&��¼�ѷ������ݰ�
//GizwitsSendBuffer��������Ӧ����Cmd-����ȫ������,GizwitsSendIdxΪ���ݰ���С(�������ͷ+Len+Sum)
void GizwitsReAsk(uchar); //��ʱ�ȴ�WiFiģ��Ans&���·���MCU��Ask���ݰ�
//------------------------------------------------------------------------------------------------//
//++++++++��ʱ����ʱ����(Ms),0Ϊ��ִ��++++++++//
#define GizwitsNeedReportMs 0 //����������Ҫ�����ϱ��������
#define DS18B20NeedReadMs 1000 //DS18B20�ɼ��¶����ݼ��

//++++++++++++++++++++++++++++++++++++++++++++++//
#define GizwitsUseWiFiRealTime //�����WiFi��ȡ��ʵʱ���ָ��
#define GizwitsUseWiFiProperty //�����WiFi��ȡWiFi���豸��Ϣָ��
//++++++++++++++++++++++++++++++++++++++++++++++//
#define GizwitsReceiveBufferSize 500 //GizwitsReceive����������
#define GizwitsSendBufferSize 500    //GizwitsSend����������
#define GizwitsSendOldQSize 3        //MCU Ans��ʷ��¼��������
//------------------------------------------------------------------------------------------------//
typedef struct GizwitsActST //Gizwits����ָʾ��
{
    ulong GizwitsSysMs; //ϵͳʱ�� (ms)
	uchar WiFiConect:1;//��ǰ�Ѿ���ӦWiFi,Ans=0x02,���Կ�ʼAsk
    //++++++++++++++++++++++++++++++++++++++++++++++//
    //��ִ�������־&��Ϣ
    uchar NeedAns : 1;          //�ѷ���Ask,��ҪWiFiӦ��
    uchar NeedAns_RstCount : 3; //���·���Ask�Ĵ���
    ulong NeedAns_Ms;           //�ϴη���Ask��ʱ��
    uchar NeedRstMCU : 1; //׼������MCU
    ulong NeedRstMCU_Ms;  //׼������MCU��ʱ��
    uchar NeedReport : 1; //��Ҫ�ϱ��豸״̬��Ϣ
    ulong NeedReport_Ms;  //�ϴ��ϱ��豸״̬��ϵͳʱ��
    
	uchar NeedReadDS18B20:1;//0:���Կ�ʼת���¶�;1:�ȴ���ȡDS18B20�¶�����
	ulong NeedReadDS18B20_Ms;//�ϴ�DS18B20(��ʼת���¶�/��ȡ����)��ϵͳʱ��

    //++++++++++++++++++++++++++++++++++++++++++++++//
    //WiFiģ�鹤��״̬
    uchar OpenSoftAP : 1;      //SoftAPģʽ
    uchar OpenStation : 1;     //Stationģʽ
    uchar OpenOnBoarding : 1;  //����(OnBoarding)ģʽ
    uchar OpenBindMode : 1;    //������ģʽ
    uchar ConnectRoute : 1;    //�ɹ���������·����
    uchar ConnectM2M : 1;      //�ɹ�����M2M������
    uchar RouteRSSI : 3;       //ConnectRoute==1ʱ��Ч,��ǰ����������·�����ź�ǿ��(RSSI),0->7
    uchar AppOnline : 1;       //��App����
    uchar ProductTestMode : 1; //������Ʒ����ģʽ
    //++++++++++++++++++++++++++++++++++++++++++++++//
    //���Ask or Ansָ��ģʽ
    uchar GizwitsConfigMode : 3; //����WiFiģ�������ķ�ʽ
    //1:SoftAP��ʽ;2:AirLink��ʽ;4:ֱ��д��������Ϣģʽ;���÷�ʽ���Ϸ�ʱ,Ĭ�Ͻ���AirLink���÷�ʽ
    uchar GizwitsIllegalCode; //Ӧ��WiFiģ�����ݰ��Ƿ�,����ԭ��
                              //1У��ʹ���;2�����ʶ��;3��������;4�ļ����Ͳ�ƥ��;0��5~255����
} GizwitsActST;
extern xdata GizwitsActST GizwitsST;
//------------------------------------------------------------------------------------------------//

//GizwitsReceive ���ݰ�
extern pdata ushort GizwitsReceiveLen;                             //��ǰ���յ����ݰ�Lenֵ
extern pdata uchar GizwitsReceiveCmd;                              //��ǰ���յ����ݰ�Cmdֵ
extern pdata uchar GizwitsReceiveSn;                               //��ǰ���յ����ݰ�Snֵ
extern pdata ushort GizwitsReceiveFlags;                           //��ǰ���յ����ݰ�Flagsֵ
extern xdata uchar GizwitsReceiveBuffer[GizwitsReceiveBufferSize]; //��ǰ���յ����ݰ����ػ�����
extern pdata ushort GizwitsReceiveIdx;                             //��ǰ���յ����ݰ����ػ������±�
extern pdata uchar GizwitsReceiveSum;                              //��ǰ���յ����ݰ�Sumֵ

//GizwitsSend - ���ݰ�
extern xdata uchar GizwitsSendBuffer[GizwitsSendBufferSize]; //�������ݰ�������
extern pdata ushort GizwitsSendIdx;                          //�������ݰ��������±�

//GizwitsSendOld - GizwitsSend���ݰ���ʷ��¼,Ans���ݰ�GizwitsSendOldQSize��,ĩβ�̶�ΪAsk���ݰ�
extern xdata uchar GizwitsSendOldBuffer[GizwitsSendOldQSize + 1][GizwitsSendBufferSize]; //�������ݰ���������ʷ��¼
extern xdata ushort GizwitsSendOldIdx[GizwitsSendOldQSize + 1];                          //�������ݰ��������±�
extern xdata uchar GizwitsSendOldHead, GizwitsSendOldTail;                               //����ͷβ�±�

#ifdef GizwitsUseWiFiRealTime
typedef struct GizwitsTimeFromWiFi //��WiFiģ���ȡ����ʵʱ��
{                                  //��ǰʱ����
    ushort Year : 12;              //��,�㹻֧��0~4095
    uchar Month : 4;               //��
    uchar Day : 5;                 //��
    uchar Hour : 5;                //Сʱ
    uchar Minute : 6;              //����
    uchar Second : 6;              //��
    ulong NTP;                     //19700101��������-��ʱ��ʱ��
} GizwitsTimeFromWiFi;
extern xdata GizwitsTimeFromWiFi GizwitsRealTime;
#endif
#ifdef GizwitsUseWiFiProperty
typedef struct GizwitsPropertyFromWiFi //ʹ�ô�WiFi��ȡWiFi���豸��Ϣָ��
{
    uchar ModuleType;           //ģ������
    uchar SerialProtocolVer[8]; //ͨ�ô���Э��汾��
    uchar HardwareVer[8];       //Ӳ���汾��
    uchar SoftwareVer[8];       //����汾��
    uchar Mac[16];              //Mac��ַ
    uchar Ip[16];               //Ip��ַ
    uchar Property[8];          //�豸����
} GizwitsPropertyFromWiFi;
extern xdata GizwitsPropertyFromWiFi GizwitsWiFiProperty;
#endif

//------------------------------------------------------------------------------------------------//
#endif