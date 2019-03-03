#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    dataRecord = new QTabWidget(this);
    dataAnalyse = new QTabWidget(this);
    QWidget* dataRecordTabObj = new dataRecordTab(this);
    QWidget* dataAnalyseTabObj = new dataAnalyseTab(this);

    dataRecord->insertTab(0,dataRecordTabObj,"dataRecord");
    dataAnalyse->insertTab(1,dataAnalyseTabObj,"dataRecord");

    dataRecord->setGeometry(50,50,500,350);
    dataAnalyse->setGeometry(10,50,500,350);
}

MainWindow::~MainWindow()
{
    delete ui;
}
