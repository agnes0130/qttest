#include "mainwindow.h"
#include "ui_mainwindow.h"
#pragma execution_character_set("utf-8")
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
    am_obj = new am(this);
    setWindowTitle(tr("电缆压接实验记录与分析软件"));//设置标题
    ui->tabWidget->setTabPosition(QTabWidget::North);//设置选项卡的方位东南西北，默认在上方
    ui->tabWidget->setTabShape(QTabWidget::Triangular);//设置选项卡的形状
    ui->tabWidget->setMovable(true);
    qDebug()<<"第一个选项卡名称："<<ui->tabWidget->tabText(0);//获取选项卡名称
    qDebug()<<"iconSize:"<<ui->tabWidget->iconSize();//获取icon的尺寸
    ui->tabWidget->setTabToolTip(0,tr("Data Record"));//鼠标悬停弹出提示
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
    plot3=new QCustomPlot(ui->tab);
    plot3->setGeometry(900,10,600,500);
    plot3->xAxis->setLabel("press/kgf");
    plot3->yAxis->setLabel("Position/mm");
    plot3->addGraph();
    plotAll = new QCustomPlot(ui->tab_2);
    plotAll->setGeometry(100,130,600,500);
    plotAll->xAxis->setLabel("time/s");
    plotAll->yAxis->setLabel("Press/N");
    plotAll->yAxis2->setLabel("Position/mm");
    plotAll->yAxis2->setVisible(true);
    plotAll->addGraph();
    serialPort= new QSerialPort(this);//串口部分
    foreach(const QSerialPortInfo &SerialPortInfo, QSerialPortInfo::availablePorts())
    {
        ui->serialBox->addItem(SerialPortInfo.portName());
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
    ui->comboTurn->addItem(tr("由内向外"));
    ui->comboTurn->addItem(tr("由外向内"));
    ui->comboNumber->addItem("4");
    ui->comboNumber->addItem("6");
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
    QString model_str = ui->modelEdit->text();
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
        out << tr("电缆尺寸：") << size_str << endl;
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
        out << tr("模具内边直径：") << model_str << " mm" << endl;
        out << "****************************" << endl;
        out << tr("压力实验数据       位移实验数据") <<endl;
        out << tr("时间/s  压力/N     时间/s  位移/mm") <<endl;
        QVector<double>::iterator iter;
        QVector<int>::iterator iter2;
        QVector<double>::iterator iter3;
        iter2=pressSave.begin();
        iter3=disSave.begin();
        for (iter=timeSave.begin();iter!=timeSave.end();iter++)
        {
            out <<  *iter << "\t";
            out << *iter2 << "\t";
            out <<  *iter << "\t";
            out << *iter3 << "\n";
            iter2++;
            iter3++;
        }
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
    QString model_str = ui->modelEdit->text();
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
        out << tr("*****抗拉强度测试结果记录单*****") << endl;
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
        out << tr("模具内边直径：") << model_str << " mm" << endl;
        out << "*********************" << endl;
        out << tr("国家标准") << standard_str << endl;
        out << tr("压力/N   抗拉强度/N") << endl;
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
    QString model_str = ui->modelEdit->text();
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
        out << tr("*****接触电阻测试结果记录单*****") << endl;
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
        out << tr("模具内边直径：") << model_str << " mm" << endl;
        out << "*********************" << endl;
        out << tr("压力/N   接触电阻/Ω") << endl;
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
    serialPort->setPortName(ui->serialBox->currentText());
    serialPort->setBaudRate(QSerialPort::Baud9600);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);
    if(serialPort->open(QIODevice::ReadWrite))
    {
         ui->serialOpen->setDisabled(true);
         ui->serialClose->setEnabled(true);
         ui->serialBox->setDisabled(true);
         ui->serialShow->setPlainText(tr("串口已打开"));
         //qDebug()<<"串口打开成功！";
    }
    else if(!serialPort->open(QIODevice::ReadWrite))
    {
        ui->serialShow->setPlainText(tr("串口打开失败"));
        QMessageBox::information(this,"abc","串口打开失败！");
    }
}
void MainWindow::on_serialClose_clicked()
{
    if(serialPort->isOpen())
    {
        serialPort->close();
        ui->serialOpen->setEnabled(true);
        ui->serialClose->setDisabled(true);
        ui->serialBox->setEnabled(true);

        ui->serialShow->setPlainText(tr("串口已关闭"));
        //qDebug()<<"串口已关闭！";
    }
}
void MainWindow::on_pressFile_clicked()
{
    QFile file;
    QString f = QFileDialog::getOpenFileName(this, QString("选择文件"), QString("/"),QString("TEXT(*.txt)"));
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
        //ui->text_r->setText(QString(t));//存储内容
        file.close();
    }
}
void MainWindow::on_positionFile_clicked()
{
    QFile file;
    QString f = QFileDialog::getOpenFileName(this, QString("选择文件"), QString("/"),QString("TEXT(*.txt)"));
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
        //ui->text_r->setText(QString(t));//存储内容
        file.close();
    }
}
void MainWindow::on_startButton_clicked()
{
    pressData.clear();
}
void MainWindow::readCom()
{
#ifndef AM_RE_TX_ENABLE
    byteArray.append(serialPort->readAll());

    int index = 0;
    int arraySize = byteArray.size();

    if(startFlag==0){
        byteArray.clear();
        startFlag++;
    }
    else{
        for(index = 0; index < arraySize; index++){
            if(byteArray[index]=='A')
            {
                if(byteArray[index + 1] !='A'){
                    char rxDataArray[5];
                    index++;
                    for(int i =0; i < 5; i++){
                        //rxDataArray[i]=byteArray[index];
                        rxDataArray[i]=byteArray[index]-'0';
                        //qDebug()<<rxDataArray[i];
                        index++;
                    }
                    int hereData;
                    if(rxDataArray[0] == -3){
                        hereData=-(1000*rxDataArray[1] + 100*rxDataArray[2] + 10*rxDataArray[3] + rxDataArray[4]);
                    }
                    else{
                        hereData = 10000*rxDataArray[0] + 1000*rxDataArray[1] + 100*rxDataArray[2] + 10*rxDataArray[3] + rxDataArray[4];
                    }
        //            char hereData='0'+17;
                   //
                    //qDebug() << hereData << endl;
                    dataPlot(hereData);
                    //pressData.append(hereData);
                }
            }
            if(byteArray[index]=='B')
            {
                if(byteArray[index + 1] !='B'){
                    char rxDataArray[5];
                    index++;
                    for(int i =0; i < 5; i++){
                        //rxDataArray[i]=byteArray[index];
                        rxDataArray[i]=byteArray[index]-'0';
                        //qDebug()<<rxDataArray[i];
                        index++;
                    }
                    int hereData;
                    if(rxDataArray[0] == -3){
                        hereData=-(1000*rxDataArray[1] + 100*rxDataArray[2] + 10*rxDataArray[3] + rxDataArray[4]);
                    }
                    else{
                        hereData = 10000*rxDataArray[0] + 1000*rxDataArray[1] + 100*rxDataArray[2] + 10*rxDataArray[3] + rxDataArray[4];
                    }
        //            char hereData='0'+17;
                    double hereData1;
                    hereData1 = -hereData*0.006 + 65;
                    //qDebug() << hereData1 << endl;
                    dataPlot2(hereData1);
                    dataPlot3();
                    //pressData.append(hereData);
                }
            }
    }
    }
    //qDebug()<<byteArray<<endl;
    byteArray.clear();
#else
    uint8_t pdata[PACKET_SIZE + 4];
    uint8_t pdata_out[ACK_DATA_SIZE + 2];
    int data_is_ok;
    int is_re_tx_flag = 0;
    byteArray.append(serialPort->readAll());
    int i = 0;
    int arraySize = byteArray.size();
    if ((byteArray[i] == 0xFF) && ())
    {
        //TODO read the data packet from stm32 to fill pdata and send to am
    }
    am_obj->am_check_data(&pdata[0], &pdata_out[0], &data_is_ok, &is_re_tx_flag);
    //TODO send pdata_out to stm32

    if (data_is_ok == CRC_OK)
    {
        //update rx window and save data
        DATA_T0_SAVE_T *head = NULL;
        am_obj->update_rx_window_and_save_data(&pdata[0], PACKET_SIZE, head);
        save_data_to_txt(head);
        am_obj->delete_node_mem_after_save_data(head);

        if (is_re_tx_flag == 0)
        {
            // TODO PLOT
        }
    }
    else
    {
        //do nothing
    }

#endif
}
void MainWindow::dataPlot(int hereData)
{
    if(intoPlotFuncNum<plotDataNum)
    {
        posDataVec1.append(hereData);
        pressSave.append(hereData);
        timeDataVec.append(intoPlotFuncNum*0.1);
        timeSave.append(intoPlotFuncNum*0.1);
    }
    else
    {
        posDataVec1.removeFirst();
        timeDataVec.removeFirst();
        posDataVec1.append(hereData);
        pressSave.append(hereData);

        timeDataVec.append(intoPlotFuncNum*0.1);
        timeSave.append(intoPlotFuncNum*0.1);
    }
    intoPlotFuncNum++;
    plot->graph(0)->setData(timeDataVec,posDataVec1);
    plot->rescaleAxes(true);
    plot->replot();
}

unsigned char MainWindow::crc8_calcluate(unsigned char *pdata, int len, unsigned char init_crc)
{
    unsigned char byte = 0;
    while(len--)
    {
        byte = init_crc ^ (*pdata);
        init_crc = crc8_table[byte];
        pdata ++;
    }
    return init_crc;
}

BOOL MainWindow::crc8_verify(unsigned char *pdata, int len, unsigned char init_crc)
{
    unsigned char expected_crc  = 0;
    expected_crc = crc8_calcluate(pdata, len - 1, init_crc);
    if (expected_crc != pdata[len - 1])
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

void MainWindow::dataPlot2(double hereData)
{
    if(intoPlotFuncNum<plotDataNum)
    {
        disDataVec.append(hereData);
        disSave.append(hereData);
    }
    else
    {
        disDataVec.removeFirst();
        disDataVec.append(hereData);
        disSave.append(hereData);
    }
    plot2->graph(0)->setData(timeDataVec,disDataVec);
    plot2->rescaleAxes(true);
    plot2->replot();
}

void MainWindow::dataPlot3()
{
    plot3->graph(0)->setData(posDataVec1,disDataVec);
    plot3->rescaleAxes(true);
    plot->replot();
void MainWindow::save_data_to_txt(DATA_T0_SAVE_T *head, uint8_t packet_size)
{
    uint8_t i;
    int16_t s16_data;
    float float_real_data;
    DATA_T0_SAVE_T *pnode;
    while(head->next != NULL)
    {
        pnode = head->next;
        for (i = 0; i < packet_size / 2; i++)
        {
            s16_data = pnode->data[i];
            float_real_data = (s16_data / 32768) * 10 /** ()*/;


        }
        head = pnode;
    }
}
