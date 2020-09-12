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
    m_rdata=new ReportData();
    grfAddress=QApplication::applicationDirPath();
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->cwStart->setSelectedDate(QDate::currentDate().addDays(-10));
    ui->cwEnd->setSelectedDate(QDate::currentDate());
    connect(ui->cwStart,&QCalendarWidget::selectionChanged,this,&findHistory::calClickIsQualified);
    connect(ui->cwEnd,&QCalendarWidget::selectionChanged,this,&findHistory::calClickIsQualified);

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
            .arg(ui->cwStart->selectedDate().year())
            .arg(ui->cwStart->selectedDate().month(),2,10,QLatin1Char('0'))
            .arg(ui->cwStart->selectedDate().day(),2,10,QLatin1Char('0'))
            .arg(ui->cwEnd->selectedDate().year())
            .arg(ui->cwEnd->selectedDate().month(),2,10,QLatin1Char('0'))
            .arg(ui->cwEnd->selectedDate().day(),2,10,QLatin1Char('0'));
    model=new QSqlQueryModel;
    model->setQuery(sqlFindStr,database);
    updateTableView();
}

void findHistory::updateTableView()
{
    model->setHeaderData(0,Qt::Horizontal,QStringLiteral("序号"));
    model->setHeaderData(1,Qt::Horizontal,QStringLiteral("生产编号"));
    model->setHeaderData(2,Qt::Horizontal,QStringLiteral("日期"));
    model->setHeaderData(3,Qt::Horizontal,QStringLiteral("时间"));
    model->setHeaderData(4,Qt::Horizontal,QStringLiteral("名称"));
    model->setHeaderData(5,Qt::Horizontal,QStringLiteral("加热效率"));
    model->setHeaderData(6,Qt::Horizontal,QStringLiteral("高温设定"));
    model->setHeaderData(7,Qt::Horizontal,QStringLiteral("内径"));
    model->setHeaderData(8,Qt::Horizontal,QStringLiteral("外径"));
    model->setHeaderData(9,Qt::Horizontal,QStringLiteral("加热长度"));
    model->setHeaderData(10,Qt::Horizontal,QStringLiteral("数据"));

    ui->tableView->setModel(model);
}

void findHistory::getrdByid(int id)
{
    m_rdata=new ReportData;
    m_rdata->id=model->index(id,0).data().toInt();
    m_rdata->productId=model->index(id,1).data().toString();
    m_rdata->day=model->index(id,2).data().toString();
    m_rdata->time=model->index(id,3).data().toString();
    m_rdata->name=model->index(id,4).data().toString();
    m_rdata->heatRatio=model->index(id,5).data().toInt();
    m_rdata->highT=model->index(id,6).data().toInt();
    m_rdata->innerC=model->index(id,7).data().toInt();
    m_rdata->outterC=model->index(id,8).data().toInt();
    m_rdata->heatL=model->index(id,9).data().toInt();
    m_rdata->dataAddress=model->index(id,10).data().toString();

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

    if (paramName == "productId") {
        paramValue = m_rdata->productId;
    }
    if (paramName == "day") {
        paramValue = m_rdata->day;
    }
    if (paramName == "time") {
        paramValue = m_rdata->time;
    }
    if (paramName == "name") {
        paramValue = m_rdata->name;
    }
    if (paramName == "heatRatio") {
        paramValue = m_rdata->heatRatio;
    }
    if (paramName == "highT") {
        paramValue = m_rdata->highT;
    }
    if (paramName == "innerC") {
        paramValue = m_rdata->innerC;
    }
    if (paramName == "outterC") {
        paramValue = m_rdata->outterC;
    }
    if (paramName == "heatL") {
        paramValue = m_rdata->heatL;
    }
    if (paramName == "dataAddress") {
        paramValue = m_rdata->dataAddress;
    }
}

void findHistory::setAllValue(const int recNo, const QString paramName, QVariant &paramValue, const int reportPage)
{
    Q_UNUSED(reportPage)
    getrdByid(recNo);


    if (paramName == "productId") {
        paramValue = m_rdata->productId;
    }
    if (paramName == "day") {
        paramValue = m_rdata->day;
    }
    if (paramName == "time") {
        paramValue = m_rdata->time;
    }
    if (paramName == "name") {
        paramValue = m_rdata->name;
    }
    if (paramName == "heatRatio") {
        paramValue = m_rdata->heatRatio;
    }
    if (paramName == "highT") {
        paramValue = m_rdata->highT;
    }
    if (paramName == "innerC") {
        paramValue = m_rdata->innerC;
    }
    if (paramName == "outterC") {
        paramValue = m_rdata->outterC;
    }
    if (paramName == "heatL") {
        paramValue = m_rdata->heatL;
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
        chartData=readFileAllData(m_rdata->dataAddress);
    }else{
        QMessageBox::information(nullptr,"ERROR",QStringLiteral("数据不存在"));
    }

    auto series1 = new QLineSeries();
    auto series2 = new QLineSeries();
    auto series3 = new QLineSeries();

    series1->setProperty("graphDS", "1");
    series2->setProperty("graphDS", "1");
    series3->setProperty("graphDS", "1");

    for (int i = 0; i < chartData.size(); ++i) {
        if(chartData[i].size()==3){
            series1->append(i,chartData[i][0]);
            series2->append(i,chartData[i][1]);
            series3->append(i,chartData[i][2]);
        }
    }
    series1->setColor(Qt::red);
    series2->setColor(Qt::blue);
    series3->setColor(Qt::black);

    series1->setName(QString("Series #%1").arg(chart.series().size()));

    if (fieldObject.name == "diagram1") {
        chart.removeAllSeries();
        chart.addSeries(series1);
        chart.createDefaultAxes();
        chart.axisX()->setTitleText(QString("x [s]"));
        chart.axisY()->setTitleText(QString("y [C]"));
    }

    if (fieldObject.name == "diagram2") {
        chart.removeAllSeries();
        chart.addSeries(series2);
        chart.createDefaultAxes();
        chart.axisX()->setTitleText(QString("x [s]"));
        chart.axisY()->setTitleText(QString("y [C]"));
    }
    if (fieldObject.name == "diagram3") {
        chart.removeAllSeries();
        chart.addSeries(series3);
        chart.createDefaultAxes();
        chart.axisX()->setTitleText(QString("x [s]"));
        chart.axisY()->setTitleText(QString("y [C]"));
    }

    QFont font=chart.titleFont();
    font.setPointSize(font.pointSize()*2);
    chart.setTitleFont(font);
}

void findHistory::calClickIsQualified()
{
    if(ui->cwStart->selectedDate()>QDate::currentDate()){
        QMessageBox::warning(this,tr("no work"),tr("this day is future"));
        ui->cwStart->setSelectedDate(QDate::currentDate());
    }

    if(ui->cwEnd->selectedDate()>QDate::currentDate()){
        QMessageBox::warning(this,tr("no work"),tr("this day is future"));
        ui->cwEnd->setSelectedDate(QDate::currentDate());
    }

    if(ui->cwStart>ui->cwEnd) {
        QMessageBox::warning(this,tr("Error"),tr("daily record of work must START day to END day"));
        ui->cwEnd->setSelectedDate(ui->cwStart->selectedDate().addDays(7));
    }
}
