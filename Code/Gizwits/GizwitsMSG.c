#include "GizwitsMSG.h"
//------------------------------------------------------------------------------------------------//
static void GizwitsSetFifiControl(void); //��GizwitsReceiveBuffer�ж�ȡ��������,����ִ��
static void GizwitsGetDevState(void);    //��GizwitsReceiveBuffer��д���豸״̬��Ϣ

//����GizwitsHandleWiFiAsk���Ӻ���,Cmd=0x0E,���ڶ�ȡWiFiģ�鹤��״̬
static void GizwitsWiFiAsk0x0E(void);
//����GizwitsHandleWiFiAns���Ӻ���,Cmd=0x17��ʹ��WiFi��RealTime��Ϣʱ��ȡ����
static void GizwitsWiFiAns0x17(void);
//����GizwitsHandleWiFiAns���Ӻ���,Cmd=0x21��ʹ��WiFi���豸��Ϣʱ��ȡ����
static void GizwitsWiFiAns0x21(void);
//����GizwitsHandleMCUAsk���Ӻ���,Cmd=0x09��GizwitsConfigMode=0x04ʱд��SendBuffer��WiFi������Ϣ
static void GizwitsMCUAsk0x09(void);

//------------------------------------------------------------------------------------------------//
void GizwitsAct(void) //����ǰ����
{
    //Gizwitsϵͳ��ʱ��������
#if GizwitsNeedReportMs != 0
    if (GizwitsST.NeedReport == false && GizwitsST.GizwitsSysMs - GizwitsST.NeedReport_Ms >= GizwitsNeedReportMs)
        GizwitsST.NeedReport = true; //��Ҫ�ϱ��豸״̬
#endif
    //Gizwitsϵͳ��ִ������
    if (GizwitsST.NeedReport == true && GizwitsHandleMCUAsk(0x05) == 0) //�ϱ����,�����ϱ�ʱ��
        GizwitsST.NeedReport = false, GizwitsST.NeedReport_Ms = GizwitsST.GizwitsSysMs;
    if (GizwitsST.NeedAns == true) //�ȴ�Ans��
        GizwitsReAsk(1);
    if (GizwitsST.NeedRstMCU == true && (GizwitsST.GizwitsSysMs - GizwitsST.NeedRstMCU_Ms >= 600))
        MCURST(); //600ms��ʱ������MCU

        //Sensor��ʱ��������
#if DS18B20NeedReadMs != 0
    if (GizwitsST.NeedReadDS18B20 == false && GizwitsST.GizwitsSysMs - GizwitsST.NeedReadDS18B20_Ms >= DS18B20NeedReadMs && DS18B20ConvertTemperature() == EXIT_SUCCESS) //��Ҫ��ȡDS18B20�¶�ֵ,���¶�ת��ָ��ɹ�
        GizwitsST.NeedReadDS18B20 = true, GizwitsST.NeedReadDS18B20_Ms = GizwitsST.GizwitsSysMs;                                                                         //�ȴ���ȡ�¶�ֵ
#endif
    //Sensor��ִ������
    if (GizwitsST.NeedReadDS18B20 == true && GizwitsST.GizwitsSysMs - GizwitsST.NeedReadDS18B20_Ms >= DS18B20ConvertTMaxTime[DS18B20ST.ResolutionMode] && DS18B20GetTemperature() == EXIT_SUCCESS) //�ɹ�ִ�ж�ȡ�¶�ֵ
        GizwitsST.NeedReadDS18B20 = false, GizwitsST.NeedReadDS18B20_Ms = GizwitsST.GizwitsSysMs;
}
void GizwitsHandleWiFiAsk(uchar Cmd) //����Cmd: MCU��Ӧ������
{
#if LOGRANK_UART1 >= 2
    printf("LOG#:Handle WiFi Ask[%02bx]\r\n", Cmd); //��־��¼MCU����WiFi Ask,CmdΪMCU������Ans
#endif
    GizwitsSendIdx = 0;
    GizwitsSendBuffer[GizwitsSendIdx++] = Cmd;
    GizwitsSendBuffer[GizwitsSendIdx++] = GizwitsReceiveSn;
    switch (Cmd)
    {
    case 0x02: //WiFiģ�������豸��Ϣ
    {
        pdata ushort i;
        GizwitsSendBuffer[GizwitsSendIdx++] = 0x00; //Flags
        GizwitsSendBuffer[GizwitsSendIdx++] = 0x00;
        for (i = 0; i != 8; ++i)
            GizwitsSendBuffer[GizwitsSendIdx++] = SerialProtocolVer[i];
        for (i = 0; i != 8; ++i)
            GizwitsSendBuffer[GizwitsSendIdx++] = BusinessProtocolVer[i];
        for (i = 0; i != 8; ++i)
            GizwitsSendBuffer[GizwitsSendIdx++] = HardwareVer[i];
        for (i = 0; i != 8; ++i)
            GizwitsSendBuffer[GizwitsSendIdx++] = SoftwareVer[i];
        for (i = 0; i != 32; ++i)
            GizwitsSendBuffer[GizwitsSendIdx++] = ProductKey[i];
        GizwitsSendBuffer[GizwitsSendIdx++] = (BindingStateSec >> 8) & 0xFF;
        GizwitsSendBuffer[GizwitsSendIdx++] = (BindingStateSec)&0xFF;
        for (i = 0; i != 8; ++i)
            GizwitsSendBuffer[GizwitsSendIdx++] = DeviceProperties[i];
        for (i = 0; i != 32; ++i)
            GizwitsSendBuffer[GizwitsSendIdx++] = ProductSecret[i];
        GizwitsSendBuffer[GizwitsSendIdx++] = (DataLen >> 8) & 0xFF;
        GizwitsSendBuffer[GizwitsSendIdx++] = (DataLen)&0xFF;
        for (i = 0; i != DataLen; ++i)
            GizwitsSendBuffer[GizwitsSendIdx++] = Data[i];
		GizwitsST.WiFiConect=true;//����Ans=0x02���,���Կ�ʼAsk
    }
    break;
    case 0x04:                                      //WiFiģ������豸&��ȡ�豸�ĵ�ǰ״̬
        GizwitsSendBuffer[GizwitsSendIdx++] = 0x00; //Flags
        GizwitsSendBuffer[GizwitsSendIdx++] = 0x00;
        {
            uchar Action = GizwitsReceiveBuffer[0];
            if (Action == 0x01) //WiFiģ������豸
            {
                GizwitsSetFifiControl();     //��GizwitsReceiveBuffer�ж�ȡ��������,����ִ��
                GizwitsST.NeedReport = true; //��Ҫ�ϱ��豸״̬
            }
            else //WiFiģ���ȡ�豸�ĵ�ǰ״̬
            {
                GizwitsSendBuffer[GizwitsSendIdx++] = 0x03;
                GizwitsGetDevState();         //��GizwitsReceiveBuffer��д���豸״̬��Ϣ
                GizwitsST.NeedReport = false; //�൱��ִ�����ϱ�����
            }
        }
        break;
    case 0x08:                                      //����
        GizwitsSendBuffer[GizwitsSendIdx++] = 0x00; //Flags
        GizwitsSendBuffer[GizwitsSendIdx++] = 0x00;
        break;
    case 0x0E:                                      //WiFi����ģ�鹤��״̬
        GizwitsSendBuffer[GizwitsSendIdx++] = 0x00; //Flags
        GizwitsSendBuffer[GizwitsSendIdx++] = 0x00;
        GizwitsWiFiAsk0x0E(); //�����ӳ�����WiFiģ�鹤��״̬��Ϣ
        break;
    case 0x10:                                      //WiFiģ����������MCU
        GizwitsSendBuffer[GizwitsSendIdx++] = 0x00; //Flags
        GizwitsSendBuffer[GizwitsSendIdx++] = 0x00;
        GizwitsST.NeedRstMCU = true;
        break;
    case 0x12:                                      //Ӧ��WiFiģ�����ݰ��Ƿ�
        GizwitsSendBuffer[GizwitsSendIdx++] = 0x00; //Flags
        GizwitsSendBuffer[GizwitsSendIdx++] = 0x00;
        GizwitsSendBuffer[GizwitsSendIdx++] = GizwitsST.GizwitsIllegalCode;
        break;
    default:
#if LOGRANK_UART1 >= 1
        printf("ERR:GizwitsHandleWiFiAsk can't Ans this AnsCmd[%02bx]\r\n", Cmd);
#endif
        GizwitsSendIdx = 0; //��շ��ͻ�����
        return;
    }
    GizwitsSend(1); //���� Ans ���ݰ�
}
void GizwitsHandleWiFiAns(uchar Cmd) //����Cmd: MCU����������
{
    if (Cmd != GizwitsSendOldBuffer[GizwitsSendOldQSize][0] || GizwitsReceiveSn != GizwitsSendOldBuffer[GizwitsSendOldQSize][1])
    { //����δ֪�Ĵ���,��ǰAns��MCU�ȴ�Ӧ���Ask,Sn��Cmd����
#if LOGRANK_UART1 >= 1
        printf("ERR:GizwitsHandleWiFiAns found AskCmd[%02bx|%02bx]or Sn[%02bx|%02bx]illegal,Ans->0x12_0x02\r\n",
               Cmd, GizwitsSendOldBuffer[GizwitsSendOldQSize][0], GizwitsReceiveSn, GizwitsSendOldBuffer[GizwitsSendOldQSize][1]);
#endif
        GizwitsST.GizwitsIllegalCode = 0x02;
        GizwitsHandleWiFiAsk(0x12); //תΪGizwitsHandleWiFiAskӦ�����ݰ��Ƿ�
        return;
    }
#if LOGRANK_UART1 >= 2
    printf("LOG#:Handle WiFi Ans[%02bx]\r\n", Cmd); //��־��¼MCU����WiFi Ans,CmdΪMCU�ѷ���Ask
#endif
    GizwitsST.NeedAns = false; //ȡ���ȴ�WiFiӦ���־,�ɹ�ִ�ж�ӦAskCmd
    switch (Cmd)               //����WiFi Ans��Ϣ
    {
    case 0x05: //MCU��WiFiģ�������ϱ���ǰ״̬
    case 0x09: //MCU֪ͨWiFiģ���������ģʽ
    case 0x0B: //����WiFiģ��
    case 0x15: //֪ͨWiFiģ�����ɰ�ģʽ
    case 0x29: //MCU����ͨѶģ��
    case 0x13: //MCU����WiFiģ��������ģʽ
        break; //û����Ϣ��Ҫ����
    case 0x17: //��ȡ����ʱ��
#ifdef GizwitsUseWiFiRealTime
        GizwitsWiFiAns0x17();
#else
#if LOGRANK_UART1 >= 1
        printf("ERR:Unable GizwitsUseWiFiRealTime Def,ignore Ans datas\r\n");
#endif
#endif
        break;
    case 0x21: //��ȡͨѶģ�����Ϣ
#ifdef GizwitsUseWiFiProperty
        GizwitsWiFiAns0x21();
#else
#if LOGRANK_UART1 >= 1
        printf("ERR:Unable GizwitsUseWiFiProperty Def,ignore Ans datas\r\n");
#endif
#endif
        break;
    default: //δ֪����,���ش���,���ܷ����������SendBuffer+ReceiveBuffer����
#if LOGRANK_UART1 >= 1
        printf("ERR:MCU AskCmd for WiFi Ans is unknown,serious ERR!\r\n");
#endif
		break;
    }
}
uchar GizwitsHandleMCUAsk(uchar Cmd) //MCU����Cmd����Ask���ݰ�
{
    static pdata uchar AskSn = 0; //����Sn
    if (GizwitsST.NeedAns == 1||GizwitsST.WiFiConect==false)   //�ϴη��͵�Ask��δӦ���δ��WiFiȷ��,ȡ������
        return 1;
#if LOGRANK_UART1 >= 2
    printf("LOG#:Handle MCU Ask[%02bx]\r\n", Cmd); //��־��¼MCU���������Ask Cmd
#endif
    GizwitsSendIdx = 0;                          //��ʼ��SendBuffer�±�,׼������Ask
    GizwitsSendBuffer[GizwitsSendIdx++] = Cmd;   //д��Cmd
    GizwitsSendBuffer[GizwitsSendIdx++] = AskSn; //д��Sn
    switch (Cmd)
    {
    case 0x05:                                      //MCU��WiFiģ�������ϱ���ǰ״̬
        GizwitsSendBuffer[GizwitsSendIdx++] = 0x00; //Flags
        GizwitsSendBuffer[GizwitsSendIdx++] = 0x00;
        GizwitsSendBuffer[GizwitsSendIdx++] = 0x04; //action
        GizwitsGetDevState();
        break;
    case 0x09:                                      //MCU֪ͨWiFiģ���������ģʽ
        GizwitsSendBuffer[GizwitsSendIdx++] = 0x00; //Flags
        GizwitsSendBuffer[GizwitsSendIdx++] = 0x00;
        GizwitsSendBuffer[GizwitsSendIdx++] = GizwitsST.GizwitsConfigMode;
        if (GizwitsST.GizwitsConfigMode == 0x04) //MCUֱ��д��������Ϣģʽ
            GizwitsMCUAsk0x09();                 //ִ���Ӻ���д�뱾��WiFi������Ϣ
        break;
    case 0x0B:                                      //����WiFiģ��
    case 0x15:                                      //֪ͨWiFiģ�����ɰ�ģʽ
    case 0x29:                                      //MCU����ͨѶģ��
    case 0x13:                                      //MCU����WiFiģ��������ģʽ
    case 0x17:                                      //��ȡ����ʱ��
        GizwitsSendBuffer[GizwitsSendIdx++] = 0x00; //Flags
        GizwitsSendBuffer[GizwitsSendIdx++] = 0x00;
        break;
    case 0x21:                                      //��ȡͨѶģ�����Ϣ
        GizwitsSendBuffer[GizwitsSendIdx++] = 0x00; //Flags
        GizwitsSendBuffer[GizwitsSendIdx++] = 0x00;
        GizwitsSendBuffer[GizwitsSendIdx++] = 0x00; //type,�̶�Ϊ0x00,���ػ�����Ϣ
        break;
    default: //δ֪����,�޷�ִ��!
#if LOGRANK_UART1 >= 1
        printf("ERR:The AskCmd is unknown\r\n");
#endif
        GizwitsSendIdx = 0; //����SendBuffer
        return 2;
    }
    GizwitsSend(2);
    GizwitsST.NeedAns = true;                      //����ȴ�Ans״̬
    GizwitsST.NeedAns_RstCount = 0;                //��ʼ���ȴ�Ans,���ط�Ask����
    GizwitsST.NeedAns_Ms = GizwitsST.GizwitsSysMs; //��ʼ���ȴ�
    ++AskSn;                                       //����AskSnֵ
    return 0;
}
//����:���ݰ�Cmd��Flagsֵ
//1.��Cmd��WiFi->MCU(����)
//���Flags�Ƿ����Ҫ��,��������0x00|ReCmd;
//2.��Cmd��WiFi->MCU(Ӧ��)
//���Flags�Ƿ����Ҫ��,��������0xFF|ReCmd;
//3.��Cmd��MCU->WiFi(����|Ӧ��)
//���Flags�Ƿ����Ҫ��,��������0x0100;
//*.Flags����쳣,Re = 0xF000
//*.Cmd�޷�ʶ��,Re = 0xF100
ushort GizwitsAnalyseCmd(uchar Cmd, ushort Flags)
{
    pdata ushort Re = 0;
    switch (Cmd)
    {
    //����ͨѶЭ��
    case 0x01:               //Ask: WiFiģ�������豸��Ϣ����WiFi->MCU
        if (Flags != 0x0000) //������Flags�쳣
            Re = ReCmd_FlagsERR;
        else
            Re = 0x0002; //MCU->WiFi Ans Cmd
        break;
    case 0x02:               //Ans: WiFiģ�������豸��Ϣ����MCU->WiFi
        if (Flags != 0x0000) //������Flags�쳣
            Re = ReCmd_FlagsERR;
        else
            Re = ReCmd_MCU2WiFi; //Flags�������
        break;

    case 0x03:               //Ask: WiFiģ������豸&��ȡ�豸�ĵ�ǰ״̬����WiFi->MCU
        if (Flags != 0x0000) //������Flags�쳣
            Re = ReCmd_FlagsERR;
        else
            Re = 0x0004; //MCU->WiFi Ans Cmd
        break;
    case 0x04:               //Ans: WiFiģ������豸&��ȡ�豸�ĵ�ǰ״̬����MCU->WiFi
        if (Flags != 0x0000) //������Flags�쳣
            Re = ReCmd_FlagsERR;
        else
            Re = ReCmd_MCU2WiFi; //Flags�������
        break;

    case 0x05:               //Ask: MCU��WiFiģ�������ϱ���ǰ״̬����MCU->WiFi
        if (Flags != 0x0000) //������Flags�쳣
            Re = ReCmd_FlagsERR;
        else
            Re = ReCmd_MCU2WiFi; //Flags�������
        break;
    case 0x06:               //Ans: MCU��WiFiģ�������ϱ���ǰ״̬����WiFi->MCU
        if (Flags != 0x0000) //������Flags�쳣
            Re = ReCmd_FlagsERR;
        else
            Re = 0xFF05; //MCU->WiFi Ask Cmd
        break;

    case 0x07:               //Ask: ��������WiFi->MCU
        if (Flags != 0x0000) //������Flags�쳣
            Re = ReCmd_FlagsERR;
        else
            Re = 0x0008; //MCU->WiFi Ans Cmd
        break;
    case 0x08:               //Ans: ��������MCU->WiFi
        if (Flags != 0x0000) //������Flags�쳣
            Re = ReCmd_FlagsERR;
        else
            Re = ReCmd_MCU2WiFi; //Flags�������
        break;

    case 0x09:               //Ask: MCU֪ͨWiFiģ���������ģʽ����MCU->WiFi
        if (Flags != 0x0000) //������Flags�쳣
            Re = ReCmd_FlagsERR;
        else
            Re = ReCmd_MCU2WiFi; //Flags�������
        break;
    case 0x0A:               //Ans: MCU֪ͨWiFiģ���������ģʽ����WiFi->MCU
        if (Flags != 0x0000) //������Flags�쳣
            Re = ReCmd_FlagsERR;
        else
            Re = 0xFF09; //MCU->WiFi Ask Cmd
        break;

    case 0x0B:               //Ask: ����WiFiģ�顪��MCU->WiFi
        if (Flags != 0x0000) //������Flags�쳣
            Re = ReCmd_FlagsERR;
        else
            Re = ReCmd_MCU2WiFi; //Flags�������
        break;
    case 0x0C:               //Ans: ����WiFiģ�顪��WiFi->MCU
        if (Flags != 0x0000) //������Flags�쳣
            Re = ReCmd_FlagsERR;
        else
            Re = 0xFF0B; //MCU->WiFi Ask Cmd
        break;

    case 0x0D:               //Ask: WiFi����ģ�鹤��״̬����WiFi->MCU
        if (Flags != 0x0000) //������Flags�쳣
            Re = ReCmd_FlagsERR;
        else
            Re = 0x000E; //MCU->WiFi Ans Cmd
        break;
    case 0x0E:               //Ans: WiFi����ģ�鹤��״̬����MCU->WiFi
        if (Flags != 0x0000) //������Flags�쳣
            Re = ReCmd_FlagsERR;
        else
            Re = ReCmd_MCU2WiFi; //Flags�������
        break;

    case 0x11:               //ERR: �Ƿ����ݰ�֪ͨ����WiFi->MCU
    case 0x12:               //ERR: �Ƿ����ݰ�֪ͨ����MCU->WiFi
        if (Flags != 0x0000) //������Flags�쳣
            Re = ReCmd_FlagsERR;
        else
            Re = ReCmd_MSGERR; //MCU->WiFi Ans Cmd
        break;

    case 0x15:               //Ask: ֪ͨWiFiģ�����ɰ�ģʽ����MCU->WiFi
        if (Flags != 0x0000) //������Flags�쳣
            Re = ReCmd_FlagsERR;
        else
            Re = ReCmd_MCU2WiFi; //Flags�������
        break;
    case 0x16:               //Ans: ֪ͨWiFiģ�����ɰ�ģʽ����WiFi->MCU
        if (Flags != 0x0000) //������Flags�쳣
            Re = ReCmd_FlagsERR;
        else
            Re = 0xFF15; //MCU->WiFi Ask Cmd
        break;

    case 0x29:               //Ask: MCU����ͨѶģ�顪��MCU->WiFi
        if (Flags != 0x0000) //������Flags�쳣
            Re = ReCmd_FlagsERR;
        else
            Re = ReCmd_MCU2WiFi; //Flags�������
        break;
    case 0x2A:               //Ans: MCU����ͨѶģ�顪��WiFi->MCU
        if (Flags != 0x0000) //������Flags�쳣
            Re = ReCmd_FlagsERR;
        else
            Re = 0xFF29; //MCU->WiFi Ask Cmd
        break;

    //��ѡͨѶЭ��
    case 0x0F:               //Ask: WiFiģ����������MCU����WiFi->MCU
        if (Flags != 0x0000) //������Flags�쳣
            Re = ReCmd_FlagsERR;
        else
            Re = 0x0010; //MCU->WiFi Ans Cmd
        break;
    case 0x10:               //Ans: WiFiģ����������MCU����MCU->WiFi
        if (Flags != 0x0000) //������Flags�쳣
            Re = ReCmd_FlagsERR;
        else
            Re = ReCmd_MCU2WiFi; //Flags�������
        break;

    case 0x13:               //Ask: MCU����WiFiģ��������ģʽ����MCU->WiFi
        if (Flags != 0x0000) //������Flags�쳣
            Re = ReCmd_FlagsERR;
        else
            Re = ReCmd_MCU2WiFi; //Flags�������
        break;
    case 0x14:               //Ans: MCU����WiFiģ��������ģʽ����WiFi->MCU
        if (Flags != 0x0000) //������Flags�쳣
            Re = ReCmd_FlagsERR;
        else
            Re = 0xFF13; //MCU->WiFi Ask Cmd
        break;

    case 0x17:               //Ask: ��ȡ����ʱ�䡪��MCU->WiFi
        if (Flags != 0x0000) //������Flags�쳣
            Re = ReCmd_FlagsERR;
        else
            Re = ReCmd_MCU2WiFi; //Flags�������
        break;
    case 0x18:               //Ans: ��ȡ����ʱ�䡪��WiFi->MCU
        if (Flags != 0x0000) //������Flags�쳣
            Re = ReCmd_FlagsERR;
        else
            Re = 0xFF17; //MCU->WiFi Ask Cmd
        break;

    case 0x21:               //Ask: ��ȡͨѶģ�����Ϣ����MCU->WiFi
        if (Flags != 0x0000) //������Flags�쳣
            Re = ReCmd_FlagsERR;
        else
            Re = ReCmd_MCU2WiFi; //Flags�������
        break;
    case 0x22:               //Ans: ��ȡͨѶģ�����Ϣ����WiFi->MCU
        if (Flags != 0x0000) //������Flags�쳣
            Re = ReCmd_FlagsERR;
        else
            Re = 0xFF21; //MCU->WiFi Ask Cmd
        break;
        //���������ָ����

    default: //Cmdδ�Ǽ�,Cmd�쳣
        Re = ReCmd_CmdERR;
    }
    return Re;
}
static void GizwitsSetFifiControl(void) //��GizwitsReceiveBuffer�ж�ȡ��������,����ִ��
{
    pdata uchar MSGMask = GizwitsReceiveBuffer[1];
    pdata uchar MSGControl = GizwitsReceiveBuffer[2];
    if ((MSGMask & 0x01) != 0)
    {
        if ((MSGControl & 0x01) != 0)
            LED_RED = 0;
        else
            LED_RED = 1;
    }
    if ((MSGMask & 0x02) != 0)
    {
        if ((MSGControl & 0x02) != 0)
            LED_BLUE = 0;
        else
            LED_BLUE = 1;
    }
#if LOGRANK_UART1 >= 2
    printf("LOG#:WiFi Control ok[%02bx,%02bx]\r\n", MSGMask, MSGControl);
#endif
}
static void GizwitsGetDevState(void) //��GizwitsReceiveBuffer��д���豸״̬��Ϣ
{
    pdata uchar MSGSTATE = 0;
    if (LED_RED == 0)
        MSGSTATE |= 0x01;
    if (LED_BLUE == 0)
        MSGSTATE |= 0x02;
    GizwitsSendBuffer[GizwitsSendIdx++] = MSGSTATE;
#if LOGRANK_UART1 >= 3
    printf("LOG:Get DevState[%02bx,%u,", MSGSTATE, DS18B20ST.TemperatureData);
#endif
    GizwitsSendBuffer[GizwitsSendIdx++] = (DS18B20ST.TemperatureData) >> 8; //д�뻺����¶�ֵ
    GizwitsSendBuffer[GizwitsSendIdx++] = (DS18B20ST.TemperatureData) & 0xFF;
    MSGSTATE = DS18B20ST.TemperatureLow, MSGSTATE <<= 1, MSGSTATE |= DS18B20ST.TemperatureHigh;
    GizwitsSendBuffer[GizwitsSendIdx++] = MSGSTATE;
#if LOGRANK_UART1 >= 3
    printf("%02bx]\r\n", MSGSTATE);
#endif
}
//����GizwitsHandleWiFiAsk���Ӻ���,Cmd=0x0E,���ڶ�ȡWiFiģ�鹤��״̬
static void GizwitsWiFiAsk0x0E(void)
{
    if (GizwitsST.OpenSoftAP ^ (GizwitsReceiveBuffer[1] & 0x01)) //SoftAPģʽ�����仯
    {
        GizwitsST.OpenSoftAP = ~(GizwitsST.OpenSoftAP);
#if LOGRANK_UART1 >= 3
        printf("LOG:%s SoftAP Mode\r\n", GizwitsST.OpenSoftAP == true ? "Enter" : "Exit");
#endif
    }
    GizwitsReceiveBuffer[1] >>= 1;
    if (GizwitsST.OpenStation ^ (GizwitsReceiveBuffer[1] & 0x01)) //Stationģʽ�����仯
    {
        GizwitsST.OpenStation = ~(GizwitsST.OpenStation);
#if LOGRANK_UART1 >= 3
        printf("LOG:%s Station Mode\r\n", GizwitsST.OpenStation == true ? "Enter" : "Exit");
#endif
    }
    GizwitsReceiveBuffer[1] >>= 1;
    if (GizwitsST.OpenOnBoarding ^ (GizwitsReceiveBuffer[1] & 0x01)) //����(OnBoarding)ģʽ�����仯
    {
        GizwitsST.OpenOnBoarding = ~(GizwitsST.OpenOnBoarding);
#if LOGRANK_UART1 >= 3
        printf("LOG:%s OnBoarding Mode\r\n", GizwitsST.OpenOnBoarding ? "Enter" : "Exit");
        if (GizwitsST.OpenOnBoarding)
            printf("    %s Mode\r\n", GizwitsST.OpenSoftAP == true ? "SoftAP" : "AirLink");
#endif
    }
    GizwitsReceiveBuffer[1] >>= 1;
    if (GizwitsST.OpenBindMode ^ (GizwitsReceiveBuffer[1] & 0x01)) //��ģʽ�����仯
    {
        GizwitsST.OpenBindMode = ~(GizwitsST.OpenBindMode);
#if LOGRANK_UART1 >= 3
        printf("LOG:%s BindMode\r\n", GizwitsST.OpenBindMode == true ? "Enter" : "Exit");
#endif
    }
    GizwitsReceiveBuffer[1] >>= 1;
    if (GizwitsST.ConnectRoute ^ (GizwitsReceiveBuffer[1] & 0x01)) //������·�������ӷ����仯
    {
        GizwitsST.ConnectRoute = ~(GizwitsST.ConnectRoute);
#if LOGRANK_UART1 >= 3
        printf("LOG:WireLess Route %s\r\n", GizwitsST.ConnectRoute == true ? "Connect" : "Disconnect");
#endif
    }
    GizwitsReceiveBuffer[1] >>= 1;
    if (GizwitsST.ConnectM2M ^ (GizwitsReceiveBuffer[1] & 0x01)) //��M2M���������ӷ����仯
    {
        GizwitsST.ConnectM2M = ~(GizwitsST.ConnectM2M);
#if LOGRANK_UART1 >= 3
        printf("LOG:M2M server %s\r\n", GizwitsST.ConnectM2M == true ? "Connect" : "Disconnect");
#endif
    }
    GizwitsST.RouteRSSI = GizwitsReceiveBuffer[0] & 0x07; //��������·�����ź�ǿ��
#if LOGRANK_UART1 >= 3
    if (GizwitsST.ConnectRoute)
        printf("LOG:Route RSSI-%bu\r\n", GizwitsST.RouteRSSI);
#endif
    if (GizwitsST.AppOnline ^ (GizwitsReceiveBuffer[0] & 0x08) >> 3) //App����״̬�����仯
    {
        GizwitsST.AppOnline = ~(GizwitsST.AppOnline);
#if LOGRANK_UART1 >= 3
        printf("LOG:%s app online now\r\n", GizwitsST.AppOnline == true ? "Have" : "None");
#endif
    }
    if (GizwitsST.ProductTestMode ^ (GizwitsReceiveBuffer[0] & 0x10) >> 4) //��Ʒ����ģʽ�����仯
    {
        GizwitsST.ProductTestMode = ~(GizwitsST.ProductTestMode);
#if LOGRANK_UART1 >= 3
        printf("LOG:%s product test\r\n", GizwitsST.ProductTestMode == true ? "Enter" : "Exit");
#endif
    }
}
//����GizwitsHandleWiFiAns���Ӻ���,Cmd=0x17��ʹ��WiFi��RealTime��Ϣʱ��ȡ����
static void GizwitsWiFiAns0x17(void)
{
    GizwitsRealTime.Year = GizwitsReceiveBuffer[0];
    GizwitsRealTime.Year <<= 8;
    GizwitsRealTime.Year |= GizwitsReceiveBuffer[1];
    GizwitsRealTime.Month = GizwitsReceiveBuffer[2];
    GizwitsRealTime.Day = GizwitsReceiveBuffer[3];
    GizwitsRealTime.Hour = GizwitsReceiveBuffer[4];
    GizwitsRealTime.Minute = GizwitsReceiveBuffer[5];
    GizwitsRealTime.Second = GizwitsReceiveBuffer[6];
    GizwitsRealTime.NTP = GizwitsReceiveBuffer[7];
    GizwitsRealTime.NTP <<= 8;
    GizwitsRealTime.NTP |= GizwitsReceiveBuffer[8];
    GizwitsRealTime.NTP <<= 8;
    GizwitsRealTime.NTP |= GizwitsReceiveBuffer[9];
    GizwitsRealTime.NTP <<= 8;
    GizwitsRealTime.NTP |= GizwitsReceiveBuffer[10];
#if LOGRANK_UART1 >= 3
    printf("LOG:Get Real Time,%04u.%02bu.%02bu,%02bu:%02bu:%02bu,NTP:%lu\r\n",
           GizwitsRealTime.Year, GizwitsRealTime.Month, GizwitsRealTime.Day,
           GizwitsRealTime.Hour, GizwitsRealTime.Minute, GizwitsRealTime.Second, GizwitsRealTime.NTP);
#endif
}
//����GizwitsHandleWiFiAns���Ӻ���,Cmd=0x21��ʹ��WiFi���豸��Ϣʱ��ȡ����
static void GizwitsWiFiAns0x21(void)
{
    pdata uchar i, j = 0;
    GizwitsWiFiProperty.ModuleType = GizwitsReceiveBuffer[j++];
    for (i = 0; i != 8; ++i)
        GizwitsWiFiProperty.SerialProtocolVer[i] = GizwitsReceiveBuffer[j++];
    for (i = 0; i != 8; ++i)
        GizwitsWiFiProperty.HardwareVer[i] = GizwitsReceiveBuffer[j++];
    for (i = 0; i != 8; ++i)
        GizwitsWiFiProperty.SoftwareVer[i] = GizwitsReceiveBuffer[j++];
    for (i = 0; i == 0 || GizwitsReceiveBuffer[i - 1] != '\0'; ++i)
        GizwitsWiFiProperty.Mac[i] = GizwitsReceiveBuffer[j++];
    for (i = 0; i == 0 || GizwitsReceiveBuffer[i - 1] != '\0'; ++i)
        GizwitsWiFiProperty.Ip[i] = GizwitsReceiveBuffer[j++];
    for (i = 0; i != 8; ++i)
        GizwitsWiFiProperty.Property[i] = GizwitsReceiveBuffer[j++];
#if LOGRANK_UART1 >= 3
    printf("LOG:Get WiFi Property@\r\nModuleType:%02bu\r\n", GizwitsWiFiProperty.ModuleType);
    printf("SerialProtocolVer:%.8s\r\n", GizwitsWiFiProperty.SerialProtocolVer);
    printf("HardwareVer:%.8s\r\n", GizwitsWiFiProperty.HardwareVer);
    printf("SoftwareVer:%.8s\r\n", GizwitsWiFiProperty.SoftwareVer);
    printf("Mac:%s\r\n", GizwitsWiFiProperty.Mac);
    printf("Ip:%s\r\n", GizwitsWiFiProperty.Ip);
    printf("Property:");
    for (i = 0; i != 8; ++i)
        printf("%02bx", GizwitsWiFiProperty.Property[i]);
    printf("\r\n");
#endif
}
//����GizwitsHandleMCUAsk���Ӻ���,Cmd=0x09��GizwitsConfigMode=0x04ʱд��SendBuffer��WiFi������Ϣ
static void GizwitsMCUAsk0x09(void)
{
    pdata uchar i8, j8;
    pdata ulong i32, j32;
    j8 = GizwitsSendBuffer[GizwitsSendIdx++] = strlen(ConfigMode4Ssid);
    for (i8 = 0; i8 != j8; ++i8)
        GizwitsSendBuffer[GizwitsSendIdx++] = ConfigMode4Ssid[i8];
    j8 = GizwitsSendBuffer[GizwitsSendIdx++] = strlen(ConfigMode4Password);
    for (i8 = 0; i8 != j8; ++i8)
        GizwitsSendBuffer[GizwitsSendIdx++] = ConfigMode4Password[i8];
    j8 = GizwitsSendBuffer[GizwitsSendIdx++] = strlen(ConfigMode4Bssid);
    for (i8 = 0; i8 != j8; ++i8)
        GizwitsSendBuffer[GizwitsSendIdx++] = ConfigMode4Bssid[i8];
    j32 = GizwitsSendBuffer[GizwitsSendIdx++] = strlen(ConfigMode4Tz);
    for (i32 = 0; i32 != j32; ++i32)
        GizwitsSendBuffer[GizwitsSendIdx++] = ConfigMode4Tz[i32];
    j8 = GizwitsSendBuffer[GizwitsSendIdx++] = strlen(ConfigMode4ServerName);
    for (i8 = 0; i8 != j8; ++i8)
        GizwitsSendBuffer[GizwitsSendIdx++] = ConfigMode4ServerName[i8];
}
//------------------------------------------------------------------------------------------------//