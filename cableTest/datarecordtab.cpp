#include "datarecordtab.h"

dataRecordTab::dataRecordTab(QWidget *parent) : QWidget(parent)
{
    init_dataRecordTab();
    layout_dataRecordTab();
}

//成员函数定义
void dataRecordTab::init_dataRecordTab()
{//TODO:初始化dataRecord中包含的对象
    data_label =new QLabel(tr("日期"));
    //data_label->setFixedSize(QSize(60,25));

    size_label = new QLabel(tr("电缆尺寸"));
    size_label->setFixedSize(QSize(80,25));

    order_label = new QLabel(tr("压接顺序"));
    order_label->setFixedSize(QSize(80,25));

    num_label = new QLabel(tr("压接点数"));
    num_label->setFixedSize(QSize(80,25));

    startDis_label = new QLabel(tr("起始距离(mm)"));
    startDis_label->setFixedSize(QSize(80,25));

    spacing_label = new QLabel(tr("压痕间距(mm)"));
    spacing_label->setFixedSize(QSize(80,25));

    pressureA_label = new QLabel(tr("压力"));
    pressureA_label->setFixedSize(QSize(80,25));

    pressureB_label = new QLabel(tr("压力"));
    pressureB_label->setFixedSize(QSize(80,25));

    tensile_label = new QLabel(tr("抗拉强度"));
    tensile_label->setFixedSize(QSize(80,25));

    resistance_label = new QLabel(tr("接触电阻"));
    resistance_label->setFixedSize(QSize(80,25));

    countryStandard_label = new QLabel(tr("/国家标准"));
    countryStandard_label->setFixedSize(QSize(80,25));

    size_comboBox = new QComboBox();
    QStringList sizeList;
    sizeList<<"50"<<"70"<<"120";
    size_comboBox->addItems(sizeList);
    size_comboBox->setFixedHeight(25);
    size_comboBox->setMaximumWidth(120);
    connect(size_comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(size_comboBoxChangedSlotFunc(int)));

    order_comboBox = new QComboBox();
    QStringList orderList;
    orderList<<"123"<<"456";
    order_comboBox->addItems(orderList);
    order_comboBox->setFixedHeight(25);
    order_comboBox->setMaximumWidth(120);

    num_comboBox = new QComboBox();
    QStringList numList;
    numList<<"123"<<"456";
    num_comboBox->addItems(numList);
    num_comboBox->setFixedHeight(25);
    num_comboBox->setMaximumWidth(120);

    serialPort_comboBox = new QComboBox();
    serialPort_comboBox->setFixedHeight(25);
    serialPort_comboBox->setMaximumWidth(120);

    dataTimeEdit = new QLineEdit();
    dataTimeEdit->setFixedHeight(25);
    dataTimeEdit->setMaximumWidth(120);

    startDis_edit = new QLineEdit();
    startDis_edit->setFixedHeight(25);
    startDis_edit->setMaximumWidth(120);

    spacing_edit = new QLineEdit();
    spacing_edit->setFixedHeight(25);
    spacing_edit->setMaximumWidth(120);

    selectTimeBtn =new QPushButton(tr("选择时间"));
    connect(selectTimeBtn, SIGNAL(clicked()), this, SLOT(clickSelectTimeBtnSlotFunc()));

    saveDataBtn = new QPushButton(tr("实验数据保存"));
    saveDataBtn->setMaximumWidth(200);
    connect(saveDataBtn, SIGNAL(clicked()), this, SLOT(clickSaveDataBtnSlotFunc()));

    pressureA_edit = new QLineEdit();
    pressureA_edit->setFixedHeight(25);
    pressureA_edit->setMaximumWidth(120);

    tensile_edit = new QLineEdit();
    tensile_edit->setFixedHeight(25);
    tensile_edit->setMaximumWidth(120);

    abc_edit = new QLineEdit();
    abc_edit->setFixedHeight(25);
    abc_edit->setMaximumWidth(120);

    pressureB_edit = new QLineEdit();
    pressureB_edit->setFixedHeight(25);
    pressureB_edit->setMaximumWidth(120);

    resistance_edit= new QLineEdit();
    resistance_edit->setFixedHeight(25);
    resistance_edit->setMaximumWidth(120);

    tensileConfirmBtn = new QPushButton(tr("确定"));
    connect(tensileConfirmBtn, SIGNAL(clicked()), this, SLOT(tensileConfirmBtnSlotFunc()));

    resistanceConfirmBtn = new QPushButton(tr("确定"));
    connect(resistanceConfirmBtn, SIGNAL(clicked()), this, SLOT(resistanceConfirmBtnSlotFunc()));

    tensileSaveDataBtn = new QPushButton(tr("抗拉测试结果保存"));
    connect(tensileSaveDataBtn, SIGNAL(clicked()), this, SLOT(tensileSaveDataBtnSlotFunc()));

    resistanceSavaDataBtn = new QPushButton(tr("电阻测试结果保存"));
    connect(resistanceSavaDataBtn, SIGNAL(clicked()), this, SLOT(resistanceSavaDataBtnSlotFunc()));

    openSerialPortBtn = new QPushButton(tr("打开串口"));
    connect(openSerialPortBtn, SIGNAL(clicked()), this, SLOT(openSerialPortSlotFunc()));

    closeSerialPortBtn = new QPushButton(tr("关闭串口"));
    connect(closeSerialPortBtn, SIGNAL(clicked()), this, SLOT(closeSerialPortSlotFunc()));

    beginRecordBtn = new QPushButton(tr("开始记录"));
    connect(beginRecordBtn, SIGNAL(clicked()), this, SLOT(beginRecordBtnSlotFunc()));

    stopRecordBtn = new QPushButton(tr("停止"));
    connect(stopRecordBtn, SIGNAL(clicked()), this, SLOT(stopRecordBtnSlotFunc()));

    continueRecordBtn = new QPushButton(tr("继续记录"));
    connect(continueRecordBtn, SIGNAL(clicked()), this, SLOT(continueRecordBtnSlotFunc()));

    restartRecordBtn = new QPushButton(tr("重新开始"));
    connect(restartRecordBtn, SIGNAL(clicked()), this, SLOT(restartRecordBtnSlotFunc()));

    plot1 = new QCustomPlot(this);
    plot2 = new QCustomPlot(this);

    serialPort = new QSerialPort(this);
    foreach(const QSerialPortInfo &SerialPortInfo, QSerialPortInfo::availablePorts())
    {
        serialPort_comboBox->addItem(SerialPortInfo.portName());
    }
    connect(serialPort,SIGNAL(readyRead()),this,SLOT(readCom()));
}

void dataRecordTab::layout_dataRecordTab()
{//TODO:将dataRecordTab中的内容排序
    QFont fontGroupbox("Arial",9,QFont::Normal);
    QGroupBox *groupBox1=new QGroupBox("设置");
    groupBox1->setFont(fontGroupbox);

    QGridLayout *gridLayout1_0= new QGridLayout;
    gridLayout1_0->addWidget(data_label,0,0,Qt::AlignLeft);
    gridLayout1_0->addWidget(dataTimeEdit,0,1);
    gridLayout1_0->addWidget(selectTimeBtn,0,2);

    QGridLayout *gridLayout1_1= new QGridLayout;
    gridLayout1_1->addWidget(size_label,1,0,Qt::AlignLeft);
    gridLayout1_1->addWidget(size_comboBox,1,1);

    QGridLayout *gridLayout1_2= new QGridLayout;
    gridLayout1_2->addWidget(order_label,2,0,Qt::AlignLeft);
    gridLayout1_2->addWidget(order_comboBox,2,1);

    QGridLayout *gridLayout1_3= new QGridLayout;
    gridLayout1_3->addWidget(num_label,3,0,Qt::AlignLeft);
    gridLayout1_3->addWidget(num_comboBox,3,1);

    QGridLayout *gridLayout1_4= new QGridLayout;
    gridLayout1_4->addWidget(startDis_label,4,0,Qt::AlignLeft);
    gridLayout1_4->addWidget(startDis_edit,4,1);

    QGridLayout *gridLayout1_5= new QGridLayout;
    gridLayout1_5->addWidget(spacing_label,5,0,Qt::AlignLeft);
    gridLayout1_5->addWidget(spacing_edit,5,1);

    QGridLayout *gridLayout1_6= new QGridLayout;
    gridLayout1_6->addWidget(saveDataBtn,6,0);

    QGridLayout *gridLayout1_all= new QGridLayout;
    gridLayout1_all->addLayout(gridLayout1_0,0,0);
    gridLayout1_all->addLayout(gridLayout1_1,1,0);
    gridLayout1_all->addLayout(gridLayout1_2,2,0);
    gridLayout1_all->addLayout(gridLayout1_3,3,0);
    gridLayout1_all->addLayout(gridLayout1_4,4,0);
    gridLayout1_all->addLayout(gridLayout1_5,5,0);
    gridLayout1_all->addLayout(gridLayout1_6,6,0);

    QGroupBox *groupBox2=new QGroupBox("抗拉测试");
    groupBox2->setFont(fontGroupbox);
    QGridLayout *gridLayout2_0= new QGridLayout;
    gridLayout2_0->addWidget(pressureA_label,0,0);
    gridLayout2_0->addWidget(pressureA_edit,0,1);
    gridLayout2_0->addWidget(countryStandard_label,0,2);
    gridLayout2_0->addWidget(abc_edit,0,3);

    QGridLayout *gridLayout2_1= new QGridLayout;
    gridLayout2_1->addWidget(tensile_label,0,0);
    gridLayout2_1->addWidget(tensile_edit,0,1);
    gridLayout2_1->addWidget(tensileConfirmBtn,0,2);
    gridLayout2_1->addWidget(tensileSaveDataBtn,0,3);

    QGridLayout *gridLayout2_all= new QGridLayout;
    gridLayout2_all->addLayout(gridLayout2_0,0,0);
    gridLayout2_all->addLayout(gridLayout2_1,1,0);

    QGroupBox *groupBox3=new QGroupBox("电阻测试");
    groupBox3->setFont(fontGroupbox);
    QGridLayout *gridLayout3_0= new QGridLayout;
    gridLayout3_0->addWidget(pressureB_label,0,0);
    gridLayout3_0->addWidget(pressureB_edit,0,1);

    QGridLayout *gridLayout3_1= new QGridLayout;
    gridLayout3_1->addWidget(resistance_label,0,0);
    gridLayout3_1->addWidget(resistance_edit,0,1);

    QGridLayout *gridLayout3_2= new QGridLayout;
    gridLayout3_2->addWidget(resistanceConfirmBtn,0,0);
    gridLayout3_2->addWidget(resistanceSavaDataBtn,0,1);

    QGridLayout *gridLayout4= new QGridLayout;
    gridLayout4->addWidget(serialPort_comboBox,0,0);
    gridLayout4->addWidget(openSerialPortBtn,0,1);
    gridLayout4->addWidget(closeSerialPortBtn,0,2);
    gridLayout4->addWidget(beginRecordBtn,0,3);
    gridLayout4->addWidget(stopRecordBtn,0,4);
    gridLayout4->addWidget(continueRecordBtn,0,5);
    gridLayout4->addWidget(restartRecordBtn,0,6);

    QGridLayout *gridLayout5= new QGridLayout;
    gridLayout5->addWidget(plot1,0,0);
    gridLayout5->addWidget(plot2,0,1);

    QGridLayout *gridLayout3_all= new QGridLayout;
    gridLayout3_all->addLayout(gridLayout3_0,0,0);
    gridLayout3_all->addLayout(gridLayout3_1,1,0);

    QHBoxLayout *hBoxLaout_0 = new QHBoxLayout;
    hBoxLaout_0->addLayout(gridLayout3_2);

    QHBoxLayout *hBoxLaout_1 = new QHBoxLayout;
    hBoxLaout_1->addLayout(gridLayout3_all);
    hBoxLaout_1->addLayout(hBoxLaout_0);

    groupBox1->setLayout(gridLayout1_all);
    groupBox2->setLayout(gridLayout2_all);
    groupBox3->setLayout(hBoxLaout_1);

    QVBoxLayout *vBoxLayout_0 = new QVBoxLayout;
    vBoxLayout_0->addWidget(groupBox2);
    vBoxLayout_0->addWidget(groupBox3);

    QHBoxLayout *hBoxLaout = new QHBoxLayout;
    hBoxLaout->addWidget(groupBox1);
    hBoxLaout->addLayout(vBoxLayout_0);

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    vBoxLayout->addLayout(hBoxLaout);
    vBoxLayout->addLayout(gridLayout4);
    vBoxLayout->addLayout(gridLayout5);
    this->setLayout(vBoxLayout);
}

unsigned char dataRecordTab::crc8_calcluate(unsigned char *pdata, int len, unsigned char init_crc)
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

BOOL dataRecordTab::crc8_verify(unsigned char *pdata, int len, unsigned char init_crc)
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
//槽函数定义
void dataRecordTab::size_comboBoxChangedSlotFunc(int index)
{
    switch (index)
    {
    case 0:
        abc_edit->setText("3000N");
        break;

    case 1:
        abc_edit->setText("4200N");
        break;

    case 2:
        abc_edit->setText("7200N");
        break;

    case 3:
        abc_edit->setText("9000N");
        break;

    case 4:
        abc_edit->setText("14400N");
        break;

    case 5:
        abc_edit->setText("18000N");
        break;

    case 6:
        abc_edit->setText("24000N");
        break;
    }
}

void dataRecordTab::clickSelectTimeBtnSlotFunc()
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy.MM.dd hh:mm");
    dataTimeEdit->setText(current_date);
}

void dataRecordTab::clickSaveDataBtnSlotFunc()
{
    QString time_str = dataTimeEdit->text();
    QString size_str = size_comboBox->currentText();
    QString first_str = startDis_edit->text();
    QString gap_str = spacing_edit->text();
    int turn_index = order_comboBox->currentIndex();
    int number_index = num_comboBox->currentIndex();
    QString path = QFileDialog::getSaveFileName(this, tr("Open File"), ".", tr("Text Files(*.txt)"));
    if(!path.isEmpty())
    {
        QFile file(path);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QMessageBox::warning(this,tr("Write File"),tr("Cannot open file:\n%1").arg(path));
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
        file.close();
    }
}

void dataRecordTab::tensileConfirmBtnSlotFunc()
{

}

void dataRecordTab::resistanceConfirmBtnSlotFunc()
{

}

void dataRecordTab::tensileSaveDataBtnSlotFunc()
{

}

void dataRecordTab::resistanceSavaDataBtnSlotFunc()
{

}

void dataRecordTab::beginRecordBtnSlotFunc()
{

}

void dataRecordTab::stopRecordBtnSlotFunc()
{

}

void dataRecordTab::continueRecordBtnSlotFunc()
{

}

void dataRecordTab::restartRecordBtnSlotFunc()
{

}

void dataRecordTab::readCom()
{

}

void dataRecordTab::openSerialPortSlotFunc()
{

}

void dataRecordTab::closeSerialPortSlotFunc()
{

}
