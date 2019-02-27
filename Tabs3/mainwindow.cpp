#include "mainwindow.h"
#include "ui_mainwindow.h"
#pragma execution_character_set("utf-8")

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
    ui->comboTurn->addItem(tr("由内向外"));
    ui->comboTurn->addItem(tr("由外向内"));
    ui->comboNumber->addItem("4");
    ui->comboNumber->addItem("6");
}



void MainWindow::on_lineEdit_3_editingFinished()
{

}

void MainWindow::on_lineEdit_4_editingFinished()
{

}

void MainWindow::on_comboSize_currentIndexChanged(int index)
{
    switch (index)
    {
    case 0:
        ui->textEdit->setPlainText("3000N");
        break;

    case 1:
        ui->textEdit->setPlainText("4200N");
        break;

    case 2:
        ui->textEdit->setPlainText("7200N");
        break;

    case 3:
        ui->textEdit->setPlainText("9000N");
        break;

    case 4:
        ui->textEdit->setPlainText("14400N");
        break;

    case 5:
        ui->textEdit->setPlainText("18000N");
        break;

    case 6:
        ui->textEdit->setPlainText("24000N");
        break;
    }
}

void MainWindow::on_timeButton_clicked()
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy.MM.dd hh:mm");
    ui->timeEdit->setPlainText(current_date);
}

void MainWindow::on_pushButton_clicked()
{
    base_information_save();

}

void MainWindow::base_information_save()
{
    QString time_str = ui->timeEdit->toPlainText();
    QString size_str = ui->comboSize->currentText();
    QString first_str = ui->lineFirst->text();
    QString gap_str = ui->lineGap->text();
    int turn_index = ui->comboTurn->currentIndex();
    int number_index = ui->comboNumber->currentIndex();
    QString path = QFileDialog::getSaveFileName(this,
                                                tr("Open File"),
                                                ".",
                                                tr("Text Files(*.txt)"));
    if(!path.isEmpty())
    {
        QFile file(path);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QMessageBox::warning(this,
                                 tr("Write File"),
                                 tr("Cannot open file:\n%1").arg(path));
            return;
        }
        QTextStream out(&file);
        out << time_str << endl;
        out << tr("电缆尺寸：") << size_str << " mm2" << endl;
        if (turn_index == 0)
            out << tr("压接顺序：由内向外") << endl;
        else if (turn_index == 1)
            out << tr("压接顺序：由外向内") << endl;
        if (number_index == 0)
            out << tr("压接点数：4") << endl;
        else if (number_index == 1)
            out << tr("压接点数：6") << endl;
        out << tr("压痕起始距离：") << first_str << " mm" << endl;
        out << tr("压痕间距：") << gap_str << " mm" << endl;
        file.close();
    }
}
