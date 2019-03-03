#include "widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    tabWidgetObj = new QTabWidget(this);
    dataRecordTabObj = new dataRecordTab();
    dataAnalyseTabObj = new dataAnalyseTab();

    tabWidgetObj->addTab(dataRecordTabObj,"dataRecord");
    tabWidgetObj->addTab(dataAnalyseTabObj,"dataAnalyse");

    QHBoxLayout *hBoxlayout1 = new QHBoxLayout;
    hBoxlayout1->addWidget(tabWidgetObj);

    QHBoxLayout *hBoxlayout2 = new QHBoxLayout;

    QVBoxLayout *vBoxLayout1 = new QVBoxLayout;
    vBoxLayout1->addLayout(hBoxlayout1);
    vBoxLayout1->addLayout(hBoxlayout2);
    this->setLayout(vBoxLayout1);
}

Widget::~Widget()
{

}
