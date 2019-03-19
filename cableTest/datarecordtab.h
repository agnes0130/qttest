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

#define CRC_INIT 0x77

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

    const unsigned char crc8_table[256] =
    {
      0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
      157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
      35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
      190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
      70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
      219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
      101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
      248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
      140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
      17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
      175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
      50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
      202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
      87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
      233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
      116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
    };
    BOOL crc8_verify(unsigned char *pdata, int len, unsigned char init_crc);
    unsigned char crc8_calcluate(unsigned char *pdata, int len, unsigned char init_rc);
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
