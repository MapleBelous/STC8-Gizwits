#include "DS18B20.h"
//------------------------------------------------------------------------------------------------//
xdata DS18B20ActST DS18B20ST;
code ushort DS18B20ConvertTMaxTime[4] = {95, 190, 378, 753}; //�¶�ת�����ȴ�ʱ��(ms)
//------------------------------------------------------------------------------------------------//
static bool DS18B20Init(void);            //��ʼ������
static void DS18B20WriteByte(uchar Byte); //д��һ���ֽ�����
static uchar DS18B20ReadByte(void);       //��ȡһ���ֽ�����
//------------------------------------------------------------------------------------------------//
void DS18B20STInit(void) //��ʼ��DS18B20�ڴ�&��ȡ��ǰDS18B20�ֱ���
{
    uchar Temp, TH, TL;
    memset(&DS18B20ST, 0, sizeof(DS18B20ActST)); //��ʼ����Ļ����¶�ֵӦΪDS18B20MinT
    if (DS18B20Init() == EXIT_FAILURE)           //DS18B20��ʼ��ʧ��
    {
#if LOGRANK_UART1 >= 1
        printf("ERR:DS18B20Init fail when DS18B20STInit!\r\n");
#endif
        return; //������ȡDS18B20�ֱ���.����־��¼ERR��Ϣ
    }
    DS18B20WriteByte(DS18B20SkipROM);                   //д������ROM����
    DS18B20WriteByte(DS18B20ReadRAM);                   //д���ȡRAMָ��
    Temp = DS18B20ReadByte(), Temp = DS18B20ReadByte(); //�����¶�,��ʱ���ܳû���ȡ�¶�,����Чֵ
    TH = DS18B20ReadByte(), TL = DS18B20ReadByte();     //����TH/TL
    Temp = DS18B20ReadByte();                           //��ȡ���üĴ���
    DS18B20ST.ResolutionMode = (Temp >> 5) & 0x03;      //����DS18B20����ָʾ����
    DS18B20ST.TemperatureHighData1 = TemperatureH;      //ȷ����ǰMode�¸��±�����ֵ
    DS18B20ST.TemperatureHighData2 = DS18B20ST.TemperatureHighData1 - TemperatureT;
    DS18B20ST.TemperatureLowData1 = TemperatureL; //ȷ����ǰMode�µ��±�����ֵ
    DS18B20ST.TemperatureLowData2 = DS18B20ST.TemperatureLowData1 + TemperatureT;
    DS18B20ConvertTemperature();      //��ʼ��-��ʼת���¶�#���Է���ֵ
    switch (DS18B20ST.ResolutionMode) //��ӦMode��ʱ
    {
    case 0:
        delay_ms(100);
        break;
    case 1:
        delay_ms(200);
        break;
    case 2:
        delay_ms(385);
        break;
    case 3:
        delay_ms(765);
        break;
    }
    DS18B20GetTemperature(); //��ʼ��-��ȡ�¶�ֵ#���Է���ֵ
#if LOGRANK_UART1 >= 2
    printf("LOG#:DS18B20Init ok\r\n");
#endif
#if LOGRANK_UART1 >= 3
    printf("LOG:TH-%bu,TL-%bu,Mode-%bu,H1-%u,H2-%u,L1-%u,L2-%u\r\n",
           TH, TL, DS18B20ST.ResolutionMode,
           DS18B20ST.TemperatureHighData1, DS18B20ST.TemperatureHighData2,
           DS18B20ST.TemperatureLowData1, DS18B20ST.TemperatureLowData2);
#endif
}
bool DS18B20Set(char TH, char TL, uchar Mode) //���ò�����TH/TL/Mode
{
    if (DS18B20Init() == EXIT_FAILURE) //DS18B20��ʼ��ʧ��,���ش�����EXIT_FAILURE
        return EXIT_FAILURE;
    DS18B20WriteByte(DS18B20SkipROM);  //д������ROM����
    DS18B20WriteByte(DS18B20WriteRAM); //д��д��RAMָ��
    DS18B20WriteByte(TH);
    DS18B20WriteByte(TL);
    DS18B20WriteByte(Mode << 5);
    if (DS18B20Init() == EXIT_FAILURE) //DS18B20��ʼ��ʧ��,���ش�����EXIT_FAILURE
        return EXIT_FAILURE;
    DS18B20WriteByte(DS18B20SkipROM); //д������ROM����
    DS18B20WriteByte(DS18B20CopyRAM); //д�뱣��RAM��E2CROM
    return EXIT_SUCCESS;
}
bool DS18B20ConvertTemperature(void) //��ʼת���¶�ֵ
{
    if (DS18B20Init() == EXIT_FAILURE) //DS18B20��ʼ��ʧ��,���ش�����EXIT_FAILURE
        return EXIT_FAILURE;
    DS18B20WriteByte(DS18B20SkipROM);  //д������ROM����
    DS18B20WriteByte(DS18B20ConvertT); //д�뿪ʼת���¶�ָ��
    return EXIT_SUCCESS;
}
bool DS18B20GetTemperature(void) //��ȡ�¶�ֵ
{
    pdata uchar H, L;
    pdata ushort T, Base = DS18B20MinT;
    Base = -Base, Base <<= (1 + DS18B20ST.ResolutionMode); //Gizwit�����׼ֵ
    if (DS18B20Init() == EXIT_FAILURE)                     //DS18B20��ʼ��ʧ��,���ش�����DS18B20InitFail
        return EXIT_FAILURE;
    DS18B20WriteByte(DS18B20SkipROM);             //д������ROM����
    DS18B20WriteByte(DS18B20ReadRAM);             //д���ȡRAMָ��
    L = DS18B20ReadByte(), H = DS18B20ReadByte(); //��ȡ�¶ȼĴ���
    T = H, T <<= 8, T |= L;                       //�ϲ���ȡ�����¶�
    T >>= (3 - (DS18B20ST.ResolutionMode));       //����Ϊ��ǰ�ֱ���
    if ((T & 0x1000) && (T & 0x0800))             //�¶�ֵΪ����,����������λȷ��
    {
        T = ~T, ++T;  //ȡԭ��
        T = Base - T; //�Ի�׼ֵ���������޷��������¶�ֵ
    }
    else
        T += Base; //�Ի�׼ֵ���������޷��������¶�ֵ
    if (T != DS18B20ST.TemperatureData)
	{
		DS18B20ST.TemperatureData = T;   //�����¶�����
		GizwitsST.NeedReport = true; //����ʷ�¶�ֵ��ͬ,���������豸�ϱ�����
		if (DS18B20ST.TemperatureHigh == false && T >= (DS18B20ST.TemperatureHighData1))
			DS18B20ST.TemperatureHigh = true; //���±���
		if (DS18B20ST.TemperatureHigh == true && T <= (DS18B20ST.TemperatureHighData2))
			DS18B20ST.TemperatureHigh = false; //���±���ȡ��
		if (DS18B20ST.TemperatureLow == false && T <= (DS18B20ST.TemperatureLowData1))
			DS18B20ST.TemperatureLow = true; //���±���
		if (DS18B20ST.TemperatureLow == true && T >= (DS18B20ST.TemperatureLowData2))
			DS18B20ST.TemperatureLow = false; //���±���ȡ��
	}
    return EXIT_SUCCESS;
}
//------------------------------------------------------------------------------------------------//
static bool DS18B20Init(void) //��ʼ������
{
    pdata uchar i = 20;
    DS18B20_DATA = 1, delay_us(60), DS18B20_DATA = 0;
    delay_us(485), DS18B20_DATA = 1; //��ʱ�ȴ�����������
    do
    {
        delay_us(10);
    } while (DS18B20_DATA == 1 && (--i)); //�ȴ�DS18B20Ӧ��
    if (i == 0)
    {
#if LOGRANK_UART1 >= 1
        printf("ERR:DS18B20Init fail\r\n"); //��־��¼DS18B20��ʼ��δ�յ�Ӧ��
#endif
        return EXIT_FAILURE;
    }
    DS18B20_DATA = 1; //�����Ѿ���λ
    delay_us(130);    //�ȴ�DS18B20��λ,ʵ��150���Ͼ����ȶ�
    return EXIT_SUCCESS;
}
static void DS18B20WriteByte(uchar Byte) //д��һ���ֽ�����
{
    pdata uchar i;
    for (i = 0; i != 8; ++i, Byte >>= 1)
    {
        DS18B20_DATA = 0;
        delay_us(1);
        DS18B20_DATA = Byte & 0x01;
        delay_us(60);
        DS18B20_DATA = 1;
        delay_us(1);
    }
}
static uchar DS18B20ReadByte(void) //��ȡһ���ֽ�����
{
    pdata uchar i, re;
    DS18B20_DATA = 1;
    delay_us(10);
    for (i = 0, re = 0; i != 8; ++i)
    {
        re >>= 1;
        DS18B20_DATA = 0;
        delay_us(1);
        DS18B20_DATA = 1;
        delay_us(1);
        if (DS18B20_DATA == 1)
            re |= 0x80;
        delay_us(60);
    }
    return re;
}
//------------------------------------------------------------------------------------------------//