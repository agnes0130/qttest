#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include"datarecordtab.h"
#include"dataanalysetab.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    /*declare the object using in the mainwindow*/
    dataRecordTab *dataRecordTabObj;
    dataAnalyseTab *dataAnalyseTabObj;

    QTabWidget *dataRecord;
    QTabWidget *dataAnalyse;

    /*declare the variable*/
    /*declare the function*/

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
