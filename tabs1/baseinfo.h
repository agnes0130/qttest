#ifndef BASEINFO_H
#define BASEINFO_H
#include <QtCore/qglobal.h>
#if QT_VERSION >= 0x050000
#include <QtWidgets/QWidget>
#else
#include <QtGui/QWidget>
#endif
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QGridLayout>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPixmap>

class BaseInfo : public QWidget
{
    Q_OBJECT
public:
    explicit BaseInfo(QWidget *parent = nullptr);

signals:

public slots:

private:

    //左侧

    QLabel *UserNameLabel;                      //用户名

    QLabel *NameLabel;                          //姓名

    QLabel *SexLabel;                           //性别

    QLabel *DepartmentLabel;                    //部门

    QLabel *AgeLabel;                           //年龄

    QLabel *OtherLabel;                         //备注

    QLineEdit *UserNameLineEdit;

    QLineEdit *NameLineEdit;

    QComboBox *SexCombox;

    QTextEdit *DepartmentTextEdit;

    QLineEdit *AgeLineEdit;

    QGridLayout *LeftLayout;

    //右侧

    QLabel *HeadLabel;                          //右上角部分，头像

    QLabel *HeadIconLabel;                      //头像图标

    QPushButton *UpdateHeadBtn;                 //更新按钮

    QHBoxLayout *TopRightLayout;                //个人说明

    QLabel *IntroductionLabel;

    QTextEdit *IntroductionTextEdit;

    QVBoxLayout *RightLayout;
};

#endif // BASEINFO_H

