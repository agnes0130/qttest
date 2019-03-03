#ifndef DATAANALYSETAB_H
#define DATAANALYSETAB_H

#include<QTabWidget>
#include<QWidget>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QGridLayout>
#include<QGroupBox>
#include<QLabel>
#include<QTextLine>
#include<QMessageBox>
class dataAnalyseTab : public QWidget
{
    Q_OBJECT
public:
    explicit dataAnalyseTab(QWidget *parent = 0);

    void init_dataAnalyseTab();

signals:

private slots:
};

#endif // DATAANALYSETAB_H
