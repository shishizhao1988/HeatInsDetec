#ifndef AIBUS_FRAME_H_
#define AIBUS_FRAME_H_

#include <qglobal.h>
#include <ostream>
#include <QDebug>

#define ERR_OK                                  (0)		//执行成功
#define ERR_ERROR                               (1)		//执行错误
#define ERR_TIMEOUT                             (2)		//通讯超时
#define ERR_INVALIED_PARAM                      (3)		//非法参数

#define AIBUS_READ                              (0x52)
#define AIBUS_PARA                              (0x0C)
#define AIBUS_RCV_CNT                           (0x0A)
#define AIBUS_WRITE                             (0x43)
#define MDBUS_READ                              (0x03)


typedef struct tag_AIBUSSENDFRAME_T
{
    unsigned char cAddr1;
    unsigned char cAddr2;
    unsigned char cType;
    unsigned char cParam;
    unsigned char cParamL;
    unsigned char cParamH;
    unsigned char cCheckSumL;
    unsigned char cCheckSumH;
}AIBUSSENDFRAME_T;

typedef struct tag_AIBUSRCVFRAME_T
{
    unsigned char cPvL;
    unsigned char cPvH;
    unsigned char cSvL;
    unsigned char cSvH;
    unsigned char cMv;
    unsigned char cAlm;
    unsigned char cParaL;
    unsigned char cParaH;
    unsigned char cCheckSumL;
    unsigned char cCheckSumH;

}AIBUSRCVFRAME_T;

//---------------------
typedef struct tag_MODBUSSENDFRAM_T
{
    //    unsigned char mStart=0x1C;
    unsigned char cAddr;
    unsigned char funcCode;
    unsigned char HoldRAddressH;
    unsigned char HoldRAddressL;
    unsigned char HoldRNumbH;
    unsigned char HoldRNumbL;
    unsigned char CRCL;
    unsigned char CRCH;
    //    unsigned char mEnd=0x1C;

}MODBUSSENDFRAM_T;

#endif // APPFRAME_H
