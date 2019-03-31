#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QDateTime>
#include "QDebug"
#include <QTextStream>
#include <QDir>
#include <QMessageBox>
#include <QFile>
#include <QString>
#include <qfiledialog.h>
#include <QVector>
#include "qcustomplot.h"
#include<QtSerialPort/QSerialPort>
#include<QtSerialPort/QSerialPortInfo>

#include "am.h"

namespace Ui {
class MainWindow;
}
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public:
    am *am_obj;
    void init();
    QCustomPlot *plot;
    QCustomPlot *plot2;
    QCustomPlot *plotAll;
    QSerialPort *serialPort;
    QByteArray byteArray;
    QVector<int> pressData;
    QVector<QString> pullPress;
    QVector<QString> pullData;
    QVector<QString> resistPress;
    QVector<QString> resistData;
    void dataPlot(int);
    void dataPlot2(double);
    int plotDataNum=100;
    int intoPlotFuncNum=0;
    int startFlag = 0;
    QVector<double> posDataVec1;
    QVector<double> disDataVec;
    QVector<double> timeDataVec;
    QVector<int> pressSave;
    QVector<double> disSave;
    QVector<double> timeSave;
private slots:
    void on_comboSize_currentIndexChanged(int index);
    void on_pushButton_clicked();
    void on_timeButton_clicked();
    void base_information_save();
    void on_pressButton_clicked();
    void on_pullConfirm_clicked();
    void on_resistConfirm_clicked();
    void on_resistButton_clicked();
    void on_serialOpen_clicked();
    void on_serialClose_clicked();
    void on_pressFile_clicked();
    void on_positionFile_clicked();
    void on_startButton_clicked();
    void readCom();
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
