#include "mainwindow.h"
#include "ui_mainwindow.h"
#pragma execution_character_set("utf-8")

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
    setWindowTitle(tr("����ѹ��ʵ���¼��������"));//���ñ���
//    QWidget *tabRecord=new QWidget(this);
//    QWidget *tabAnalyse=new QWidget(this);

    ui->tabWidget->setTabPosition(QTabWidget::North);//����ѡ��ķ�λ����������Ĭ�����Ϸ�
    ui->tabWidget->setTabShape(QTabWidget::Triangular);//����ѡ�����״

    ui->tabWidget->setMovable(true);
    qDebug()<<"��һ��ѡ����ƣ�"<<ui->tabWidget->tabText(0);//��ȡѡ�����
    qDebug()<<"iconSize:"<<ui->tabWidget->iconSize();//��ȡicon�ĳߴ�

    ui->tabWidget->setTabToolTip(0,tr("Data Record"));//�����ͣ������ʾ
    ui->tabWidget->setTabToolTip(1,tr("Data Analyse"));

    plot=new QCustomPlot(ui->tab);
    plot->setGeometry(10,290,400,350);
    plot->xAxis->setLabel("time/s");
    plot->yAxis->setLabel("Press/N");
    plot->addGraph();

    plot2=new QCustomPlot(ui->tab);
    plot2->setGeometry(450,290,400,350);
    plot2->xAxis->setLabel("time/s");
    plot2->yAxis->setLabel("Position/mm");
    plot2->addGraph();

    plotAll = new QCustomPlot(ui->tab_2);
    plotAll->setGeometry(100,130,600,500);
    plotAll->xAxis->setLabel("time/s");
    plotAll->yAxis->setLabel("Press/N");
    plotAll->yAxis2->setLabel("Position/mm");
    plotAll->yAxis2->setVisible(true);
    plotAll->addGraph();

    serialPort= new QSerialPort(this);//���ڲ���
    foreach(const QSerialPortInfo &SerialPortInfo, QSerialPortInfo::availablePorts())
    {
        ui->serialPort->addItem(SerialPortInfo.portName());
    }
    connect(serialPort,SIGNAL(readyRead()),this,SLOT(readCom()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    ui->comboSize->addItem("50 mm2");
    ui->comboSize->addItem("70 mm2");
    ui->comboSize->addItem("120 mm2");
    ui->comboSize->addItem("150 mm2");
    ui->comboSize->addItem("240 mm2");
    ui->comboSize->addItem("300 mm2");
    ui->comboSize->addItem("400 mm2");
    ui->comboTurn->addItem(tr("��������"));
    ui->comboTurn->addItem(tr("��������"));
    ui->comboNumber->addItem("4");
    ui->comboNumber->addItem("6");
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
        out << tr("���³ߴ磺") << size_str << endl;
        if (turn_index == 0)
            out << tr("ѹ��˳����������") << endl;
        else if (turn_index == 1)
            out << tr("ѹ��˳����������") << endl;
        if (number_index == 0)
            out << tr("ѹ�ӵ�����4") << endl;
        else if (number_index == 1)
            out << tr("ѹ�ӵ�����6") << endl;
        out << tr("ѹ����ʼ���룺") << first_str << " mm" << endl;
        out << tr("ѹ�ۼ�ࣺ") << gap_str << " mm" << endl;
        out << "****************************" << endl;
        out << tr("ѹ��ʵ������       λ��ʵ������") <<endl;
        out << tr("ʱ��/s  ѹ��/N     ʱ��/s  λ��/mm") <<endl;
        file.close();
    }
}

void MainWindow::on_pressButton_clicked()
{
    QString time_str = ui->timeEdit->toPlainText();
    QString size_str = ui->comboSize->currentText();
    QString first_str = ui->lineFirst->text();
    QString gap_str = ui->lineGap->text();
    QString standard_str = ui->textEdit->toPlainText();
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
        out << tr("*****����ǿ�Ȳ��Խ����¼��*****") << endl;
        out << time_str << endl;
        out << tr("���³ߴ磺") << size_str << " mm2" << endl;
        if (turn_index == 0)
            out << tr("ѹ��˳����������") << endl;
        else if (turn_index == 1)
            out << tr("ѹ��˳����������") << endl;
        if (number_index == 0)
            out << tr("ѹ�ӵ�����4") << endl;
        else if (number_index == 1)
            out << tr("ѹ�ӵ�����6") << endl;
        out << tr("ѹ����ʼ���룺") << first_str << " mm" << endl;
        out << tr("ѹ�ۼ�ࣺ") << gap_str << " mm" << endl;
        out << "*********************" << endl;
        out << tr("���ұ�׼") << standard_str << endl;
        out << tr("ѹ��/N   ����ǿ��/N") << endl;
        QVector<QString>::iterator iter;
        QVector<QString>::iterator iter2;
        iter2=pullData.begin();
        for (iter=pullPress.begin();iter!=pullPress.end();iter++)
        {
            out <<  *iter << "    ";
            out << *iter2 << "\n";
            iter2++;
        }
        file.close();
    }
}

void MainWindow::on_pullConfirm_clicked()
{
    QString pull_press = ui->linePress->text();
    QString pull_data = ui->linePull->text();
    pullPress.append(pull_press);
    pullData.append(pull_data);
}

void MainWindow::on_resistConfirm_clicked()
{
    QString resist_press = ui->linePressResist->text();
    QString resist_data = ui->lineResist->text();
    resistPress.append(resist_press);
    resistData.append(resist_data);
}

void MainWindow::on_resistButton_clicked()
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
        out << tr("*****�Ӵ�������Խ����¼��*****") << endl;
        out << time_str << endl;
        out << tr("���³ߴ磺") << size_str << " mm2" << endl;
        if (turn_index == 0)
            out << tr("ѹ��˳����������") << endl;
        else if (turn_index == 1)
            out << tr("ѹ��˳����������") << endl;
        if (number_index == 0)
            out << tr("ѹ�ӵ�����4") << endl;
        else if (number_index == 1)
            out << tr("ѹ�ӵ�����6") << endl;
        out << tr("ѹ����ʼ���룺") << first_str << " mm" << endl;
        out << tr("ѹ�ۼ�ࣺ") << gap_str << " mm" << endl;
        out << "*********************" << endl;
        out << tr("ѹ��/N   �Ӵ�����/��") << endl;
        QVector<QString>::iterator iter;
        QVector<QString>::iterator iter2;
        iter2=resistData.begin();
        for (iter=resistPress.begin();iter!=resistPress.end();iter++)
        {
            out <<  *iter << "    ";
            out << *iter2 << "\n";
            iter2++;
        }
        file.close();
    }
}

void MainWindow::on_serialOpen_clicked()
{
    serialPort->setPortName(ui->serialPort->currentText());
    serialPort->setBaudRate(QSerialPort::Baud115200);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);
    if(serialPort->open(QIODevice::ReadWrite))
    {
         ui->serialOpen->setDisabled(true);
         ui->serialClose->setEnabled(true);
         ui->serialPort->setDisabled(true);
         ui->serialShow->setPlainText(tr("�����Ѵ�"));
         qDebug()<<"���ڴ򿪳ɹ���";
    }
    else if(!serialPort->open(QIODevice::ReadWrite))
    {
        ui->serialShow->setPlainText(tr("���ڴ�ʧ��"));
        QMessageBox::information(this,"abc","���ڴ�ʧ�ܣ�");
    }
}

void MainWindow::on_serialClose_clicked()
{
    if(serialPort->isOpen())
    {
        serialPort->close();
        ui->serialOpen->setEnabled(true);
        ui->serialClose->setDisabled(true);
        ui->serialPort->setEnabled(true);

        ui->serialShow->setPlainText(tr("�����ѹر�"));
        qDebug()<<"�����ѹرգ�";
    }
}

void MainWindow::on_pressFile_clicked()
{
    QFile file;
    QString f = QFileDialog::getOpenFileName(this, QString("ѡ���ļ�"), QString("/"),QString("TEXT(*.txt)"));
    ui->pressDir->setPlainText(f);
    //QString fileName =
    file.setFileName(f);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray t ;
        while(!file.atEnd())
        {
            t += file.readLine();
        }
        //ui->text_r->setText(QString(t));//�洢����
        file.close();
    }
}

void MainWindow::on_positionFile_clicked()
{
    QFile file;
    QString f = QFileDialog::getOpenFileName(this, QString("ѡ���ļ�"), QString("/"),QString("TEXT(*.txt)"));
    ui->positionDir->setPlainText(f);
    //QString fileName =
    file.setFileName(f);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray t ;
        while(!file.atEnd())
        {
            t += file.readLine();
        }
        //ui->text_r->setText(QString(t));//�洢����
        file.close();
    }
}
