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
    void init();

private slots:
    void on_lineEdit_3_editingFinished();

    void on_lineEdit_4_editingFinished();

    void on_comboSize_currentIndexChanged(int index);

    void on_pushButton_clicked();

    void on_timeButton_clicked();
    void base_information_save();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
