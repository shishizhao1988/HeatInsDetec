#include "getmdbdata.h"
#include <QMessageBox>
#include <QDebug>



GetMdbDataThread::GetMdbDataThread(QObject *parent)
    :QObject(parent),
      m_aibus(new CAiBusProtocol()),
      loseRetryNum(0)
{    
    m_aibus->comPortOpen("com7");
    isLose9=0;
    isLose10=0;
    isLose11=0;
    isLose12=0;

}

GetMdbDataThread::~GetMdbDataThread()
{
    if(m_aibus->isOpen()){
        if(!m_aibus->writeParam(1,16,0,revdata1)){
            m_aibus->closeComPort();
        }
    }

    //    closeComPort();

    delete m_aibus;
}


void GetMdbDataThread::updateData()
{
    QMutexLocker locker(&mutex);

    if(!m_aibus->isOpen()){
        retData.at(0).isError=true;
        retData.at(1).isError=true;
        retData.at(2).isError=true;
        retData.at(3).isError=true;
        loseRetryNum++;
        if(loseRetryNum==13){
            qDebug()<<"reopen com port-->OPEN";
            m_aibus->comPortOpen("com7");
        }else if(loseRetryNum==15){
            loseRetryNum=0;
            qDebug()<<"SEND LOSE";
            emit loseConnectMdb();
        }
    }else{
        retData.at(0).isError=false;
        retData.at(1).isError=false;
        retData.at(2).isError=false;
        retData.at(3).isError=false;

        isLose0=m_aibus->readParam(1,16,revdata1);
        if(!isLose0){
            ioValue=revdata1.cParaL;
            retData.at(3).isOnWork=(ioValue&0x40)>0?true:false;
            retData.at(2).isOnWork=(ioValue&0x10)>0?true:false;
            retData.at(1).isOnWork=(ioValue&0x4)>0?true:false;
            retData.at(0).isOnWork=(ioValue&0x1)>0?true:false;
        }


        isLose1= m_aibus->readParam(10,0,revdata2);
        if(!isLose1){
            retData[0].innerTemp=(revdata2.cPvH*0x100+revdata2.cPvL)/10.0;
            retData[0].innerSetTemp=(revdata2.cSvH*0x100+revdata2.cSvL)/10.0;
        }

        isLose2= m_aibus->readParam(6,0,revdata3);
        if(!isLose2){
            retData[0].outterTemp=(revdata3.cPvH*0x100+revdata3.cPvL)/10.0;
        }

        isLose3= m_aibus->readParam(11,0,revdata4);
        if(!isLose3){
            retData[1].innerTemp=(revdata4.cPvH*0x100+revdata4.cPvL)/10.0;
            retData[1].innerSetTemp=(revdata4.cSvH*0x100+revdata4.cSvL)/10.0;
        }

        isLose4= m_aibus->readParam(7,0,revdata5);
        if(!isLose4){
            retData[1].outterTemp=(revdata5.cPvH*0x100+revdata5.cPvL)/10.0;

        }

        isLose5= m_aibus->readParam(12,0,revdata6);
        if(!isLose5){
            retData[2].innerTemp=(revdata6.cPvH*0x100+revdata6.cPvL)/10.0;
            retData[2].innerSetTemp=(revdata6.cSvH*0x100+revdata6.cSvL)/10.0;
        }
        isLose6= m_aibus->readParam(8,0,revdata7);
        if(!isLose6){
            retData[2].outterTemp=(revdata7.cPvH*0x100+revdata7.cPvL)/10.0;
        }

        isLose7= m_aibus->readParam(13,0,revdata8);
        if(!isLose7){
            retData[3].innerTemp=(revdata8.cPvH*0x100+revdata8.cPvL)/10.0;
            retData[3].innerSetTemp=(revdata8.cSvH*0x100+revdata8.cSvL)/10.0;
        }
        isLose8= m_aibus->readParam(9,0,revdata9);
        if(!isLose8){
            retData[3].outterTemp=(revdata9.cPvH*0x100+revdata9.cPvL)/10.0;
        }


        isLose9=readMdb(2,8200);

        isLose9= m_aibus->readModbusParam(2,mdrevdata);
        if(!isLose9){
            ((uint8_t*)&retData[0].hotPower)[3]=mdrevdata.hold0;
            ((uint8_t*)&retData[0].hotPower)[2]=mdrevdata.hold1;
            ((uint8_t*)&retData[0].hotPower)[1]=mdrevdata.hold2;
            ((uint8_t*)&retData[0].hotPower)[0]=mdrevdata.hold3;
        }


        isLose10= m_aibus->readModbusParam(3,mdrevdata);
        if(!isLose10){
            ((uint8_t*)&retData[1].hotPower)[3]=mdrevdata.hold0;
            ((uint8_t*)&retData[1].hotPower)[2]=mdrevdata.hold1;
            ((uint8_t*)&retData[1].hotPower)[1]=mdrevdata.hold2;
            ((uint8_t*)&retData[1].hotPower)[0]=mdrevdata.hold3;
        }


        isLose11= m_aibus->readModbusParam(4,mdrevdata);
        if(!isLose11){
            ((uint8_t*)&retData[2].hotPower)[3]=mdrevdata.hold0;
            ((uint8_t*)&retData[2].hotPower)[2]=mdrevdata.hold1;
            ((uint8_t*)&retData[2].hotPower)[1]=mdrevdata.hold2;
            ((uint8_t*)&retData[2].hotPower)[0]=mdrevdata.hold3;
        }


        isLose12= m_aibus->readModbusParam(5,mdrevdata);
        if(!isLose12){
            ((uint8_t*)&retData[3].hotPower)[3]=mdrevdata.hold0;
            ((uint8_t*)&retData[3].hotPower)[2]=mdrevdata.hold1;
            ((uint8_t*)&retData[3].hotPower)[1]=mdrevdata.hold2;
            ((uint8_t*)&retData[3].hotPower)[0]=mdrevdata.hold3;
        }

        //get total uesed power
        isLose9= m_aibus->readModbusParam(2,mdrevdata,true);
        if(!isLose9){
            ((uint8_t*)&retData[0].totalPower)[3]=mdrevdata.hold0;
            ((uint8_t*)&retData[0].totalPower)[2]=mdrevdata.hold1;
            ((uint8_t*)&retData[0].totalPower)[1]=mdrevdata.hold2;
            ((uint8_t*)&retData[0].totalPower)[0]=mdrevdata.hold3;
        }


        isLose10= m_aibus->readModbusParam(3,mdrevdata,true);
        if(!isLose10){
            ((uint8_t*)&retData[1].totalPower)[3]=mdrevdata.hold0;
            ((uint8_t*)&retData[1].totalPower)[2]=mdrevdata.hold1;
            ((uint8_t*)&retData[1].totalPower)[1]=mdrevdata.hold2;
            ((uint8_t*)&retData[1].totalPower)[0]=mdrevdata.hold3;
        }

        isLose11= m_aibus->readModbusParam(4,mdrevdata,true);
        if(!isLose11){
            ((uint8_t*)&retData[2].totalPower)[3]=mdrevdata.hold0;
            ((uint8_t*)&retData[2].totalPower)[2]=mdrevdata.hold1;
            ((uint8_t*)&retData[2].totalPower)[1]=mdrevdata.hold2;
            ((uint8_t*)&retData[2].totalPower)[0]=mdrevdata.hold3;
        }

        isLose12= m_aibus->readModbusParam(5,mdrevdata,true);
        if(!isLose12){
            ((uint8_t*)&retData[3].totalPower)[3]=mdrevdata.hold0;
            ((uint8_t*)&retData[3].totalPower)[2]=mdrevdata.hold1;
            ((uint8_t*)&retData[3].totalPower)[1]=mdrevdata.hold2;
            ((uint8_t*)&retData[3].totalPower)[0]=mdrevdata.hold3;

        }

        if(isLose12||isLose11||isLose10||isLose9||isLose8||isLose7||isLose6
                ||isLose5 ||isLose4||isLose3||isLose2||isLose1||isLose0) {
            loseRetryNum++;
            if(loseRetryNum==10) {
                qDebug()<<"reopen com port-->CLOSE";
                m_aibus->closeComPort();
            }
        }else{
            if(loseRetryNum) loseRetryNum--;
            emit resultReady(retData);
        }
    }


}

void GetMdbDataThread::acceptIoValue(int address, int value)
{
    QMutexLocker locker(&mutex);

    if(address==1){
        switch (value) {
        case 0:
            m_ioValue= ioValue^0x1;
            break;
        case 1:
            m_ioValue= ioValue^0x4;
            break;
        case 2:
            m_ioValue= ioValue^0x10;
            break;
        case 3:
            m_ioValue= ioValue^0x40;
            break;
        default:
            break;
        }

        if(m_ioValue!=ioValue){
            m_aibus->writeParam(1,16,m_ioValue,revdata10);
            m_ioValue=ioValue;
        }
    }else{
        qDebug()<<"\n write temp :"<<address<<"of value:"<<value<<"is ok? ->";
        qDebug()<<m_aibus->writeParam(address,0,value,revdata11);
    }
}
