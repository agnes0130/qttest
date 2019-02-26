#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"

MainWindow::MainWindow(QWidget *parent) :

    QMainWindow(parent),

    ui(new Ui::MainWindow)

{

    ui->setupUi(this);

    setWindowTitle(tr("TabWidget"));//设置标题

    ui->tabWidget->clear();//清空选项卡



    QWidget *tabSports=new QWidget(this);

    QWidget *tabMusic=new QWidget(this);

    QWidget *tabSoftware=new QWidget(this);

    QWidget *tabDigital=new QWidget(this);

    QWidget *tabLanguage=new QWidget(this);



    ui->tabWidget->setTabPosition(QTabWidget::North);//设置选项卡的方位东南西北，默认在上方



    ui->tabWidget->addTab(tabSports,tr("运动"));//在后面添加选项卡

    ui->tabWidget->addTab(tabMusic,tr("音乐"));

    ui->tabWidget->addTab(tabSoftware,QIcon("F:\\磊神图片\\icons\\1.ico"),tr("软件"));//在后面添加带图标的选项卡

    ui->tabWidget->insertTab(3,tabDigital,tr("数码"));//插入选项卡

    ui->tabWidget->insertTab(4,tabLanguage,QIcon("F:\\磊神图片\\icons\\3.ico"),tr("语言"));//插入带图标的选项卡

    ui->tabWidget->setTabShape(QTabWidget::Triangular);//设置选项卡的形状

    //ui->tabWidget->removeTab(0);//移除选项卡



    ui->tabWidget->setTabIcon(0,QIcon("F:\\磊神图片\\icons\\2.ico"));//设置选项卡图标

    ui->tabWidget->setTabIcon(1,QIcon("F:\\磊神图片\\icons\\4.ico"));//设置选项卡图标

    ui->tabWidget->setTabIcon(3,QIcon("F:\\磊神图片\\icons\\5.ico"));//设置选项卡图标

    ui->tabWidget->setIconSize(QSize(50,25));//设置图标的大小(选项卡的大小也会改变)

    ui->tabWidget->setMovable(true);

    ui->tabWidget->setTabsClosable(true);//在选项卡上添加关闭按钮

    qDebug()<<"第一个选项卡名称："<<ui->tabWidget->tabText(0);//获取选项卡名称

    qDebug()<<"iconSize:"<<ui->tabWidget->iconSize();//获取icon的尺寸



    ui->tabWidget->setTabEnabled(0,false);//禁用选项卡

    ui->tabWidget->setTabEnabled(1,false);

    ui->tabWidget->setTabToolTip(2,tr("Beautiful"));//鼠标悬停弹出提示

    ui->tabWidget->usesScrollButtons();//选项卡太多滚动

    //ui->tabWidget->removeTab(0);//移除选项卡

    //ui->tabWidget->setTabWhatsThis(3,tr("Beautiful Girl"));//不知道干嘛的

    //ui->tabWidget->setTabBarAutoHide(true);//包含少于2个选项卡时会自动隐藏

    //ui->tabWidget->setTabText(0,tr("软件"));//按索引修改选项卡的名称

    //ui->tabWidget->setTabText(1,tr("舞蹈"));

    //ui->tabWidget->setDocumentMode(true);//设置选项卡是否以适合文档页面的模式呈现

    //ui->tabWidget->setElideMode(Qt::ElideLeft);//标签栏中删除文字



    //ui->tabWidget->clear();//清空选项卡

}



MainWindow::~MainWindow()

{

    delete ui;

}
