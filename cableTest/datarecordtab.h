#ifndef DATARECORDTAB_H
#define DATARECORDTAB_H

#include<QTabWidget>
#include<QWidget>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QGridLayout>
#include<QGroupBox>
#include<QLabel>
#include<QTextLine>
#include<QMessageBox>
#include<QDateTimeEdit>
#include<QComboBox>
#include<QLineEdit>
#include<QPushButton>
#include<QFile>
#include<QFileDialog>
#include<QTextStream>
#include<QtSerialPort/QSerialPort>
#include<QtSerialPort/QSerialPortInfo>
#include"qcustomplot.h"

class dataRecordTab : public QWidget
{
    Q_OBJECT
public:
    explicit dataRecordTab(QWidget *parent = 0);

    //公有成员函数声明
    void init_dataRecordTab();
    void layout_dataRecordTab();

    //公有成员对象声明
    QLabel *data_label;
    QLabel *size_label;
    QLabel *order_label;
    QLabel *num_label;
    QLabel *startDis_label;
    QLabel *spacing_label;

    QLabel *pressureA_label;
    QLabel *pressureB_label;
    QLabel *tensile_label;
    QLabel *resistance_label;
    QLabel *countryStandard_label;

    QLineEdit *dataTimeEdit;
    QPushButton *selectTimeBtn;
    QComboBox *size_comboBox;
    QComboBox *order_comboBox;
    QComboBox *num_comboBox;
    QComboBox *serialPort_comboBox;
    QLineEdit *startDis_edit;
    QLineEdit *spacing_edit;
    QLineEdit *pressureA_edit;
    QLineEdit *tensile_edit;
    QLineEdit *abc_edit;
    QLineEdit *pressureB_edit;
    QLineEdit *resistance_edit;

    QPushButton *saveDataBtn;
    QPushButton *tensileConfirmBtn;
    QPushButton *resistanceConfirmBtn;
    QPushButton *tensileSaveDataBtn;
    QPushButton *resistanceSavaDataBtn;
    QPushButton *openSerialPortBtn;
    QPushButton *closeSerialPortBtn;
    QPushButton *beginRecordBtn;
    QPushButton *stopRecordBtn;
    QPushButton *continueRecordBtn;
    QPushButton *restartRecordBtn;
    QGridLayout *gridLayout1;
    QGridLayout *gridLayout2;

    QCustomPlot *plot1;
    QCustomPlot *plot2;
    QSerialPort *serialPort;
signals:

private slots:
    //槽函数声明
    void size_comboBoxChangedSlotFunc(int index);
    void clickSelectTimeBtnSlotFunc();
    void clickSaveDataBtnSlotFunc();
    void tensileConfirmBtnSlotFunc();
    void resistanceConfirmBtnSlotFunc();
    void tensileSaveDataBtnSlotFunc();
    void resistanceSavaDataBtnSlotFunc();
    void beginRecordBtnSlotFunc();
    void stopRecordBtnSlotFunc();
    void continueRecordBtnSlotFunc();
    void restartRecordBtnSlotFunc();
    void readCom();
    void openSerialPortSlotFunc();
    void closeSerialPortSlotFunc();
};

#endif // DATARECORDTAB_H
