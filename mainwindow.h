#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void shif_Data(double p,double r,double y);
private slots:
    void clickedOpen();
    void clickedClose();
    void updateData();
    void clickedRescan();
private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    QTimer *current;
    QVector<double> pitch,roll,yaw;
    QVector<double> indexF;
};

#endif // MAINWINDOW_H
