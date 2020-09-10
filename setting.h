#ifndef SETTING_H
#define SETTING_H

#include <QDialog>
#include <QSettings>
#include <QStandardItemModel>
#include <QMenu>
#include "dataType.h"

namespace Ui {
class Setting;

}

class Setting : public QDialog
{
    Q_OBJECT


public:

    explicit Setting(QWidget *parent = nullptr);
    ~Setting();


    SysSetData* sysSettings() const;
private:

    void initUI();
    void initAction();
    void addItem();
    void deleteItem();
    void changeItem();
    void useCurrentItem();
    void saveToSsd();
private slots:
    void tableVClk(const QModelIndex &index);
private:
    Ui::Setting *ui;
    SysSetData *m_settings,tempSData;
    QSettings *iniRSettings;

    QStandardItemModel *m_model;
    QMenu *tbRightMc;

    int tbSelectRow;
    bool isUseThis;
};

#endif // SETTING_H
