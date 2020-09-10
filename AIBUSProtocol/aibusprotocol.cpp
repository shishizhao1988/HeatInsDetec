#include <QTime>
#include "aibusprotocol.h"

CAiBusProtocol::CAiBusProtocol()
{
    this->m_MyComPort = nullptr;
}

CAiBusProtocol::~CAiBusProtocol()
{
	closeComPort();
    if(m_MyComPort != nullptr)
    {
        delete m_MyComPort;
        m_MyComPort = nullptr;
    }
}

bool CAiBusProtocol::comPortOpen(QString strCom)
{
    if(m_MyComPort != nullptr)
    {
        m_MyComPort->close();
        delete m_MyComPort;
        m_MyComPort = nullptr;
    }

    m_MyComPort = new Win_QextSerialPort(strCom,QextSerialBase::Polling);
    if( m_MyComPort->open(QIODevice::ReadWrite))
    {
        m_MyComPort->setBaudRate(BAUD9600);
        m_MyComPort->setDataBits(DATA_8);
        m_MyComPort->setStopBits(STOP_1);
        m_MyComPort->setParity(PAR_NONE);
        m_MyComPort->setFlowControl(FLOW_OFF);
        m_MyComPort->setTimeout(30);
		strComPort = strCom;
        return true;
    }
    else
    {
        delete  m_MyComPort;
        m_MyComPort = nullptr;
    }
    return false;
}

qint64 CAiBusProtocol::ReadData(char *pcReadBuff, qint64 nReadLenth)
{
    qint64 nTempReadLenth = 0;

    if((m_MyComPort != nullptr) && (m_MyComPort->isOpen()))
    {
        nTempReadLenth = m_MyComPort->read(pcReadBuff, nReadLenth);
    }
    return nTempReadLenth;
}


qint64 CAiBusProtocol::WriteData(const char *pcWriteBuff, qint64 nWriteLenth)
{
    qint64 nTempWriteLenth = 0;

    if((m_MyComPort != nullptr) && (m_MyComPort->isOpen()))
    {
        nTempWriteLenth = m_MyComPort->write(pcWriteBuff, nWriteLenth);
    }

    return nTempWriteLenth;
}


bool CAiBusProtocol::SendFrame(const char *pcFrameBuff, qint64 nFrameSize, int nTimeOut)
{

    QTime time;

    time.start();

    while((time.elapsed() < nTimeOut) && (nFrameSize > 0))
    {
        qint64 nWriteBytes = WriteData(pcFrameBuff, nFrameSize);
        pcFrameBuff += nWriteBytes;
        nFrameSize -= nWriteBytes;
    }

    return (nFrameSize==0);
}


bool CAiBusProtocol::ReceiveFrame(char *pcFrameBuff, qint64 nFrameSize, int nTimeOut)
{

    QTime Time;
    Time.start();
    qint64 nReadCurCount = 0;

    while((Time.elapsed()< nTimeOut) && (nReadCurCount < nFrameSize))
    {
        qint64 nReadBytes = ReadData(pcFrameBuff, nFrameSize);
        pcFrameBuff += nReadBytes;
        nReadCurCount += nReadBytes;
    }
    return(nReadCurCount == nFrameSize);
}



void CAiBusProtocol::closeComPort()
{
    if((m_MyComPort != nullptr) && (m_MyComPort->isOpen()))
    {
        m_MyComPort->close();
    }
}

bool CAiBusProtocol::isOpen()
{
    if((m_MyComPort != nullptr) && (m_MyComPort->isOpen()))
    {
        return true;
    }

    return false;
}

int CAiBusProtocol::readParam(quint8 u8DevAddr, quint8 u8ParamId, AIBUSRCVFRAME_T &stRcvData)
{
    AIBUSSENDFRAME_T stSendframe;
    int nRetCode = ERR_ERROR;
    stSendframe.cAddr1 = 0x80 + u8DevAddr;
    stSendframe.cAddr2 = 0x80 + u8DevAddr;
    stSendframe.cType = AIBUS_READ;
    stSendframe.cParam = u8ParamId;
    stSendframe.cParamL = 0x00;
    stSendframe.cParamH = 0x00;
    int nCheckSum = (u8ParamId * 256 + AIBUS_READ + u8DevAddr) & 0xffff;
    stSendframe.cCheckSumL = nCheckSum & 0xff;
    stSendframe.cCheckSumH = (nCheckSum >> 8) & 0xff;

    if(SendFrame((const char *)&stSendframe, sizeof(stSendframe), 20))
    {
        if(ReceiveFrame((char *)&stRcvData, sizeof(stRcvData), 130))
        {
            nRetCode = ERR_OK;
        }
  
    }

    return nRetCode;
}

int CAiBusProtocol::writeParam(quint8 u8DevAddr, quint8 u8ParamId, int nValue, AIBUSRCVFRAME_T &stRcvData)
{
    AIBUSSENDFRAME_T stSendframe;
    int nRetCode = ERR_ERROR;
    stSendframe.cAddr1 = 0x80 + u8DevAddr;
    stSendframe.cAddr2 = 0x80 + u8DevAddr;
    stSendframe.cType = AIBUS_WRITE;
    stSendframe.cParam = u8ParamId;
    stSendframe.cParamL = nValue & 0xff;
    stSendframe.cParamH = (nValue>>8)&0xff;
    int nCheckSum = (u8ParamId * 256 + AIBUS_WRITE + u8DevAddr + nValue) & 0xffff;
    stSendframe.cCheckSumL = nCheckSum & 0xff;
    stSendframe.cCheckSumH = (nCheckSum >> 8) & 0xff;

    if(SendFrame((const char *)&stSendframe, sizeof(stSendframe), 20))
    {
        if(ReceiveFrame((char *)&stRcvData, sizeof(stRcvData), 130))
        {
            nRetCode = ERR_OK;
        }

    }

    return nRetCode;
}

QString CAiBusProtocol::comPort()
{
	return strComPort;
}



