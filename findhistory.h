#ifndef FINDHISTORY_H
#define FINDHISTORY_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlError>
#include <QtSql/QSqlQueryModel>
#include <QtSql/QSqlDatabase>
#include <QSqlQueryModel>

#include <QDateTime>
#include <QPicture>
#include <vector>

//#include "commenuse.h"
#include "qtrpt.h"
#include "report/chart.h"

using namespace std;

namespace Ui {
class findHistory;
}

using  vvd=vector<vector<double>> ;

struct ReportData
{
    int id;
    QString productId;
    QString day;
    QString time;
    QString name;
    int heatRatio;
    int highT;
    int innerC;
    int     outterC;
    int     heatL;
    QString dataAddress;

};

class findHistory : public QDialog
{
    Q_OBJECT

public:
    explicit findHistory(QWidget *parent = nullptr);
    ~findHistory();

private slots:
    void on_pbFindShowAll_clicked();

    void on_pbFindByDay_clicked();

    void on_pbPrint_clicked();

    void on_pbChartPrint_clicked();

    void on_tableView_clicked(const QModelIndex &index);

    void setValue(const int recNo, const QString paramName, QVariant &paramValue, const int reportPage);
    void setAllValue(const int recNo, const QString paramName, QVariant &paramValue, const int reportPage);

    void setDSInfo(DataSetInfo &dsInfo);
    void setAllDSInfo(DataSetInfo &dsInfo);

    void setChart(RptFieldObject &fieldObject,QChart &chart);

private:
    Ui::findHistory *ui;
    QSqlQueryModel *model;
    QSqlDatabase database;

    QString grfAddress;
    vvd chartData;
    ReportData *m_rdata;

private:
    void updateTableView();
    void getrdByid(int id);
    vvd readFileAllData(QString fileAddr){
        QFile fileIn(fileAddr);
        vvd dReaddata;
        if(!fileIn.open(QIODevice::ReadOnly)){
            qDebug()<<"Error";
            return dReaddata;
        }
        QTextStream in(&fileIn);

        while (!in.atEnd()) {
            QVector<QString> strcolData= in.readLine().split(",").toVector();
            vector<double> dcolData;
            std::transform(strcolData.begin(),strcolData.end(),std::back_inserter(dcolData),
                           [](const QString& str){return str.toDouble();});
            dReaddata.push_back(dcolData);
        }
        fileIn.close();
        return dReaddata;
    }
    //    commenUse *m_cuse;

};

#endif // FINDHISTORY_H
