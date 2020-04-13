#include "GizwitsHandle.h"
//------------------------------------------------------------------------------------------------//
xdata GizwitsActST GizwitsST; //Gizwits����ָʾ��

//GizwitsReceive�����ڴ�
static pdata ushort GizwitsReceiveCount; //��ǰ���յ������ݰ�����
static pdata uchar GizwitsReceiveState;  //��ǰ�������ݰ�����
static pdata uchar GizwitsReceiveLast;   //��һ����������,���ڹ���0x55
//GizwitsReceive - ���ݰ�
pdata ushort GizwitsReceiveLen;                             //��ǰ���յ����ݰ�Lenֵ
pdata uchar GizwitsReceiveCmd;                              //��ǰ���յ����ݰ�Cmdֵ
pdata uchar GizwitsReceiveSn;                               //��ǰ���յ����ݰ�Snֵ
pdata ushort GizwitsReceiveFlags;                           //��ǰ���յ����ݰ�Flagsֵ
xdata uchar GizwitsReceiveBuffer[GizwitsReceiveBufferSize]; //��ǰ���յ����ݰ����ػ�����
pdata ushort GizwitsReceiveIdx;                             //��ǰ���յ����ݰ����س���
pdata uchar GizwitsReceiveSum;                              //��ǰ���յ����ݰ�Sumֵ

//GizwitsSend - ���ݰ�
xdata uchar GizwitsSendBuffer[GizwitsSendBufferSize]; //�������ݰ�������
pdata ushort GizwitsSendIdx;                          //�������ݰ��������±�

//GizwitsSendOld - GizwitsSend���ݰ���ʷ��¼,Ans���ݰ�GizwitsSendOldQSize��,ĩβ�̶�ΪAsk���ݰ�
xdata uchar GizwitsSendOldBuffer[GizwitsSendOldQSize + 1][GizwitsSendBufferSize]; //�������ݰ���������ʷ��¼
xdata ushort GizwitsSendOldIdx[GizwitsSendOldQSize + 1];                          //�������ݰ��������±�
xdata uchar GizwitsSendOldHead, GizwitsSendOldTail;                               //����ͷβ�±�

#ifdef GizwitsUseWiFiRealTime
xdata GizwitsTimeFromWiFi GizwitsRealTime; //��WiFiģ���ȡ����ʵʱ��
#endif
#ifdef GizwitsUseWiFiProperty
xdata GizwitsPropertyFromWiFi GizwitsWiFiProperty; //��WiFi��ȡWiFi���豸��Ϣ
#endif

//------------------------------------------------------------------------------------------------//
static void GizwitsReceive(void);     //��WiFi���ڻ�������ȡ���ݰ�
static void GizwitsReceiveInit(void); //��ʼ��/����Giawits���ڽ���
static uchar GizwitsSum(uchar);       //����У���
static uchar GizwitsSendOld(void);    //�����ѷ������ݰ���ʷ��¼,�ҵ������·���
//------------------------------------------------------------------------------------------------//

void GizwitsInit(void) //��ʼ��Gizwits�ڴ�
{
    GizwitsReceiveInit();
    memset(&GizwitsST, 0, sizeof(GizwitsST));
    GizwitsSendIdx = 0;
    GizwitsSendOldHead = GizwitsSendOldTail = 0;
    GizwitsST.GizwitsIllegalCode = 0;
#if LOGRANK_UART1 >= 2
        printf("LOG#:GizwitsInit ok\r\n");
#endif
}
void GizwitsMainLoop(void) //Gizwits��ѭ������
{
    GizwitsReceive();
    GizwitsAct();
}
void GizwitsSend(uchar Mode) //����GizwitsSendBuffer�������ڵ����ݰ�&��¼�ѷ������ݰ�
{
    pdata ushort i;
    ++GizwitsSendIdx;                                      //����У��ͳ���1
    GizwitsSendBuffer[GizwitsSendIdx - 1] = GizwitsSum(1); //����У���
#if LOGRANK_UART1 >= 2
    //��־��¼MCU�����������ݰ�
    printf("LOG#:GizwitsSend   :%04x %02bx %02bx", GizwitsSendIdx, GizwitsSendBuffer[0], GizwitsSendBuffer[1]); //Len,Cmd
    i = GizwitsSendBuffer[2], i <<= 8, i |= GizwitsSendBuffer[3];
    printf(" %04x", i); //Flags
    for (i = 4; i != GizwitsSendIdx - 1; ++i)
        printf(" %02bx", GizwitsSendBuffer[i]); //����
    printf(" %02bx\r\n", GizwitsSendBuffer[GizwitsSendIdx - 1]);
#endif
    uart4_send8(0xFF), uart4_send8(0xFF);                                          //���͹̶���ͷ
    uart4_send8((GizwitsSendIdx >> 8) & 0xFF), uart4_send8(GizwitsSendIdx & 0xFF); //�������ݰ�����
    for (i = 0; i != GizwitsSendIdx; ++i)
        uart4_send8(GizwitsSendBuffer[i]); //�������ݰ�datas
    //�������ʷ��¼����
    if (Mode == 1) //Ans ��ʷ��¼
    {
        if (GizwitsSendOldHead == GizwitsSendOldTail)
        {
            if (GizwitsSendOldHead + 1 == GizwitsSendOldQSize)
                GizwitsSendOldHead = 0;
            else
                ++GizwitsSendOldHead;
        }
        GizwitsSendOldIdx[GizwitsSendOldTail] = GizwitsSendIdx;
        for (i = 0; i != GizwitsSendIdx; ++i)
            GizwitsSendOldBuffer[GizwitsSendOldTail][i] = GizwitsSendBuffer[i];
        if (GizwitsSendOldTail + 1 == GizwitsSendOldQSize)
            GizwitsSendOldTail = 0;
        else
            ++GizwitsSendOldTail;
    }
    if (Mode == 2) //Ask ��ʷ��¼
    {
        GizwitsSendOldIdx[GizwitsSendOldQSize] = GizwitsSendIdx;
        for (i = 0; i != GizwitsSendIdx; ++i)
            GizwitsSendOldBuffer[GizwitsSendOldQSize][i] = GizwitsSendBuffer[i];
    }
}
static void GizwitsReceive(void) //��WiFi���ڻ�������ȡ���ݰ�
{
    pdata ushort Cidx1 = uart4_idx1, Cidx2 = uart4_idx2;
    //�������ڻ�������ǰ�ɶ�ȡ����
    while (Cidx1 != Cidx2)
    {
        ++GizwitsReceiveCount; //�յ������ݰ�����+1
        if (GizwitsReceiveCount >= 4 &&
            GizwitsReceiveLast == 0xFF && uart4_buffer[Cidx1] == 0x55) //�ǹ̶���ͷ�׶ι���0x55����
            goto WhileEnd;
        if (GizwitsReceiveCount >= 3)
            GizwitsReceiveLast = uart4_buffer[Cidx1]; //������ʷ����
        switch (GizwitsReceiveState)
        {
        case 0: //�̶���ͷ���ս׶�
            if (uart4_buffer[Cidx1] == 0xFF)
            {
                if (GizwitsReceiveCount == 2)
                    ++GizwitsReceiveState;
            }
            else //�����쳣��ͷ����,����Giawits���ڽ���
            {
#if LOGRANK_UART1 >= 1
                printf("ERR:GizwitsReceive can't find 0xFF\r\n");
#endif
                GizwitsReceiveInit();
            }
            break;
        case 1:
            GizwitsReceiveLen <<= 8;
            GizwitsReceiveLen |= uart4_buffer[Cidx1]; //��ȡ���ݰ�Lenֵ
            if (GizwitsReceiveCount == 4)
            {
                ++GizwitsReceiveState;
                if (GizwitsReceiveLen - 5 > GizwitsReceiveBufferSize) //���س��Ƚ��������ݰ����ػ��������
                {
#if LOGRANK_UART1 >= 1
                    printf("ERR:GizwitsReceive Len, but it will cause ReceiveBuffer overflow\r\n");
#endif
                    GizwitsReceiveInit(); //����Giawits���ڽ���
                }
            }
            break;
        case 2:
            GizwitsReceiveCmd = uart4_buffer[Cidx1]; //��ȡ���ݰ�Cmdֵ
            ++GizwitsReceiveState;
            break;
        case 3:
            GizwitsReceiveSn = uart4_buffer[Cidx1]; ////��ȡ���ݰ�Snֵ
            ++GizwitsReceiveState;
            break;
        case 4:
            GizwitsReceiveFlags <<= 8;
            GizwitsReceiveFlags |= uart4_buffer[Cidx1]; //��ȡ���ݰ�Flagsֵ
            if (GizwitsReceiveCount == 8)
                if (GizwitsReceiveLen - 5 > 0) //�ж��Ƿ��и���
                    ++GizwitsReceiveState;
                else
                    GizwitsReceiveState += 2;
            break;
        case 5:
            GizwitsReceiveBuffer[GizwitsReceiveIdx++] = uart4_buffer[Cidx1]; //��ȡ���ݰ�
            if (GizwitsReceiveCount - 3 == GizwitsReceiveLen)                //���ݰ�ʣ�೤��Ϊ1,��������ȡ�׶�
                ++GizwitsReceiveState;
            break;
        case 6:
            GizwitsReceiveSum = uart4_buffer[Cidx1]; ////��ȡ���ݰ�Sumֵ
#if LOGRANK_UART1 >= 1
            if (GizwitsReceiveLen + 4 != GizwitsReceiveCount) //�������ݰ���ȡ���Ȳ���,δ֪ԭ��
                printf("ERR:GizwitsReceive receive one packet, but len invalid\r\n");
            else
#endif
                if (GizwitsReceiveSum == GizwitsSum(0)) //У�����ȷ,���յ�һ�����ݰ�
            {
                ushort ReCmdFlags = GizwitsAnalyseCmd(GizwitsReceiveCmd, GizwitsReceiveFlags); //�������ݰ�Cmd�����Flags
                uchar ReCmd = ReCmdFlags & 0xFF, ReFlags = (ReCmdFlags >> 8) & 0xFF;
#if LOGRANK_UART1 >= 2
                //��־�㱨MCU���յ���ʽ��У�����ȷ�����ݰ�
                ushort i;
                printf("LOG#:GizwitsReceive:%04x %02bx %02bx %04x", GizwitsReceiveLen,
                       GizwitsReceiveCmd, GizwitsReceiveSn, GizwitsReceiveFlags); //Len,Cmd,Sn,Flags
                for (i = 0; i != GizwitsReceiveIdx; ++i)
                    printf(" %02bx", GizwitsReceiveBuffer[i]); //����
                printf(" %02bx\r\n", GizwitsReceiveSum);       //Sum
#endif
                if (ReCmd == 0x00)
                {
                    if (ReCmdFlags == ReCmd_FlagsERR) //���ݰ�Flags�쳣
                    {
#if LOGRANK_UART1 >= 1
                        printf("ERR:Packet Flags error,Ans->0x12_0x03\r\n");
#endif
                        GizwitsST.GizwitsIllegalCode = 0x03;
                        GizwitsHandleWiFiAsk(0x12);
                    }
                    else if (ReCmdFlags == ReCmd_CmdERR) //���ݰ�Cmd�޷�ʶ��
                    {
#if LOGRANK_UART1 >= 1
                        printf("ERR:Packet Cmd invalid or error,Ans->0x12_0x02\r\n");
#endif
                        GizwitsST.GizwitsIllegalCode = 0x02;
                        GizwitsHandleWiFiAsk(0x12);
                    }
                    else if (ReCmdFlags == ReCmd_MSGERR) //WiFi��Ӧ��Ӧ���쳣,��Ҫ���·���
                    {
#if LOGRANK_UART1 >= 2
                        printf("LOG#:Ask one packet is illegal,MCU will sent again\r\n");
#endif
                        if (GizwitsSendOld() != 0)
                        {
#if LOGRANK_UART1 >= 1
                            printf("ERR: GizwitsSendOld Fail,Sn is not found,can't sent again\r\n");
#endif
                        }
                    }
                    else
                    {
#if LOGRANK_UART1 >= 1
                        printf("ERR:Packet unknown error,Ans->0x12_0x03\r\n");
#endif
                        GizwitsST.GizwitsIllegalCode = 0x03;
                        GizwitsHandleWiFiAsk(0x12);
                    }
                }
                else
                {
                    if (ReFlags == 0x00) //���ݰ��� Ask
                        GizwitsHandleWiFiAsk(ReCmd);
                    else //���ݰ��� Ans
                        GizwitsHandleWiFiAns(ReCmd);
                }
            }
            else //У��ʹ���
            {
#if LOGRANK_UART1 >= 1
                printf("ERR: Packet Sum check fail,Ans->0x12_0x01\r\n");
#endif
                GizwitsST.GizwitsIllegalCode = 0x01;
                GizwitsHandleWiFiAsk(0x12);
            }
            GizwitsReceiveInit(); //����Giawits���ڽ���
            break;
        }
    WhileEnd:
        ++Cidx1; //��ȡ����������1�ֽ�,�ͷ�1�ֽڴ��ڻ������ռ�
        if (Cidx1 == uart4_buffer_size)
            Cidx1 = 0, uart4_idx1 = 0;
        uart4_idx1 = Cidx1;
    }
}
static void GizwitsReceiveInit(void) //��ʼ��/����Giawits���ڽ���
{
    GizwitsReceiveCount = 0;
    GizwitsReceiveState = 0;
    GizwitsReceiveLast = 0;
    GizwitsReceiveLen = 0;
    GizwitsReceiveCmd = 0;
    GizwitsReceiveSn = 0;
    GizwitsReceiveFlags = 0;
    GizwitsReceiveIdx = 0;
    GizwitsReceiveSum = 0;
}
static uchar GizwitsSendOld(void) //�����ѷ������ݰ���ʷ��¼,�ҵ������·���
{
    //Ask�ȴ�Ans���Ҽ��Sn��ȴ�Ans��Ask�Ƿ�һ��
    if (GizwitsST.NeedAns = true && GizwitsReceiveSn == GizwitsSendOldBuffer[GizwitsSendOldQSize][1])
    {
        GizwitsReAsk(2); //�������·���Ask
        return 0;
    }
    else //����Ask,��ѯAns�Ƿ��ж�ӦSn
    {
        pdata uchar idx = GizwitsSendOldHead;
        while (idx != GizwitsSendOldTail)
        {
            if (GizwitsReceiveSn == GizwitsSendOldBuffer[idx][1]) //�ɹ���Ans���ҵ���ӦSn
            {
                pdata ushort i;
                uart4_send8(0xFF), uart4_send8(0xFF); //���͹̶���ͷ
                uart4_send8((GizwitsSendOldIdx[idx] >> 8) & 0xFF);
                uart4_send8(GizwitsSendOldIdx[idx] & 0xFF); //�������ݰ�����
                for (i = 0; i != GizwitsSendOldIdx[idx]; ++i)
                    uart4_send8(GizwitsSendOldBuffer[idx][i]); //�������ݰ�datas
#if LOGRANK_UART1 >= 2
                printf("LOG#:GizwitsSendOld Ans ok\r\n");
#endif
                return 0;
            }
            if (idx == GizwitsSendOldQSize)
                idx = 0;
            else
                ++idx;
        }
    }
    return 1; //��ʷ��¼��δ�ҵ���ӦSn,���·���ʧ��
}
void GizwitsReAsk(uchar Mode) //��ʱ�ȴ�WiFiģ��Ans&���·���MCU��Ask���ݰ�
{
    pdata ushort i;
    if (GizwitsST.NeedAns_RstCount == 3) //�ط�Ask���λ�δ�õ�Ans,�����ȴ�Ans���ط�
    {
        GizwitsST.NeedAns = 0;
        GizwitsST.NeedAns_RstCount = 0;
        return;
    }
    if (Mode == 1) //����������ģʽ,��ʱ�ȴ�
    {
        if (GizwitsST.GizwitsSysMs - GizwitsST.NeedAns_Ms >= 200) //200msδ�յ�Ans,�ط�Ask
            ++GizwitsST.NeedAns_RstCount;                  //��¼һ���ط�Ask����
        else
            return; //�����ȴ�Ans
    }
    if (Mode == 2) //WiFi�����ط�Askģʽ,�������Ͳ����õȴ�ʱ��&����
        GizwitsST.NeedAns_RstCount = 0;
    uart4_send8(0xFF), uart4_send8(0xFF); //���͹̶���ͷ
    uart4_send8((GizwitsSendOldIdx[GizwitsSendOldQSize] >> 8) & 0xFF);
    uart4_send8(GizwitsSendOldIdx[GizwitsSendOldQSize] & 0xFF); //�������ݰ�����
    for (i = 0; i != GizwitsSendOldIdx[GizwitsSendOldQSize]; ++i)
        uart4_send8(GizwitsSendOldBuffer[GizwitsSendOldQSize][i]); //�������ݰ�datas
    GizwitsST.NeedAns_Ms = GizwitsST.GizwitsSysMs;                               //��¼���η���Ask��ʱ��
#if LOGRANK_UART1 >= 2
    printf("LOG#:GizwitsReAsk Cnt-%bu\r\n", GizwitsST.NeedAns_RstCount); //��־��¼���·���Ask�����ط�����
#endif
}
static uchar GizwitsSum(uchar Mode) //����У���
{
    pdata uchar Sum = 0;
    pdata ushort i;
    switch (Mode)
    {
    case 0: //GizwitsReceiveģʽ
        Sum += (GizwitsReceiveLen >> 8) & 0xFF;
        Sum += (GizwitsReceiveLen)&0xFF;
        Sum += GizwitsReceiveCmd;
        Sum += GizwitsReceiveSn;
        Sum += (GizwitsReceiveFlags >> 8) & 0xFF;
        Sum += (GizwitsReceiveFlags)&0xFF;
        for (i = 0; i != GizwitsReceiveIdx; ++i)
            Sum += GizwitsReceiveBuffer[i];
        break;
    case 1: //GizwitsSendģʽ
        Sum += (GizwitsSendIdx >> 8) & 0xFF;
        Sum += (GizwitsSendIdx)&0xFF;
        for (i = 0; i < GizwitsSendIdx - 1; ++i)
            Sum += GizwitsSendBuffer[i];
        break;
    }
    return Sum;
}
//------------------------------------------------------------------------------------------------//