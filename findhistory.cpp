#include "findhistory.h"
#include "ui_findhistory.h"
#include "mainwindow.h"
#include <QDebug>
#include <QSqlRecord>
#include <QMessageBox>
#include <QDir>



findHistory::findHistory(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::findHistory)
{
    ui->setupUi(this);

    m_cuse=new commenUse();
    m_rdata=new ReportData();
    grfAddress=QApplication::applicationDirPath();
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

//    ui->deEnd->setDate(QDate::currentDate());
//    ui->deEnd->setCalendarPopup(true);

//    ui->deStart->setDate(QDate::currentDate().addDays(-1));
//    ui->deStart->setCalendarPopup(true);
}


findHistory::~findHistory()
{
    delete ui;
}

void findHistory::on_pbFindShowAll_clicked()
{
    model=new QSqlQueryModel;
    model->setQuery("SELECT * FROM dayWork",database);
    updateTableView();
}

void findHistory::on_pbFindByDay_clicked()
{
    QString sqlFindStr=QString("SELECT * FROM dayWork WHERE day BETWEEN '%1-%2-%3' AND '%4-%5-%6' ")
            .arg(ui->sbStartYear->value())
            .arg(ui->sbStartMonth->value(),2,10,QLatin1Char('0'))
            .arg(ui->sbStartDay->value(),2,10,QLatin1Char('0'))
            .arg(ui->sbEndYear->value())
            .arg(ui->sbEndMonth->value(),2,10,QLatin1Char('0'))
            .arg(ui->sbEndDay->value(),2,10,QLatin1Char('0'));
    model=new QSqlQueryModel;
    model->setQuery(sqlFindStr,database);
    updateTableView();
}

void findHistory::updateTableView()
{
    model->setHeaderData(0,Qt::Horizontal,QStringLiteral("序号"));
    model->setHeaderData(1,Qt::Horizontal,QStringLiteral("送检单位"));
    model->setHeaderData(2,Qt::Horizontal,QStringLiteral("检测人员"));
    model->setHeaderData(3,Qt::Horizontal,QStringLiteral("检测日期"));
    model->setHeaderData(4,Qt::Horizontal,QStringLiteral("检测时间"));
    model->setHeaderData(5,Qt::Horizontal,QStringLiteral("高压设定"));
    model->setHeaderData(6,Qt::Horizontal,QStringLiteral("保压时间"));
    model->setHeaderData(7,Qt::Horizontal,QStringLiteral("允许压降"));
    model->setHeaderData(8,Qt::Horizontal,QStringLiteral("综合压降"));
    model->setHeaderData(9,Qt::Horizontal,QStringLiteral("公扣状态"));
    model->setHeaderData(10,Qt::Horizontal,QStringLiteral("母扣状态"));
    model->setHeaderData(11,Qt::Horizontal,QStringLiteral("油管规格"));
    model->setHeaderData(12,Qt::Horizontal,QStringLiteral("生产编号"));
    model->setHeaderData(13,Qt::Horizontal,QStringLiteral("数据地址"));
    ui->tableView->setModel(model);
}

void findHistory::getrdByid(int id)
{
    m_rdata=new ReportData;
    m_rdata->id=model->index(id,0).data().toInt();
    m_rdata->workFor=model->index(id,1).data().toString();
    m_rdata->jcry=model->index(id,2).data().toString();
    m_rdata->workDay=model->index(id,3).data().toString();
    m_rdata->workTime=model->index(id,4).data().toString();
    m_rdata->setHighPress=model->index(id,5).data().toInt();
    m_rdata->pressTimeLong=model->index(id,6).data().toInt();
    m_rdata->setDifPress=model->index(id,7).data().toInt();
    m_rdata->difPress=model->index(id,8).data().toInt();
    m_rdata->famaleResult=model->index(id,9).data().toInt();
    m_rdata->maleResult=model->index(id,10).data().toInt();
    m_rdata->workType=model->index(id,11).data().toString();
    m_rdata->workId=model->index(id,12).data().toString();
    m_rdata->dataAddress=model->index(id,13).data().toString();
}

void findHistory::on_pbPrint_clicked()
{
    QDir dir(qApp->applicationDirPath());

    QString fileName=dir.absolutePath()+"/totalProduct.xml";
    //    auto report=QtRPT::createSPtr(this);
    auto report=new QtRPT(this);

    if(report->loadReport(fileName)==false){
        qDebug()<<"Report file not found";
    }

    QObject::connect(report, SIGNAL(setValue(const int, const QString, QVariant&, const int)),
                     this, SLOT(setAllValue(const int, const QString, QVariant&, const int)));
    QObject::connect(report, SIGNAL(setDSInfo(DataSetInfo &)),
                     this, SLOT(setAllDSInfo(DataSetInfo &)));
    report->printExec();

}

void findHistory::on_pbChartPrint_clicked()
{
    QDir dir(qApp->applicationDirPath());

    QString fileName=dir.absolutePath()+"/chartAnddata.xml";
    auto report=QtRPT::createSPtr(this);

    if(report->loadReport(fileName)==false){
        qDebug()<<"Report file not found";
    }

    QObject::connect(report.data(), SIGNAL(setValue(const int, const QString, QVariant&, const int)),
                     this, SLOT(setValue(const int, const QString, QVariant&, const int)));

    QObject::connect(report.data(), SIGNAL(setChart(RptFieldObject&, QChart&)),
                     this, SLOT(setChart(RptFieldObject&, QChart&)));
    report->printExec();

}

void findHistory::on_tableView_clicked(const QModelIndex &index)
{
    getrdByid(index.row());
}

void findHistory::setValue(const int recNo, const QString paramName, QVariant &paramValue, const int reportPage)
{
    Q_UNUSED(reportPage)
    Q_UNUSED(recNo)

    if (paramName == "id") {
        paramValue = m_rdata->id;
    }
    if (paramName == "sjdw") {
        paramValue = m_rdata->workFor;
    }
    if (paramName == "jcry") {
        paramValue = m_rdata->jcry;
    }
    if (paramName == "day") {
        paramValue = m_rdata->workDay;
    }
    if (paramName == "time") {
        paramValue = m_rdata->workTime;
    }
    if (paramName == "gysd") {
        paramValue = m_rdata->setHighPress;
    }
    if (paramName == "bysj") {
        paramValue = m_rdata->pressTimeLong;
    }
    if (paramName == "yxyj") {
        paramValue = m_rdata->setDifPress;
    }
    if (paramName == "yajiang") {
        paramValue = m_rdata->difPress;
    }
    if (paramName == "gkjg") {
        paramValue = m_rdata->famaleResult;
    }
    if (paramName == "mkjg") {
        paramValue = m_rdata->maleResult;
    }
    if (paramName == "guige") {
        paramValue = m_rdata->workType;
    }
    if (paramName == "scbh") {
        paramValue = m_rdata->workId;
    }
}

void findHistory::setAllValue(const int recNo, const QString paramName, QVariant &paramValue, const int reportPage)
{
    Q_UNUSED(reportPage)
    getrdByid(recNo);

    if (paramName == "id") {
        paramValue = m_rdata->id;
    }
    if (paramName == "sjdw") {
        paramValue = m_rdata->workFor;
    }
    if (paramName == "jcry") {
        paramValue = m_rdata->jcry;
    }
    if (paramName == "jcrq") {
        paramValue = m_rdata->workDay;
    }
    if (paramName == "jcsj") {
        paramValue = m_rdata->workTime;
    }
    if (paramName == "gysd") {
        paramValue = m_rdata->setHighPress;
    }
    if (paramName == "bysj") {
        paramValue = m_rdata->pressTimeLong;
    }
    if (paramName == "yxyj") {
        paramValue = m_rdata->setDifPress;
    }
    if (paramName == "yajiang") {
        paramValue = m_rdata->difPress;
    }
    if (paramName == "gkzt") {
        paramValue = m_rdata->famaleResult;
    }
    if (paramName == "mkzt") {
        paramValue = m_rdata->maleResult;
    }
    if (paramName == "guige") {
        paramValue = m_rdata->workType;
    }
    if (paramName == "scbh") {
        paramValue = m_rdata->workId;
    }
}

void findHistory::setDSInfo(DataSetInfo &dsInfo)
{
    dsInfo.recordCount=1;       //print chart
}

void findHistory::setAllDSInfo(DataSetInfo &dsInfo)
{
    dsInfo.recordCount =model->rowCount();  //print list
}

void findHistory::setChart(RptFieldObject &fieldObject, QChart &chart)
{

    QFileInfo fileIsHave(m_rdata->dataAddress);
    if(fileIsHave.exists()==true){
        chartData=m_cuse->readFileAllData(m_rdata->dataAddress);
    }else{
        QMessageBox::information(nullptr,"ERROR",QStringLiteral("数据不存在"));
    }

    chart.removeAllSeries();

    if (fieldObject.name == "diagram1") {
        auto series = new QLineSeries();
        series->setProperty("graphDS", "1");

        for (int i = 0; i < chartData.size(); ++i) {
            series->append(i,chartData[i]);
        }
        series->setColor(Qt::red);
        series->setName(QString("Series #%1").arg(chart.series().size()));

        chart.addSeries(series);
        chart.createDefaultAxes();
        chart.axisX()->setTitleText(QString("x [s]"));
        chart.axisY()->setTitleText(QString("y [MPa]"));
        //        chart.setTitle("xxx");
    }

    QFont font=chart.titleFont();
    font.setPointSize(font.pointSize()*2);
    chart.setTitleFont(font);
}
