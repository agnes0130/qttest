﻿#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
    setWindowTitle(tr("TabWidget"));//设置标题
//    QWidget *tabRecord=new QWidget(this);
//    QWidget *tabAnalyse=new QWidget(this);

    ui->tabWidget->setTabPosition(QTabWidget::North);//设置选项卡的方位东南西北，默认在上方
    ui->tabWidget->setTabShape(QTabWidget::Triangular);//设置选项卡的形状

    ui->tabWidget->setMovable(true);
    qDebug()<<"第一个选项卡名称："<<ui->tabWidget->tabText(0);//获取选项卡名称
    qDebug()<<"iconSize:"<<ui->tabWidget->iconSize();//获取icon的尺寸

    ui->tabWidget->setTabToolTip(0,tr("Data Record"));//鼠标悬停弹出提示
    ui->tabWidget->setTabToolTip(1,tr("Data Analyse"));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    ui->comboSize->addItem("50");
    ui->comboSize->addItem("70");
    ui->comboSize->addItem("120");
    ui->comboSize->addItem("150");
    ui->comboSize->addItem("240");
    ui->comboSize->addItem("300");
    ui->comboSize->addItem("400");
    ui->comboBox_2->addItem(QString::fromLocal8Bit("由内向外"));
    ui->comboBox_2->addItem(QString::fromLocal8Bit("由外向内"));
    ui->comboBox_3->addItem("4");
    ui->comboBox_3->addItem("6");
}

void MainWindow::on_comboSize_currentTextChanged(const QString &arg1)
{

}
