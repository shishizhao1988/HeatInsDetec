#ifndef AIBUSPROTOCOL_H_
#define AIBUSPROTOCOL_H_
#include "win_qextserialport.h"
#include "aibus_Frame.h"
#include <QString>

class CAiBusProtocol
{
public:
    CAiBusProtocol();
    ~CAiBusProtocol();
	//打开串口
    bool comPortOpen(QString strCom);
	//打开状态
	bool isOpen();
	//参数读取接口
    int readParam(quint8 u8DevAddr, quint8 u8ParamId, AIBUSRCVFRAME_T &stRcvData);
    //写数据
    int writeParam(quint8 u8DevAddr, quint8 u8ParamId, int nValue, AIBUSRCVFRAME_T &stRcvData);
	//关闭串口
	void closeComPort();
	//获取当前串口号
	QString comPort();
	
private:
    //从串口读取数据
    qint64 ReadData(char *pcReadBuff, qint64 nReadLenth);
    //写数据到串口
    qint64 WriteData(const char *pcWriteBuff, qint64 nWriteLenth);
    bool SendFrame(const char *pcFrameBuff, qint64 nFrameSize, int nTimeOut);
    bool ReceiveFrame(char *pcFrameBuff, qint64 nFrameSize, int nTimeOut);
private:
    Win_QextSerialPort *m_MyComPort;
	QString strComPort;
};

#endif // AIBUSPROTOCOL_H_
