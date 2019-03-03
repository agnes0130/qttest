#include "datarecordtab.h"

dataRecordTab::dataRecordTab(QWidget *parent) : QWidget(parent)
{
    init_dataRecordTab();
    layout_dataRecordTab();
}

//成员函数定义
void dataRecordTab::init_dataRecordTab()
{//TODO:初始化dataRecord中包含的对象
    data_label =new QLabel("日期");
    size_label = new QLabel("电缆尺寸");
    order_label = new QLabel("压接顺序");
    num_label = new QLabel("压接点数");
    startDis_label = new QLabel("起始距离");
    spacing_label = new QLabel("压痕间距");
    millimeter_label = new QLabel("mm");

    size_comboBox = new QComboBox();
    QStringList sizeList;
    sizeList<<"50"<<"70"<<"120";
    size_comboBox->addItems(sizeList);
    size_comboBox->setFixedHeight(25);
    connect(size_comboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(size_comboBoxChangedSlotFunc()));
}

void dataRecordTab::layout_dataRecordTab()
{//TODO:将dataRecordTab中的内容排序
    QFont fontGroupbox("Arial",9,QFont::Normal);
    QGroupBox *groupBox1=new QGroupBox("设置");
    groupBox1->setFont(fontGroupbox);

    QGridLayout *gridLayout1= new QGridLayout;
    gridLayout1->addWidget(size_label,0,0);
    gridLayout1->addWidget(size_comboBox,0,1);

    QGridLayout *gridLayout2= new QGridLayout;
    gridLayout2->addWidget(size_label,0,0);
    gridLayout2->addWidget(size_comboBox,0,1);

    //继续添加

    QGridLayout *gridLayout_all= new QGridLayout;
    gridLayout_all->addLayout(gridLayout1,0,0);
    gridLayout_all->addLayout(gridLayout2,1,0);
    //继续添加

    groupBox1->setLayout(gridLayout_all);

    //this->setLayout(groupBox1);
    this->setLayout(gridLayout_all);
}
//槽函数定义
void dataRecordTab::size_comboBoxChangedSlotFunc()
{//TODO:在国家标准中显示size*60的计算结果
//读取linetxt中的size，计算得到结果后，再set“国家标准的”edit内容就可以显示
}
