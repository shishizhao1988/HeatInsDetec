#ifndef GETMDBDATA_H
#define GETMDBDATA_H

#include <QTimer>
#include <QObject>
#include <array>

#include "dataType.h"
#include "AIBUSProtocol/aibus_Frame.h"
#include "AIBUSProtocol/aibusprotocol.h"


class GetMdbDataThread : public QObject
{
    Q_OBJECT
public:
    explicit GetMdbDataThread(QObject *parent=nullptr);
    ~GetMdbDataThread();

    std::array<lbShowData,4> retData;
    QMutex mutex;
private:
public slots:
    void updateData();
    void acceptIoValue(int address,int value);

private:

    CAiBusProtocol* m_aibus;
    AIBUSRCVFRAME_T revdata1,revdata2,revdata3,revdata4;
    AIBUSRCVFRAME_T revdata5,revdata6,revdata7,revdata8,revdata9,revdata10,revdata11;

    int m_ioValue;
    int ioValue;
    int loseRetryNum;
    int isLose0,isLose1,isLose2,isLose3,isLose4,isLose5,isLose6,
    isLose7,isLose8,isLose9,isLose10,isLose11,isLose12;


signals:
    void resultReady(const std::array<lbShowData,4> rd);
    void loseConnectMdb();
};

#endif // GETMDBDATA_H
