#include "mainwindow.h"
#include <iostream>
#include "ui_mainwindow.h"
#include <QModbusRtuSerialMaster>
#include <QStandardItemModel>
#include <QSerialPortInfo>
#include <QDebug>
#include <math.h>
#include <cmath>
#include <QSerialPort>
#include <QModbusRtuSerialMaster>
#include <QModbusReply>
#include <ostream>
#include <chrono>
#include <thread>
#include <cassert>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , dataTimer(new QTimer())
    ,modbusDevice(nullptr)
    , m_aibus(new CAiBusProtocol())
{
    ui->setupUi(this);
    qRegisterMetaType<std::array<lbShowData,4>>("std::array<lbShowData,4>");
    m_setting=new Setting();

    m_setData=m_setting->sysSettings();
    modbusDevice=new QModbusRtuSerialMaster(this);

    calcuFactor=std::log(m_setData->OutDiam.toDouble()/m_setData->InnerDiam.toDouble())
            /(2*m_setData->Length.toDouble()*M_PI);
    heatRate=m_setData->RateofHeat.toDouble();
    dataTimer->setInterval(1000);
    yudianPort=false;
    powerPort=false;
    initUI();
    initPlot();
    initAction();

    aiPortOpen();
    mdbPortOpen();
}

MainWindow::~MainWindow()
{
    dataTimer->stop();
    delete dataTimer;
    if(modbusDevice){
        modbusDevice->disconnectDevice();
    }
    if(m_aibus->isOpen()){
        m_aibus->closeComPort();
    }
    delete modbusDevice;
    delete m_aibus;
    delete ui;
}



void MainWindow::initUI()
{
    ui->leYuDianPort->setText(QString::number(m_setting->getYudianPort()));
    ui->lePowerPort->setText(QString::number(m_setting->getModbusPort()));
    test1lbShow=new lbShowData;
    test2lbShow=new lbShowData;
    test3lbShow=new lbShowData;
    test4lbShow=new lbShowData;
    ui->actionDisconnect485->setEnabled(false);
    startT1=new QTime;
    startT2=new QTime;
    startT3=new QTime;
    startT4=new QTime;


    isOpen1=false;
    isOpen2=false;
    isOpen3=false;
    isOpen4=false;

    const auto portInfos=QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : portInfos) {
        ui->portCombo->addItem(info.portName());
    }
    onLed="border-image: url(:/img/switch-on.png)";
    offLed="border-image: url(:/img/switch-off.png)";
}

void MainWindow::initPlot()
{
    //--1111111111111111111111111111------------------------------
    plotTest1=new QCustomPlot(this);
    plotTest1->yAxis->setTickLabels(false);
    connect(plotTest1->yAxis2,SIGNAL(rangeChanged(QCPRange)),
            plotTest1->yAxis,SLOT(setRange(QCPRange)));
    plotTest1->yAxis2->setVisible(true);
    plotTest1->axisRect()->addAxis(QCPAxis::atRight);
    plotTest1->axisRect()->addAxis(QCPAxis::atRight);

    plotTest1->axisRect()->axis(QCPAxis::atRight,0)->setPadding(35);
    plotTest1->axisRect()->axis(QCPAxis::atRight,1)->setPadding(35);
    plotTest1->axisRect()->axis(QCPAxis::atRight,2)->setPadding(35);

    plotTest1->legend->setVisible(true);
    //create graphs:
    pt1mGraph1=plotTest1->addGraph(plotTest1->xAxis,plotTest1->axisRect()->axis(QCPAxis::atRight,0));
    pt1mGraph2=plotTest1->addGraph(plotTest1->xAxis,plotTest1->axisRect()->axis(QCPAxis::atRight,1));
    pt1mGraph3=plotTest1->addGraph(plotTest1->xAxis,plotTest1->axisRect()->axis(QCPAxis::atRight,2));
    pt1mGraph1->valueAxis()->setRange(0,360);
    pt1mGraph2->valueAxis()->setRange(0,360);

    pt1mGraph1->setPen(QPen(QColor(250,120,0)));
    pt1mGraph2->setPen(QPen(QColor(0,180,60)));
    pt1mGraph3->setPen(QPen(QColor(220,20,60)));
    pt1mGraph1->setName(tr("Inner Temp"));
    pt1mGraph2->setName(tr("Outer Temp"));
    pt1mGraph3->setName(tr("Total Power"));

    //create tags with newly introduced axistag class(see axistag.h/.cpp)
    pt1mTag1=new AxisTag(pt1mGraph1->valueAxis());
    pt1mTag1->setPen(pt1mGraph1->pen());
    pt1mTag2=new AxisTag(pt1mGraph2->valueAxis());
    pt1mTag2->setPen(pt1mGraph2->pen());
    pt1mTag3=new AxisTag(pt1mGraph3->valueAxis());
    pt1mTag3->setPen(pt1mGraph3->pen());
    //22222222222222222222222222222-------------------------------
    plotTest2=new QCustomPlot(this);
    plotTest2->yAxis->setTickLabels(false);
    connect(plotTest2->yAxis2,SIGNAL(rangeChanged(QCPRange)),
            plotTest2->yAxis,SLOT(setRange(QCPRange)));
    plotTest2->yAxis2->setVisible(true);
    plotTest2->axisRect()->addAxis(QCPAxis::atRight);
    plotTest2->axisRect()->addAxis(QCPAxis::atRight);

    plotTest2->axisRect()->axis(QCPAxis::atRight,0)->setPadding(35);
    plotTest2->axisRect()->axis(QCPAxis::atRight,1)->setPadding(35);
    plotTest2->axisRect()->axis(QCPAxis::atRight,2)->setPadding(35);

    plotTest2->legend->setVisible(true);
    //create graphs:
    pt2mGraph1=plotTest2->addGraph(plotTest2->xAxis,plotTest2->axisRect()->axis(QCPAxis::atRight,0));
    pt2mGraph2=plotTest2->addGraph(plotTest2->xAxis,plotTest2->axisRect()->axis(QCPAxis::atRight,1));
    pt2mGraph3=plotTest2->addGraph(plotTest2->xAxis,plotTest2->axisRect()->axis(QCPAxis::atRight,2));
    pt2mGraph1->valueAxis()->setRange(0,360);
    pt2mGraph2->valueAxis()->setRange(0,360);

    pt2mGraph1->setPen(QPen(QColor(250,120,0)));
    pt2mGraph2->setPen(QPen(QColor(0,180,60)));
    pt2mGraph3->setPen(QPen(QColor(220,20,60)));
    pt2mGraph1->setName(tr("Inner Temp"));
    pt2mGraph2->setName(tr("Outer Temp"));
    pt2mGraph3->setName(tr("Total Power"));

    //create tags with newly introduced axistag class(see axistag.h/.cpp)
    pt2mTag1=new AxisTag(pt2mGraph1->valueAxis());
    pt2mTag1->setPen(pt2mGraph1->pen());
    pt2mTag2=new AxisTag(pt2mGraph2->valueAxis());
    pt2mTag2->setPen(pt2mGraph2->pen());
    pt2mTag3=new AxisTag(pt2mGraph3->valueAxis());
    pt2mTag3->setPen(pt2mGraph3->pen());

    //333333333333333333333333333333----------------------
    plotTest3=new QCustomPlot(this);
    plotTest3->yAxis->setTickLabels(false);
    connect(plotTest3->yAxis2,SIGNAL(rangeChanged(QCPRange)),
            plotTest3->yAxis,SLOT(setRange(QCPRange)));
    plotTest3->yAxis2->setVisible(true);
    plotTest3->axisRect()->addAxis(QCPAxis::atRight);
    plotTest3->axisRect()->addAxis(QCPAxis::atRight);

    plotTest3->axisRect()->axis(QCPAxis::atRight,0)->setPadding(35);
    plotTest3->axisRect()->axis(QCPAxis::atRight,1)->setPadding(35);
    plotTest3->axisRect()->axis(QCPAxis::atRight,2)->setPadding(35);

    plotTest3->legend->setVisible(true);
    //create graphs:
    pt3mGraph1=plotTest3->addGraph(plotTest3->xAxis,plotTest3->axisRect()->axis(QCPAxis::atRight,0));
    pt3mGraph2=plotTest3->addGraph(plotTest3->xAxis,plotTest3->axisRect()->axis(QCPAxis::atRight,1));
    pt3mGraph3=plotTest3->addGraph(plotTest3->xAxis,plotTest3->axisRect()->axis(QCPAxis::atRight,2));
    pt3mGraph1->valueAxis()->setRange(0,360);
    pt3mGraph2->valueAxis()->setRange(0,360);


    pt3mGraph1->setPen(QPen(QColor(250,120,0)));
    pt3mGraph2->setPen(QPen(QColor(0,180,60)));
    pt3mGraph3->setPen(QPen(QColor(220,20,60)));
    pt3mGraph1->setName(tr("Inner Temp"));
    pt3mGraph2->setName(tr("Outer Temp"));
    pt3mGraph3->setName(tr("Total Power"));

    //create tags with newly introduced axistag class(see axistag.h/.cpp)
    pt3mTag1=new AxisTag(pt3mGraph1->valueAxis());
    pt3mTag1->setPen(pt3mGraph1->pen());
    pt3mTag2=new AxisTag(pt3mGraph2->valueAxis());
    pt3mTag2->setPen(pt3mGraph2->pen());
    pt3mTag3=new AxisTag(pt3mGraph3->valueAxis());
    pt3mTag3->setPen(pt3mGraph3->pen());
    //4444444444444444444444444444444444444444----------------------------
    plotTest4=new QCustomPlot(this);
    plotTest4->yAxis->setTickLabels(false);
    connect(plotTest4->yAxis2,SIGNAL(rangeChanged(QCPRange)),
            plotTest4->yAxis,SLOT(setRange(QCPRange)));

    plotTest4->yAxis2->setVisible(true);

    plotTest4->axisRect()->addAxis(QCPAxis::atRight);
    plotTest4->axisRect()->addAxis(QCPAxis::atRight);

    plotTest4->axisRect()->axis(QCPAxis::atRight,0)->setPadding(35);
    plotTest4->axisRect()->axis(QCPAxis::atRight,1)->setPadding(35);
    plotTest4->axisRect()->axis(QCPAxis::atRight,2)->setPadding(35);

    plotTest4->legend->setVisible(true);
    //create graphs:
    pt4mGraph1=plotTest4->addGraph(plotTest4->xAxis,plotTest4->axisRect()->axis(QCPAxis::atRight,0));
    pt4mGraph2=plotTest4->addGraph(plotTest4->xAxis,plotTest4->axisRect()->axis(QCPAxis::atRight,1));
    pt4mGraph3=plotTest4->addGraph(plotTest4->xAxis,plotTest4->axisRect()->axis(QCPAxis::atRight,2));
    pt4mGraph1->valueAxis()->setRange(0,360);
    pt4mGraph2->valueAxis()->setRange(0,360);

    pt4mGraph1->setPen(QPen(QColor(250,120,0)));
    pt4mGraph2->setPen(QPen(QColor(0,180,60)));
    pt4mGraph3->setPen(QPen(QColor(220,20,60)));
    pt4mGraph1->setName(tr("Inner Temp"));
    pt4mGraph2->setName(tr("Outer Temp"));
    pt4mGraph3->setName(tr("Total Power"));

    //create tags with newly introduced axistag class(see axistag.h/.cpp)
    pt4mTag1=new AxisTag(pt4mGraph1->valueAxis());
    pt4mTag1->setPen(pt4mGraph1->pen());
    pt4mTag2=new AxisTag(pt4mGraph2->valueAxis());
    pt4mTag2->setPen(pt4mGraph2->pen());
    pt4mTag3=new AxisTag(pt4mGraph3->valueAxis());
    pt4mTag3->setPen(pt4mGraph3->pen());
    //    plotTest4->yAxis->setRange(0,360);


    ui->twLoPlot1->addWidget(plotTest1);
    ui->twLoPlot2->addWidget(plotTest2);
    ui->twLoPlot3->addWidget(plotTest3);
    ui->twLoPlot4->addWidget(plotTest4);

}

void MainWindow::initAction()
{
    connect(ui->actionconnect485,&QAction::triggered,
            this,&MainWindow::aiPortOpen);
    connect(ui->actionconnect485,&QAction::triggered,
            this,&MainWindow::mdbPortOpen);
    connect(ui->actionDisconnect485,&QAction::triggered,
            this,&MainWindow::twoPortClose);
    connect(this,&MainWindow::changePort,
            m_setting,&Setting::updatePort);
    connect(ui->pbExit,&QPushButton::clicked,this,&MainWindow::exitExt);
    connect(ui->actionoptionSys,&QAction::triggered,m_setting,&QDialog::show);
    connect(dataTimer,SIGNAL(timeout()),this,SLOT(timerRW()));

}


double MainWindow::calculateLamda(double currentPower, double outTempe, double innerTempe)
{    
    double retCalcu=currentPower*heatRate;
    double logCalcu=calcuFactor/(innerTempe-outTempe);
    //    qDebug()<<"Heat total: "<<retCalcu<<" log:"<<logCalcu<<" and Result : "<<retCalcu*logCalcu;
    return retCalcu*logCalcu;
}

void MainWindow::plot1Flow(double innerTemp, double outerTemp, double totalPower)
{
    //calculate and add new data point to each graph
    pt1mGraph1->addData(pt1mGraph1->dataCount(),innerTemp);
    pt1mGraph2->addData(pt1mGraph2->dataCount(),outerTemp);
    pt1mGraph3->addData(pt1mGraph3->dataCount(),totalPower);

    //make key axis range scroll with the data:
    plotTest1->xAxis->rescale();

    //    pt1mGraph1->rescaleValueAxis(false,true);
    //    pt1mGraph2->rescaleValueAxis(false,true);
    pt1mGraph3->rescaleValueAxis(/*false*/true,true);
    plotTest1->xAxis->setRange(plotTest1->xAxis->range().upper,100,Qt::AlignRight);

    //update the vertical axis tag position and
    //texts to match the rightmost data point of the graphs:
    double graph1Value=pt1mGraph1->dataMainValue(pt1mGraph1->dataCount()-1);
    double graph2Value=pt1mGraph2->dataMainValue(pt1mGraph2->dataCount()-1);
    double graph3Value=pt1mGraph3->dataMainValue(pt1mGraph3->dataCount()-1);

    pt1mTag1->updatePosition(graph1Value);
    pt1mTag2->updatePosition(graph2Value);
    pt1mTag3->updatePosition(graph3Value);

    pt1mTag1->setText(QString::number(graph1Value,'f',2));
    pt1mTag2->setText(QString::number(graph2Value,'f',2));
    pt1mTag3->setText(QString::number(graph3Value,'f',2));

    plotTest1->replot();
}

void MainWindow::plot2Flow(double innerTemp, double outerTemp, double totalPower)
{
    //calculate and add new data point to each graph
    pt2mGraph1->addData(pt2mGraph1->dataCount(),innerTemp);
    pt2mGraph2->addData(pt2mGraph2->dataCount(),outerTemp);
    pt2mGraph3->addData(pt2mGraph3->dataCount(),totalPower);

    //make key axis range scroll with the data:
    plotTest2->xAxis->rescale();

    //    pt2mGraph1->rescaleValueAxis(false,true);
    //    pt2mGraph2->rescaleValueAxis(false,true);
    pt2mGraph3->rescaleValueAxis(false,true);
    plotTest2->xAxis->setRange(plotTest2->xAxis->range().upper,100,Qt::AlignRight);

    //update the vertical axis tag position and
    //texts to match the rightmost data point of the graphs:
    double graph1Value=pt2mGraph1->dataMainValue(pt2mGraph1->dataCount()-1);
    double graph2Value=pt2mGraph2->dataMainValue(pt2mGraph2->dataCount()-1);
    double graph3Value=pt2mGraph3->dataMainValue(pt2mGraph3->dataCount()-1);

    pt2mTag1->updatePosition(graph1Value);
    pt2mTag2->updatePosition(graph2Value);
    pt2mTag3->updatePosition(graph3Value);

    pt2mTag1->setText(QString::number(graph1Value,'f',2));
    pt2mTag2->setText(QString::number(graph2Value,'f',2));
    pt2mTag3->setText(QString::number(graph3Value,'f',2));

    plotTest2->replot();
}

void MainWindow::plot3Flow(double innerTemp, double outerTemp, double totalPower)
{
    //calculate and add new data point to each graph
    pt3mGraph1->addData(pt3mGraph1->dataCount(),innerTemp);
    pt3mGraph2->addData(pt3mGraph2->dataCount(),outerTemp);
    pt3mGraph3->addData(pt3mGraph3->dataCount(),totalPower);

    //make key axis range scroll with the data:
    plotTest3->xAxis->rescale();

    //    pt3mGraph1->rescaleValueAxis(false,true);
    //    pt3mGraph2->rescaleValueAxis(false,true);
    pt3mGraph3->rescaleValueAxis(false,true);
    plotTest3->xAxis->setRange(plotTest3->xAxis->range().upper,100,Qt::AlignRight);

    //update the vertical axis tag position and
    //texts to match the rightmost data point of the graphs:
    double graph1Value=pt3mGraph1->dataMainValue(pt1mGraph1->dataCount()-1);
    double graph2Value=pt3mGraph2->dataMainValue(pt1mGraph2->dataCount()-1);
    double graph3Value=pt3mGraph3->dataMainValue(pt1mGraph3->dataCount()-1);

    pt3mTag1->updatePosition(graph1Value);
    pt3mTag2->updatePosition(graph2Value);
    pt3mTag3->updatePosition(graph3Value);

    pt3mTag1->setText(QString::number(graph1Value,'f',2));
    pt3mTag2->setText(QString::number(graph2Value,'f',2));
    pt3mTag3->setText(QString::number(graph3Value,'f',2));

    plotTest3->replot();
}

void MainWindow::plot4Flow(double innerTemp, double outerTemp, double totalPower)
{
    //calculate and add new data point to each graph
    pt4mGraph1->addData(pt4mGraph1->dataCount(),innerTemp);
    pt4mGraph2->addData(pt4mGraph2->dataCount(),outerTemp);
    pt4mGraph3->addData(pt4mGraph3->dataCount(),totalPower);

    //make key axis range scroll with the data:
    plotTest4->xAxis->rescale();
    plotTest4->xAxis->setRange(plotTest4->xAxis->range().upper,1000,Qt::AlignRight);

    //update the vertical axis tag position and
    //texts to match the rightmost data point of the graphs:
    double graph1Value=pt4mGraph1->dataMainValue(pt4mGraph1->dataCount()-1);
    double graph2Value=pt4mGraph2->dataMainValue(pt4mGraph2->dataCount()-1);
    double graph3Value=pt4mGraph3->dataMainValue(pt4mGraph3->dataCount()-1);

    pt4mTag1->updatePosition(graph1Value);
    pt4mTag2->updatePosition(graph2Value);
    pt4mTag3->updatePosition(graph3Value);

    pt4mTag1->setText(QString::number(graph1Value,'f',2));
    pt4mTag2->setText(QString::number(graph2Value,'f',2));
    pt4mTag3->setText(QString::number(graph3Value,'f',2));

    plotTest4->replot();
}

void MainWindow::lbForShow(lbShowData *showData, QTime *startTime, int index, double result)
{
    QTime vOftime(0,0,0,0);
    vOftime=vOftime.addMSecs(startTime->elapsed());
    QString strOftime=vOftime.toString("h:mm:ss");

    switch (index) {
    case 0:
        ui->lbInnerC1->setText(QString::number(showData->innerTemp));
        ui->lbOutC1->setText(QString::number(showData->outterTemp));
        ui->lbSetC1->setText(QString::number(showData->innerSetTemp));
        ui->lbHotpower1->setText(QString::number(showData->totalPower));
        ui->lbTime1->setText(strOftime);
        ui->lbResult1->setText(QString::number(result,10,9));
        ui->lbLevel1->setText(whatsLeveInResult(result));

        break;
    case 1:
        ui->lbInnerC2->setText(QString::number(showData->innerTemp));
        ui->lbOutC2->setText(QString::number(showData->outterTemp));
        ui->lbSetC2->setText(QString::number(showData->innerSetTemp));
        ui->lbHotpower2->setText(QString::number(showData->totalPower));
        ui->lbTime2->setText(strOftime);
        ui->lbResult2->setText(QString::number(result,10,9));
        ui->lbLevel2->setText(whatsLeveInResult(result));
        break;
    case 2:
        ui->lbInnerC3->setText(QString::number(showData->innerTemp));
        ui->lbOutC3->setText(QString::number(showData->outterTemp));
        ui->lbSetC3->setText(QString::number(showData->innerSetTemp));
        ui->lbHotpower3->setText(QString::number(showData->totalPower));
        ui->lbTime3->setText(strOftime);
        ui->lbResult3->setText(QString::number(result,10,9));
        ui->lbLevel3->setText(whatsLeveInResult(result));
        break;
    case 3:
        ui->lbInnerC4->setText(QString::number(showData->innerTemp));
        ui->lbOutC4->setText(QString::number(showData->outterTemp));
        ui->lbSetC4->setText(QString::number(showData->innerSetTemp));
        ui->lbHotpower4->setText(QString::number(showData->totalPower));
        ui->lbTime4->setText(strOftime);
        ui->lbResult4->setText(QString::number(result,10,9));
        ui->lbLevel4->setText(whatsLeveInResult(result));
        break;
    default:
        break;
    }
}

QString MainWindow::whatsLeveInResult(double result)
{
    QString whatsLevel="";
    if(result==0){
        whatsLevel="Waiting...";
    }else if(result>=0.06&&result<0.08){
        whatsLevel="A";
    }else if(result>=0.04&&result<0.06){
        whatsLevel="B";
    }else if(result>=0.02&&result<0.04){
        whatsLevel="C";
    }else if(result>=0.006&&result<0.02){
        whatsLevel="D";
    }else if(result>=0.002&&result<0.006){
        whatsLevel="E";
    }else if(result>0&&result<0.002){
        whatsLevel="ERROR";
    }
    return whatsLevel;
}

int MainWindow::readMdb(quint8 address, int dataAddress)
{
    if(!modbusDevice) return ERR_ERROR;
    QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters,dataAddress,4);

    QMutexLocker locker(&mutex);
    if(auto *reply=modbusDevice->sendReadRequest(readUnit,address)){
        if(!reply->isFinished()){
            QFileDevice::connect(reply,&QModbusReply::finished,this,&MainWindow::mdbReadReady);
        }else {
            delete reply;
        }
    }
    else {
        qDebug()<<"read error in showData"
               <<modbusDevice->errorString();
    }
    return ERR_OK;
}

void MainWindow::wtMdb(int address, int value)
{
    dataTimer->stop();
    int rtResult=m_aibus->writeParam(address,0,value,revdata5);
    if(!rtResult){
        switch (address) {
        case 10:
            recData485[0].innerTemp=(revdata5.cPvH*0x100+revdata5.cPvL)/10.0;
            recData485[0].innerSetTemp=(revdata5.cSvH*0x100+revdata5.cSvL)/10.0;
            break;
        case 11:
            recData485[1].innerTemp=(revdata5.cPvH*0x100+revdata5.cPvL)/10.0;
            recData485[1].innerSetTemp=(revdata5.cSvH*0x100+revdata5.cSvL)/10.0;
            break;
        case 12:
            recData485[2].innerTemp=(revdata5.cPvH*0x100+revdata5.cPvL)/10.0;
            recData485[2].innerSetTemp=(revdata5.cSvH*0x100+revdata5.cSvL)/10.0;
            break;
        case 13:
            recData485[3].innerTemp=(revdata5.cPvH*0x100+revdata5.cPvL)/10.0;
            recData485[3].innerSetTemp=(revdata5.cSvH*0x100+revdata5.cSvL)/10.0;
            break;
        default:
            break;
        }

        updateResult(recData485);

        dataTimer->start();
    }
}

void MainWindow::aiPortOpen()
{
    if(!(m_aibus->comPortOpen("com"+ui->leYuDianPort->text()))){
        yudianPort=false;
        QMessageBox::information(this,"title",QString(" com port %1 is not open").arg(ui->leYuDianPort->text()));
    }else{
        dataTimer->start();
        yudianPort=true;
    }

}

void MainWindow::mdbPortOpen()
{
    if (!modbusDevice){
        powerPort=false;
    }
    if (modbusDevice->state() != QModbusDevice::ConnectedState) {
        modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter,
                                             "com"+ui->lePowerPort->text());
        modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,
                                             QSerialPort::NoParity);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,
                                             QSerialPort::Baud9600);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,
                                             QSerialPort::Data8);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,
                                             QSerialPort::TwoStop);
        modbusDevice->setTimeout(100);
        modbusDevice->setNumberOfRetries(10);
        if (!modbusDevice->connectDevice()) {
            powerPort=false;
            QMessageBox::information(this,"title",QString(" com port %1 is not open").arg(ui->lePowerPort->text()));
        } else {
            powerPort=true;
            dataTimer->start();
        }
    } else {
        modbusDevice->disconnectDevice();
        powerPort=false;
    }
}


void MainWindow::timerRW()
{
    if(yudianPort){
        isLose2= m_aibus->readParam(6,0,revdata6);
        if(!isLose2){
            recData485[0].outterTemp=(revdata6.cPvH*0x100+revdata6.cPvL)/10.0;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds{80});
        isLose4= m_aibus->readParam(7,0,revdata7);
        if(!isLose4){
            recData485[1].outterTemp=(revdata7.cPvH*0x100+revdata7.cPvL)/10.0;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds{80});

        isLose6= m_aibus->readParam(8,0,revdata8);
        if(!isLose6){
            recData485[2].outterTemp=(revdata8.cPvH*0x100+revdata8.cPvL)/10.0;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds{80});

        isLose8= m_aibus->readParam(9,0,revdata9);
        if(!isLose8){
            recData485[3].outterTemp=(revdata9.cPvH*0x100+revdata9.cPvL)/10.0;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds{80});

        isLose1= m_aibus->readParam(10,0,revdata10);
        if(!isLose1){
            recData485[0].innerTemp=(revdata10.cPvH*0x100+revdata10.cPvL)/10.0;
            recData485[0].innerSetTemp=(revdata10.cSvH*0x100+revdata10.cSvL)/10.0;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds{80});

        isLose3= m_aibus->readParam(11,0,revdata11);
        if(!isLose3){
            recData485[1].innerTemp=(revdata11.cPvH*0x100+revdata11.cPvL)/10.0;
            recData485[1].innerSetTemp=(revdata11.cSvH*0x100+revdata11.cSvL)/10.0;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds{80});

        isLose5= m_aibus->readParam(12,0,revdata12);
        if(!isLose5){
            recData485[2].innerTemp=(revdata12.cPvH*0x100+revdata12.cPvL)/10.0;
            recData485[2].innerSetTemp=(revdata12.cSvH*0x100+revdata12.cSvL)/10.0;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds{80});

        isLose7= m_aibus->readParam(13,0,revdata13);
        if(!isLose7){
            recData485[3].innerTemp=(revdata13.cPvH*0x100+revdata13.cPvL)/10.0;
            recData485[3].innerSetTemp=(revdata13.cSvH*0x100+revdata13.cSvL)/10.0;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds{80});

    }
    if(powerPort){
        readMdb(2,0x4000);
        std::this_thread::sleep_for(std::chrono::milliseconds{80});

        readMdb(3,0x4000);
        std::this_thread::sleep_for(std::chrono::milliseconds{80});

        readMdb(4,0x4000);
        std::this_thread::sleep_for(std::chrono::milliseconds{80});

        readMdb(5,0x4000);
    }
    updateResult(recData485);
}

void MainWindow::exitExt()
{
    if(QMessageBox::Yes==QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("您要退出程序吗,点击<yes>退出"),
                                              QMessageBox::Yes|QMessageBox::Cancel)){
        QApplication *app;
        app->exit(0);
    }
}

void MainWindow::mdbReadReady()
{
    auto reply=qobject_cast<QModbusReply *>(sender());
    if(!reply){
        return;
    }

    //ok manage receve data
    if(reply->error()==QModbusDevice::NoError){
        const QModbusDataUnit rdResult=reply->result();
        switch (reply->serverAddress()) {
        case 2:
            ((uint16_t*)&recData485[0].totalPower)[1]=rdResult.value(0);
            ((uint16_t*)&recData485[0].totalPower)[0]=rdResult.value(1);
            break;
        case 3:
            ((uint16_t*)&recData485[1].totalPower)[1]=rdResult.value(0);
            ((uint16_t*)&recData485[1].totalPower)[0]=rdResult.value(1);
            break;
        case 4:
            ((uint16_t*)&recData485[2].totalPower)[1]=rdResult.value(0);
            ((uint16_t*)&recData485[2].totalPower)[0]=rdResult.value(1);
            break;
        case 5:
            ((uint16_t*)&recData485[3].totalPower)[1]=rdResult.value(0);
            ((uint16_t*)&recData485[3].totalPower)[0]=rdResult.value(1);
            break;
        default:
            break;
        }

        //        qDebug()<<QString("hotPower1: %1 ,hotPower2 : %2 ,hotPower3 : %3 ,hotPower4 : %4 ,address is : %5")
        //                  .arg(recData485[0].totalPower)
        //                  .arg(recData485[1].totalPower)
        //                  .arg(recData485[2].totalPower)
        //                  .arg(recData485[3].totalPower)
        //                  .arg(reply->serverAddress());
    }else{
        qDebug()<<"error read response : "<<reply->errorString();
    }
    reply->deleteLater();
}

void MainWindow::on_actionChnLag_triggered()
{
    tLag.load(":/Chn.qm");
    qApp->installTranslator(&tLag);
    this->ui->retranslateUi(this);
}

void MainWindow::on_actionJpnLag_triggered()
{
    tLag.load(":/Jap.qm");
    qApp->installTranslator(&tLag);
    this->ui->retranslateUi(this);
}

void MainWindow::on_actionEngLag_triggered()
{
    tLag.load(":/Arab.qm");
    qApp->installTranslator(&tLag);
    this->ui->retranslateUi(this);
}

void MainWindow::on_pbIoOut1_clicked()
{
    mmessageBox=new QMessageBox();
    QString msgTitle,msgText;

    if(!isOpen1){
        msgTitle=tr("Do you start NOW");
        msgText=tr("YES:: go heating .\n NO: DONOT HEATING");
    }else{
        msgTitle=tr("STOP HEAT");
        msgText=tr("YES:: go temperature 0 .\n NO: DONOT HEATING");
    }

    if(mmessageBox->information(this,msgTitle,msgText,QMessageBox::Yes|QMessageBox::No)==QMessageBox::Yes){
        if(!isOpen1){
            startT1=new QTime;
            startT1->start();
            ui->cbableCalcul1->setCheckState(Qt::Unchecked);
            averageKWstrat1=recData485[0].totalPower;
            if(yudianPort) wtMdb(10,m_setData->HighTempe.toDouble()*10);
            isOpen1=true;
        }else{
            if(yudianPort) wtMdb(10,0);
            isOpen1=false;
        }
    }
}

void MainWindow::on_pbIoOut2_clicked()
{
    mmessageBox=new QMessageBox();
    QString msgTitle,msgText;

    if(!isOpen2){
        msgTitle=tr("Do you start NOW");
        msgText=tr("YES:: go heating .\n NO: DONOT HEATING");
    }else{
        msgTitle=tr("STOP HEAT");
        msgText=tr("YES:: go temperature 0 .\n NO: DONOT HEATING");
    }

    if(mmessageBox->information(this,msgTitle,msgText,QMessageBox::Yes|QMessageBox::No)==QMessageBox::Yes){

        if(!isOpen2){
            startT2=new QTime;
            startT2->start();
            ui->cbableCalcul2->setCheckState(Qt::Unchecked);
            averageKWstrat2=recData485[1].totalPower;
            if(yudianPort) wtMdb(11,m_setData->HighTempe.toDouble()*10);
            isOpen2=true;
        }else{
            if(yudianPort) wtMdb(11,0);
            isOpen2=false;
        }
    }
}

void MainWindow::on_pbIoOut3_clicked()
{
    mmessageBox=new QMessageBox();
    QString msgTitle,msgText;

    if(!isOpen3){
        msgTitle=tr("Do you start NOW");
        msgText=tr("YES:: go heating .\n NO: DONOT HEATING");
    }else{
        msgTitle=tr("STOP HEAT");
        msgText=tr("YES:: go temperature 0 .\n NO: DONOT HEATING");
    }

    if(mmessageBox->information(this,msgTitle,msgText,QMessageBox::Yes|QMessageBox::No)==QMessageBox::Yes){

        if(!isOpen3){
            startT3=new QTime;
            startT3->start();
            ui->cbableCalcul3->setCheckState(Qt::Unchecked);
            averageKWstrat3=recData485[2].totalPower;
            if(yudianPort) wtMdb(12,m_setData->HighTempe.toDouble()*10);
            isOpen3=true;
        }else{
            if(yudianPort) wtMdb(12,0);
            isOpen3=false;
        }
    }
}

void MainWindow::on_pbIoOut4_clicked()
{
    mmessageBox=new QMessageBox();
    QString msgTitle,msgText;

    if(!isOpen4){
        msgTitle=tr("Do you start NOW");
        msgText=tr("YES:: go heating .\n NO: DONOT HEATING");
    }else{
        msgTitle=tr("STOP HEAT");
        msgText=tr("YES:: go temperature 0 .\n NO: DONOT HEATING");
    }

    if(mmessageBox->information(this,msgTitle,msgText,QMessageBox::Yes|QMessageBox::No)==QMessageBox::Yes){

        if(!isOpen4){
            startT4=new QTime;
            startT4->start();
            ui->cbableCalcul4->setCheckState(Qt::Unchecked);
            averageKWstrat4=recData485[3].totalPower;
            if(yudianPort) wtMdb(13,m_setData->HighTempe.toDouble()*10);
            isOpen4=true;
        }else{
            if(yudianPort) wtMdb(13,0);
            isOpen4=false;
        }
    }
}


void MainWindow::updateResult(std::array<lbShowData,4> rd)
{
    float powerUsed=0.0;
    //11111111111111111111111111111111
    if(isOpen1){
        ui->pbIoOut1->setStyleSheet(onLed);
        powerUsed=rd[0].totalPower-averageKWstrat1;
        if(ui->cbableCalcul1->isChecked()){
            averageKW1=(powerUsed*1000)/(averageTstrat1->elapsed()/1000);
            calculResult1=calculateLamda(averageKW1,rd[0].outterTemp,rd[0].innerTemp);
            lbForShow(&rd[0],startT1,0,calculResult1);
        }else{
            lbForShow(&rd[0],startT1,0);
        }
        recordChart1+=QString("%1,%2,%3 \n").arg(rd[0].innerTemp)
                .arg(rd[0].outterTemp)
                .arg(powerUsed);
        plot1Flow(rd[0].innerTemp,rd[0].outterTemp,powerUsed);
    }else{
        ui->pbIoOut1->setStyleSheet(offLed);
    }
    //2222222222222222222
    if(isOpen2){
        ui->pbIoOut2->setStyleSheet(onLed);
        powerUsed=rd[1].totalPower-averageKWstrat2;

        if(ui->cbableCalcul2->isChecked()){
            averageKW2=(powerUsed*1000)/(averageTstrat2->elapsed()/1000);
            calculResult2=calculateLamda(averageKW2,rd[1].outterTemp,rd[1].innerTemp);
            lbForShow(&rd[1],startT2,1,calculResult2);
        }else{
            lbForShow(&rd[1],startT2,1);
        }
        recordChart2+=QString("%1,%2,%3 \n").arg(rd[1].innerTemp)
                .arg(rd[1].outterTemp)
                .arg(powerUsed);
        plot2Flow(rd[1].innerTemp,rd[1].outterTemp,powerUsed);
    }else{
        ui->pbIoOut2->setStyleSheet(offLed);
    }
    //3333333333333333333333333333
    if(isOpen3){
        ui->pbIoOut3->setStyleSheet(onLed);
        powerUsed=rd[2].totalPower-averageKWstrat3;

        if(ui->cbableCalcul3->isChecked()){
            averageKW3=(powerUsed*1000)/(averageTstrat3->elapsed()/1000);
            calculResult3=calculateLamda(averageKW3,rd[2].outterTemp,rd[2].innerTemp);
            lbForShow(&rd[2],startT3,2,calculResult3);
        }else{
            lbForShow(&rd[2],startT3,2);
        }
        recordChart3+=QString("%1,%2,%3 \n").arg(rd[2].innerTemp)
                .arg(rd[2].outterTemp)
                .arg(powerUsed);
        plot3Flow(rd[2].innerTemp,rd[2].outterTemp,powerUsed);
    }else{
        ui->pbIoOut3->setStyleSheet(offLed);
    }
    //444444444444444444444444
    if(isOpen4){
        ui->pbIoOut4->setStyleSheet(onLed);
        powerUsed=rd[3].totalPower-averageKWstrat4;

        if(ui->cbableCalcul4->isChecked()){
            averageKW4=(powerUsed*1000)/(averageTstrat4->elapsed()/1000);
            calculResult4=calculateLamda(averageKW4,rd[3].outterTemp,rd[3].innerTemp);
            lbForShow(&rd[3],startT4,0,calculResult4);
        }else{
            lbForShow(&rd[0],startT4,0);
        }
        recordChart4+=QString("%1,%2,%3 \n").arg(rd[3].innerTemp)
                .arg(rd[3].outterTemp)
                .arg(powerUsed);
        plot4Flow(rd[3].innerTemp,rd[3].outterTemp,powerUsed);
    }else{
        ui->pbIoOut4->setStyleSheet(offLed);
    }

}

void MainWindow::writeStringToText(QString *txtStream)
{
    QString dataSaveAddress=QApplication::applicationDirPath()+"/Data/"+QDate::currentDate().toString("yyyyMMdd");
    QDir dir;
    if(!dir.exists(dataSaveAddress)){
        bool makeDir=dir.mkdir(dataSaveAddress);
        qDebug()<<"create save file address..."<<makeDir<<"..."<<dataSaveAddress;
    }
//    QString fname=dataSaveAddress+"/"+QString::number(productId)+".txt";
    QString fname=QString("%1/%2.txt").arg(dataSaveAddress).arg(productId);
    QFile   file(fname);
    if(!file.exists()){
        qDebug()<<"file not exist";
    }

    if(!file.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append)){
        qDebug()<<fname<<"is have error";
    }
    QTextStream stream(&file);
    stream<<*txtStream<<"\n";
    file.close();
    *txtStream="";
}


void MainWindow::on_cbableCalcul1_stateChanged(int arg1)
{
    if(arg1==Qt::Checked){
        averageTstrat1=new QTime;
        averageTstrat1->start();
        averageKWstrat1=recData485[0].totalPower;
        qDebug()<<averageKWstrat1;
    }
}

void MainWindow::on_cbTempeSet1_currentIndexChanged(int index)
{
    //    qDebug()<<"combobox out is :"<<ui->cbTempeSet1->itemData(index,Qt::DisplayRole).toInt()*10;
    wtMdb(10,ui->cbTempeSet1->itemData(index,Qt::DisplayRole).toInt()*10);
}

void MainWindow::on_cbableCalcul2_stateChanged(int arg1)
{
    if(arg1==Qt::Checked){
        averageTstrat2=new QTime;
        averageTstrat2->start();
        averageKWstrat2=recData485[1].totalPower;
        qDebug()<<averageKWstrat2;
    }
}

void MainWindow::on_cbableCalcul3_stateChanged(int arg1)
{
    if(arg1==Qt::Checked){
        averageTstrat3=new QTime;
        averageTstrat3->start();
        averageKWstrat3=recData485[2].totalPower;
        qDebug()<<averageKWstrat3;
    }
}

void MainWindow::on_cbableCalcul4_stateChanged(int arg1)
{
    if(arg1==Qt::Checked){
        averageTstrat4=new QTime;
        averageTstrat4->start();
        averageKWstrat4=recData485[3].totalPower;
        qDebug()<<averageKWstrat4;
    }
}

void MainWindow::on_cbTempeSet2_currentIndexChanged(int index)
{
    //    qDebug()<<"combobox out is :"<<ui->cbTempeSet1->itemData(index,Qt::DisplayRole).toInt()*10;
    wtMdb(11,ui->cbTempeSet2->itemData(index,Qt::DisplayRole).toInt()*10);
}

void MainWindow::on_cbTempeSet3_currentIndexChanged(int index)
{
    //    qDebug()<<"combobox out is :"<<ui->cbTempeSet1->itemData(index,Qt::DisplayRole).toInt()*10;
    wtMdb(12,ui->cbTempeSet3->itemData(index,Qt::DisplayRole).toInt()*10);
}

void MainWindow::on_cbTempeSet4_currentIndexChanged(int index)
{
    //    qDebug()<<"combobox out is :"<<ui->cbTempeSet1->itemData(index,Qt::DisplayRole).toInt()*10;
    wtMdb(13,ui->cbTempeSet4->itemData(index,Qt::DisplayRole).toInt()*10);
}

void MainWindow::twoPortClose()
{
    if(m_aibus->isOpen()){
        m_aibus->closeComPort();
        yudianPort=false;
    }
    if(modbusDevice){
        modbusDevice->disconnectDevice();
        powerPort=false;
    }
}


void MainWindow::on_leYuDianPort_editingFinished()
{
    emit changePort(ui->leYuDianPort->text().toInt(),ui->lePowerPort->text().toInt());
}

void MainWindow::on_lePowerPort_editingFinished()
{
    emit changePort(ui->leYuDianPort->text().toInt(),ui->lePowerPort->text().toInt());
}
