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

#include "commenuse.h"
#include "qtrpt.h"
#include "report/chart.h"

using namespace std;

namespace Ui {
class findHistory;
}

struct ReportData
{
    int id;
    QString workFor;
    QString workType;
    QString workId;
    QString workDay;
    QString workTime;
    QString jcry;
    int     setHighPress;
    int     setDifPress;
    int     difPress;
    int     pressTimeLong;

    QString famaleResult;
    QString maleResult;
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
    vector<double> chartData;
    ReportData *m_rdata;

private:
    void updateTableView();
    void getrdByid(int id);
    commenUse *m_cuse;

};

#endif // FINDHISTORY_H
