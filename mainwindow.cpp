#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
        ui->comboBoxSerial->addItem(info.portName());
    ui->comboBoxSerial->setFocus();
    setWindowTitle(tr("Monitor"));

    pitch.resize(301);
    roll.resize(301);
    yaw.resize(301);
    indexF.resize(301);
    for(int i = indexF.count()-1;i>=0;i--){
        indexF[i]=i;
    }

    current = new QTimer(this);
    current->setInterval(80);

    serial = new QSerialPort;

    ui->pitchCur->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->pitchCur->addGraph();
    ui->pitchCur->graph(0)->setBrush(QColor(102, 153, 153));
    ui->pitchCur->axisRect()->setBackground(QColor(102, 153, 153));
    ui->pitchCur->graph(0)->setPen(QColor(255,255,255));
    ui->pitchCur->xAxis2->setVisible(false);
    ui->pitchCur->xAxis2->setTickLabels(false);
    ui->pitchCur->xAxis->setTickLabels(false);
    ui->pitchCur->yAxis->setRange(-400,400);
    ui->pitchCur->xAxis->setRange(1,300);


    ui->rollCur->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->rollCur->addGraph();
    ui->rollCur->graph(0)->setBrush(QColor(102, 153, 153));
    ui->rollCur->axisRect()->setBackground(QColor(102, 153, 153));
    ui->rollCur->graph(0)->setPen(QColor(255,255,255));
    ui->rollCur->xAxis2->setVisible(false);
    ui->rollCur->xAxis2->setTickLabels(false);
    ui->rollCur->xAxis->setTickLabels(false);
    ui->rollCur->yAxis->setRange(-400,400);
    ui->rollCur->xAxis->setRange(1,300);

    ui->yawCur->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->yawCur->addGraph();
    ui->yawCur->graph(0)->setBrush(QColor(102, 153, 153));
    ui->yawCur->axisRect()->setBackground(QColor(102, 153, 153));
    ui->yawCur->graph(0)->setPen(QColor(255,255,255));
    ui->yawCur->xAxis2->setVisible(false);
    ui->yawCur->xAxis2->setTickLabels(false);
    ui->yawCur->xAxis->setTickLabels(false);
    ui->yawCur->yAxis->setRange(-400,400);
    ui->yawCur->xAxis->setRange(1,300);

    connect(ui->OpenButton, &QPushButton::clicked, this, &MainWindow::clickedOpen);
    connect(ui->CloseButton, &QPushButton::clicked, this, &MainWindow::clickedClose);
    connect(ui->ScanButton, &QPushButton::clicked, this, &MainWindow::clickedRescan);
    connect(current,SIGNAL(timeout()),this,SLOT(updateData()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clickedOpen()
{
    QString com = ui->comboBoxSerial->currentText();
    QString baud = ui->comboBoxBaud->currentText();

    serial->setPortName(com);
    serial->setBaudRate(baud.toInt());
    if (!serial->open(QIODevice::ReadWrite)) {
        ui->statusLabel->setText(tr("Can't open %1, error code %2").arg(com).arg(serial->error()));
        return;
    }
    else{
        ui->statusLabel->setText(tr("Working"));
        ui->comboBoxSerial->setEnabled(false);
        ui->comboBoxBaud->setEnabled(false);
        ui->OpenButton->setEnabled(false);
        ui->CloseButton->setEnabled(true);
        current->start();
    }
}

void MainWindow::clickedClose()
{
    //关闭串口
    serial->clear();
    serial->close();
    current->stop();

    ui->statusLabel->setText(tr("Closed"));
    ui->comboBoxSerial->setEnabled(true);
    ui->comboBoxBaud->setEnabled(true);
    ui->OpenButton->setEnabled(true);
    ui->CloseButton->setEnabled(false);
}
void MainWindow::clickedRescan()
{
    ui->comboBoxSerial->clear();
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
        ui->comboBoxSerial->addItem(info.portName());
    ui->comboBoxSerial->setFocus();
}

void MainWindow::updateData()
{
    if(indexF[0]==301){ //针对绘图输出，如果300个数据点已满，那么标志位置-1
        indexF[0] = -1;
    }
    QByteArray buf;     //声明缓冲区buf
    if(serial->canReadLine()){      //如果串口有一行数据可读，那么执行下面的操作，否则阻塞
        buf = serial->readLine(1024);   //读取一行数据
        qDebug()<<tr(buf);              //字符串定向到debug窗口
        QString recieveData = tr(buf);  //QByteArray转化为QString类型

        //将PID数据根据###分割，并且输出到图形界面的Label上
        QString yawData = recieveData.section(QRegExp("[#]+"),3,3);
        qDebug()<<yawData.trimmed().toDouble()<<endl;
        ui->yawL->setText(yawData.trimmed());

        QString pitchData = recieveData.section(QRegExp("[#]+"),2,2);
        qDebug()<<pitchData.trimmed().toDouble()<<endl;
        ui->pitchL->setText(pitchData.trimmed());

        QString rollData = recieveData.section(QRegExp("[#]+"),1,1);
        qDebug()<<rollData.trimmed().toDouble()<<endl;
        ui->rollL->setText(rollData.trimmed());

        //电机的PWM波数据同样切割，但通过ProcessBar控件设置setValue
        QString m1Data = recieveData.section(QRegExp("[#]+"),4,4);
        qDebug()<<m1Data.trimmed().toDouble()<<endl;
        ui->motor1->setValue(m1Data.trimmed().toInt());

        QString m2Data = recieveData.section(QRegExp("[#]+"),5,5);
        qDebug()<<m2Data.trimmed().toDouble()<<endl;
        ui->motor2->setValue(m2Data.trimmed().toInt());

        QString m3Data = recieveData.section(QRegExp("[#]+"),6,6);
        qDebug()<<m3Data.trimmed().toDouble()<<endl;
        ui->motor3->setValue(m3Data.trimmed().toInt());

        QString m4Data = recieveData.section(QRegExp("[#]+"),7,7);
        qDebug()<<m4Data.trimmed().toDouble()<<endl;
        ui->motor4->setValue(m4Data.trimmed().toInt());

        //滑动移位数组，使其便于更新Plot图形绘制
        shif_Data(pitchData.trimmed().toDouble(),rollData.trimmed().toDouble(),yawData.trimmed().toDouble());
        ui->pitchCur->graph(0)->setData(indexF,pitch);
        ui->rollCur->graph(0)->setData(indexF,roll);
        ui->yawCur->graph(0)->setData(indexF,yaw);
        ui->pitchCur->replot();
        ui->rollCur->replot();
        ui->yawCur->replot();
    }
}

void MainWindow::shif_Data(double p,double r,double y){
    if(indexF[0]!=-1){
        pitch[indexF[0]]=p;
        roll[indexF[0]]=r;
        yaw[indexF[0]]=y;
        indexF[0]++;
    }
    else{
        for(int i=1;i<indexF.count()-1;i++){
            pitch[i]=pitch[i+1];
            roll[i]=roll[i+1];
            yaw[i]=yaw[i+1];
        }
        pitch[300]=p;
        roll[300]=r;
        yaw[300]=y;
    }
}
