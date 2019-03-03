#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include"datarecordtab.h"
#include"dataanalysetab.h"

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

    dataRecordTab *dataRecordTabObj;
    dataAnalyseTab *dataAnalyseTabObj;

    QTabWidget *tabWidgetObj;
};

#endif // WIDGET_H
