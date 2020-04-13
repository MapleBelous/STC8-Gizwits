#include "GizwitsConfig.h"
//------------------------------------------------------------------------------------------------//

//Gizwits System Set for WiFiAsk, AnsCmd = 0x02
code uchar SerialProtocolVer[8] = {"00000004"};//ͨ�ô���Э��汾��
code uchar BusinessProtocolVer[8] = {"00000002"};//����Э��汾��
code uchar HardwareVer[8] = {"Ha200402"};//Ӳ���汾��
code uchar SoftwareVer[8] = {"Hs200407"};//����汾��
code uchar ProductKey[32] = {"c6491f9bc91b497c80fef21ad01db936"};
code ushort BindingStateSec = 30;//�ɰ�״̬ʧЧʱ��(s),0��ʾ��ʱ�ɰ�
code uchar DeviceProperties[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};//�豸����
code uchar ProductSecret[32] = {"21a08167d7d84d43aa525fc56170e7fd"};
code ushort DataLen = 40;//������������,���뾫׼����,=strlen(Data)
code uchar Data[] = "apName=ESP8266_SoftAPMode&apPwd=asdf1234";//��������

//ConfigMode4 WiFi Config for MCUAsk, AskCmd = 0x09
ConfigMode4ArrayMode uchar ConfigMode4Ssid[] = "BelousWIFI";
ConfigMode4ArrayMode uchar ConfigMode4Password[] = "12340987";
ConfigMode4ArrayMode uchar ConfigMode4Bssid[] = "";
ConfigMode4ArrayMode uchar ConfigMode4Tz[] = "";
ConfigMode4ArrayMode uchar ConfigMode4ServerName[] = "";

//------------------------------------------------------------------------------------------------//