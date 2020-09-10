#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QTime>
#include <array>
#include <QModbusDataUnit>
#include <QModbusClient>

#include "qcustomplot.h"
#include "axistag.h"
#include "setting.h"
#include "getmdbdata.h"
#include "dataType.h"
#include "AIBUSProtocol/aibus_Frame.h"
#include "AIBUSProtocol/aibusprotocol.h"
#include <QMetaType>
#include <QSerialPort>


QT_BEGIN_NAMESPACE


namespace Ui {
    class MainWindow;
    class systemDlg;
}

QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    double calcuFactor;
    QMutex mutex;

private:
    void initUI();
    void initPlot();
    void initAction();
    double calculateLamda(double currentPower,double outTempe,double innerTempe);
    void plot1Flow(double innerTemp,double outerTemp,double totalPower);
    void plot2Flow(double innerTemp,double outerTemp,double totalPower);
    void plot3Flow(double innerTemp,double outerTemp,double totalPower);
    void plot4Flow(double innerTemp,double outerTemp,double totalPower);
    void lbForShow(lbShowData *showData,QTime *startTime, int index=0,double result=0);
    QString whatsLeveInResult(double result);
    int readMdb(quint8 address,int dataAddress);
    void wtMdb(int address,int value);
    void updateResult(const std::array<lbShowData,4> rd);
    void writeStringToText(QString* txtStream);
signals:
    void readDone();
    void serialIsDiscnt();

private slots:
    void timerRW();
    void exitExt();

    void mdbReadReady();

    void on_actionChnLag_triggered();

    void on_actionJpnLag_triggered();

    void on_actionEngLag_triggered();

    void on_pbIoOut1_clicked();

    void on_pbIoOut2_clicked();

    void on_pbIoOut3_clicked();

    void on_pbIoOut4_clicked();


    void on_cbableCalcul1_stateChanged(int arg1);

    void on_cbTempeSet1_currentIndexChanged(int index);

    void on_cbableCalcul2_stateChanged(int arg1);

    void on_cbableCalcul3_stateChanged(int arg1);

    void on_cbableCalcul4_stateChanged(int arg1);

    void on_cbTempeSet2_currentIndexChanged(int index);

    void on_cbTempeSet3_currentIndexChanged(int index);

    void on_cbTempeSet4_currentIndexChanged(int index);
    void  aiPortOpen();
    void  mdbPortOpen();
    void twoPortClose();
private:
    Ui::MainWindow *ui;
    Setting         *m_setting;
    SysSetData *m_setData;



    QPointer<QCPGraph> pt1mGraph1;
    QPointer<QCPGraph> pt1mGraph2;
    QPointer<QCPGraph> pt1mGraph3;
    QPointer<QCPGraph> pt2mGraph1;
    QPointer<QCPGraph> pt2mGraph2;
    QPointer<QCPGraph> pt2mGraph3;
    QPointer<QCPGraph> pt3mGraph1;
    QPointer<QCPGraph> pt3mGraph2;
    QPointer<QCPGraph> pt3mGraph3;
    QPointer<QCPGraph> pt4mGraph1;
    QPointer<QCPGraph> pt4mGraph2;
    QPointer<QCPGraph> pt4mGraph3;


    int xj5v,xj6v,xj7v,xj8v;

    lbShowData *test1lbShow,*test2lbShow,*test3lbShow,*test4lbShow;
    int enableCal;
    double calculResult1,calculResult2,calculResult3,calculResult4;
    int productId;
    QCustomPlot *plotTest1;
    QCustomPlot *plotTest2;
    QCustomPlot *plotTest3;
    QCustomPlot *plotTest4;

    AxisTag *pt1mTag1;
    AxisTag *pt1mTag2;
    AxisTag *pt1mTag3;
    AxisTag *pt2mTag1;
    AxisTag *pt2mTag2;
    AxisTag *pt2mTag3;
    AxisTag *pt3mTag1;
    AxisTag *pt3mTag2;
    AxisTag *pt3mTag3;
    AxisTag *pt4mTag1;
    AxisTag *pt4mTag2;
    AxisTag *pt4mTag3;


    QTimer* dataTimer;
    QTime *startT1,*startT2,*startT3,*startT4;
    QTime *averageTstrat1,*averageTstrat2,*averageTstrat3,*averageTstrat4;
    float averageKWstrat1,averageKWstrat2,averageKWstrat3,averageKWstrat4;
    float averageKW1,averageKW2,averageKW3,averageKW4;

    bool readOrWrite;
    bool isOpen1,isOpen2,isOpen3,isOpen4;
    bool yudianPort,powerPort;
    double heatRate;
    int  wtAddress,wtValue;
    QString readmsg;
    QString onLed,offLed;
    QString recordChart1,recordChart2,recordChart3,recordChart4;

    QMessageBox* mmessageBox;
    QTranslator tLag;
//    GetMdbDataThread *gmdWork;

    QModbusReply *lastRequest = nullptr;
    QModbusClient *modbusDevice = nullptr;
    CAiBusProtocol *m_aibus=nullptr;
    AIBUSRCVFRAME_T revdata1,revdata2,revdata3,revdata4;
    AIBUSRCVFRAME_T revdata5,revdata6,revdata7,revdata8,revdata9,revdata10,revdata11,revdata12,revdata13,revdata14;
    std::array<lbShowData,4> recData485;
    int isLose0,isLose1,isLose2,isLose3,isLose4,isLose5,isLose6,
    isLose7,isLose8,isLose9,isLose10,isLose11,isLose12;

};
#endif // MAINWINDOW_H
