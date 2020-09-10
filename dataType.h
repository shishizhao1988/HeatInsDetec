#ifndef DATATYPE_H
#define DATATYPE_H

#include <QTimer>

enum heatFlowStep{
    noRun,
    Heat150,
    Heat200,
    Heat250,
    Heat300,
    Heat350,
    waitStop
};

struct SysSetData
{
    QString Name;
    QString Length;
    QString InnerDiam;
    QString OutDiam;
    QString HighTempe;
    QString RateofHeat;
    QString calculateTime;
};

struct lbShowData
{
    double      innerTemp=0.0;
    double      innerSetTemp=0.0;
    double      outterTemp=0.0;
    float       totalPower=0.0;
};

#endif // DATATYPE_H
