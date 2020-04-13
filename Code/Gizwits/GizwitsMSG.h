#ifndef _GizwitsMSG_H_
#define _GizwitsMSG_H_
#include "GizwitsConfig.h"
#include "GizwitsHandle.h"
#include "MCUdef.h"
#include <string.h>
#include "DS18B20.h"
//------------------------------------------------------------------------------------------------//
ushort GizwitsAnalyseCmd(uchar, ushort); //�������ݰ�Cmd&���Flags

void GizwitsAct(void);                //����ǰ����
void GizwitsHandleWiFiAsk(uchar);     //������յ���Ask���ݰ�(����Ans���ݰ�/�����Ϣ�������������)
void GizwitsHandleWiFiAns(uchar);     //������յ���Ans���ݰ�(�����Ϣ�����ȡ��ϵͳ�ȴ�Ans״̬)
uchar GizwitsHandleMCUAsk(uchar Cmd); //���ɴ�����Cmd�����Ask���ݰ�
//------------------------------------------------------------------------------------------------//

//GizwitsAnalyseCmd DEF
#define ReCmd_FlagsERR 0xF000
#define ReCmd_CmdERR 0xF100
#define ReCmd_MSGERR 0xF200
#define ReCmd_MCU2WiFi 0x0100

//------------------------------------------------------------------------------------------------//
#endif