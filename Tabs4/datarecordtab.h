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
#include<QGridLayout>

class dataRecordTab : public QWidget
{
    Q_OBJECT
public:
    explicit dataRecordTab(QWidget *parent = nullptr);

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
    QLabel *millimeter_label;

    QDateTimeEdit *dataTimeEdit;
    QComboBox *size_comboBox;
    QComboBox *order_comboBox;
    QComboBox *num_comboBox;
    QLineEdit *startDis_edit;
    QLineEdit *spacing_edit;

    QGridLayout *gridLayout1;
    QGridLayout *gridLayout2;

signals:

public slots:
    //槽函数声明
    void size_comboBoxChangedSlotFunc();
};

#endif // DATARECORDTAB_H
