#include "setting.h"
#include "ui_setting.h"
#include <QDebug>
#include <QMessageBox>

Setting::Setting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Setting)
{
    ui->setupUi(this);
    initUI();
    initAction();

    connect(ui->buttonBox,&QDialogButtonBox::accepted,[this](){
        if(isUseThis) m_settings=&tempSData;
        iniRSettings->beginGroup("Default");
        iniRSettings->setValue("1Name",tempSData.Name);
        iniRSettings->setValue("2OutD",tempSData.OutDiam);
        iniRSettings->setValue("3InnerD",tempSData.InnerDiam);
        iniRSettings->setValue("4HeatL",tempSData.Length);
        iniRSettings->setValue("5HeatTemp",tempSData.HighTempe);
        iniRSettings->setValue("6Rate",tempSData.RateofHeat);
        iniRSettings->endGroup();
        hide();
    });

    tbSelectRow=-1;
    isUseThis=false;
}

Setting::~Setting()
{
    delete ui;
}

SysSetData *Setting::sysSettings() const
{
    return m_settings;
}

void Setting::initUI()
{
    iniRSettings=new QSettings(QApplication::applicationDirPath()+"/config.ini",QSettings::IniFormat);
    iniRSettings->setIniCodec("UTF8");
    QStringList groupList=iniRSettings->childGroups();

    m_model=new QStandardItemModel();
    m_model->setColumnCount(6);
    m_model->setHeaderData(0,Qt::Horizontal,"Name");
    m_model->setHeaderData(1,Qt::Horizontal,"Out D");
    m_model->setHeaderData(2,Qt::Horizontal,"Inner Diameter");
    m_model->setHeaderData(3,Qt::Horizontal,"Heat Length");
    m_model->setHeaderData(4,Qt::Horizontal,"Heat Level");
    m_model->setHeaderData(5,Qt::Horizontal,"Rate of Heat");


    foreach (QString group, groupList) {
        iniRSettings->beginGroup(group);
        QStringList keyList=iniRSettings->childKeys();
        QList<QStandardItem *> itl;
//        foreach (QString key, keyList) {
//            qDebug()<<key<<" ";
//            QStandardItem *it=new QStandardItem(iniRSettings->value(key).toString());
//            itl.append(it);
//        }
        for(const auto& i:keyList){
            QStandardItem *it=new QStandardItem(iniRSettings->value(i).toString());
            itl.append(it);
        }
        m_model->appendRow(itl);
        itl.clear();
        iniRSettings->endGroup();
    }

    ui->tvType->setModel(m_model);
    ui->tvType->setAlternatingRowColors(true);

    m_settings=new SysSetData();
    m_settings->Name=iniRSettings->value("Default/1Name").toString();
    m_settings->Length=iniRSettings->value("Default/4HeatL").toString();
    m_settings->OutDiam=iniRSettings->value("Default/2OutD").toString();
    m_settings->HighTempe=iniRSettings->value("Default/5HeatTemp").toString();
    m_settings->InnerDiam=iniRSettings->value("Default/3InnerD").toString();
    m_settings->RateofHeat=iniRSettings->value("Default/6Rate").toString();

}

void Setting::initAction()
{
    connect(ui->pbTreeviewAdd,&QPushButton::clicked,this,&Setting::addItem);
    connect(ui->actionaddItem,&QAction::triggered,this,&Setting::addItem);
    connect(ui->pbTreeviewDel,&QPushButton::clicked,this,&Setting::deleteItem);
    connect(ui->actiondeleteItem,&QAction::triggered,this,&Setting::deleteItem);

    connect(ui->pbTreeviewChg,&QPushButton::clicked,this,&Setting::changeItem);
    connect(ui->actionalterItem,&QAction::triggered,this,&Setting::changeItem);
    connect(ui->pbUseCur,&QPushButton::clicked,this,&Setting::useCurrentItem);
    connect(ui->actionuseItem,&QAction::triggered,this,&Setting::useCurrentItem);

    connect(ui->tvType,SIGNAL(clicked(const QModelIndex &)),this,SLOT(tableVClk(const QModelIndex &)));

    tbRightMc=new QMenu(this);
    tbRightMc->addAction(ui->actionaddItem);
    tbRightMc->addAction(ui->actionuseItem);
    tbRightMc->addAction(ui->actionalterItem);
    tbRightMc->addAction(ui->actiondeleteItem);

    ui->tvType->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tvType,&QPushButton::customContextMenuRequested,[=](const QPoint &pos)
    {
        tbRightMc->exec(QCursor::pos());
    });
}

void Setting::addItem()
{
    saveToSsd();

    int insertRow=m_model->rowCount(QModelIndex());
    m_model->insertRow(insertRow);
    m_model->setItem(insertRow,0,new QStandardItem(tempSData.Name));
    m_model->setItem(insertRow,1,new QStandardItem(tempSData.Length));
    m_model->setItem(insertRow,2,new QStandardItem(tempSData.InnerDiam));
    m_model->setItem(insertRow,3,new QStandardItem(tempSData.OutDiam));
    m_model->setItem(insertRow,4,new QStandardItem(tempSData.HighTempe));
    m_model->setItem(insertRow,5,new QStandardItem(tempSData.RateofHeat));

}

void Setting::deleteItem()
{
    if(tbSelectRow<0){
        QMessageBox::information(nullptr, "warning", "No select ");
        return;
    }
    iniRSettings->remove(tempSData.Name);
    m_model->removeRows(tbSelectRow,1);
}

void Setting::changeItem()
{
    saveToSsd();

    m_model->item(tbSelectRow,0)->setData(tempSData.Name,Qt::DisplayRole);
    m_model->item(tbSelectRow,1)->setData(tempSData.Length,Qt::DisplayRole);
    m_model->item(tbSelectRow,2)->setData(tempSData.InnerDiam,Qt::DisplayRole);
    m_model->item(tbSelectRow,3)->setData(tempSData.OutDiam,Qt::DisplayRole);
    m_model->item(tbSelectRow,4)->setData(tempSData.HighTempe,Qt::DisplayRole);
    m_model->item(tbSelectRow,5)->setData(tempSData.RateofHeat,Qt::DisplayRole);
}

void Setting::useCurrentItem()
{
    isUseThis=!isUseThis;
    ui->pbUseCur->setStyleSheet(isUseThis?"color: rgb(255, 0, 127);":"");
}

void Setting::saveToSsd()
{
    if(ui->leName->text()==nullptr){
        QMessageBox::information(nullptr, "warning", "Name Not Define");
        return;
    }
    tempSData.Name=ui->leName->text();
    tempSData.Length=ui->sbHeatLength->text();
    tempSData.OutDiam=ui->sbOutDiam->text();
    tempSData.InnerDiam=ui->sbInnerDiam->text();
    tempSData.RateofHeat=ui->dsbHeatRate->text();
    tempSData.HighTempe=ui->sbHighTemp->text();

    iniRSettings->beginGroup(tempSData.Name);
    iniRSettings->setValue("1Name",tempSData.Name);
    iniRSettings->setValue("2OutD",tempSData.OutDiam);
    iniRSettings->setValue("3InnerD",tempSData.InnerDiam);
    iniRSettings->setValue("4HeatL",tempSData.Length);
    iniRSettings->setValue("5HeatTemp",tempSData.HighTempe);
    iniRSettings->setValue("6Rate",tempSData.RateofHeat);
    iniRSettings->endGroup();
}

void Setting::tableVClk(const QModelIndex &index)
{
    tbSelectRow=index.row();
    ui->leName->setText(m_model->item(index.row())->text());
    ui->sbOutDiam->setValue( m_model->item(index.row(),1)->text().toInt());
    ui->sbInnerDiam->setValue( m_model->item(index.row(),2)->text().toInt());
    ui->sbHeatLength->setValue( m_model->item(index.row(),3)->text().toInt());
    ui->sbHighTemp->setValue( m_model->item(index.row(),4)->text().toInt());
    ui->dsbHeatRate->setValue( m_model->item(index.row(),5)->text().toInt());

}
