#ifndef CONTACT_H
#define CONTACT_H
#include <QtCore/qglobal.h>
#if QT_VERSION >= 0x050000
#include <QtWidgets/QWidget>
#else
#include <QtGui/QWidget>
#endif
#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>
#include <QCheckBox>

class Contact : public QWidget
{
    Q_OBJECT
public:
    explicit Contact(QWidget *parent = nullptr);

signals:

public slots:
private:

    QLabel *EmailLabel;

    QLineEdit *EmailLineEdit;

    QLabel *AddrLabel;

    QLineEdit *AddrLineEdit;

    QLabel *CodeLabel;

    QLineEdit *CodeLineEdit;

    QLabel *MoviTelLabel;

    QLineEdit *MoviTelLineEdit;

    QCheckBox *MoviTelCheckBox;

    QLabel *ProTelLabel;

    QLineEdit *ProTelLineEdit;

    QGridLayout *mainLayout;
};

#endif // CONTACT_H
