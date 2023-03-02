#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QWidget>
#include<QApplication>
#include<QtSerialPort>
#include<QtSerialPort/QSerialPortInfo>
#include<QDebug>
#include<QTimer>
#include<QThread>
#include<QFile>
#include<QDateTime>
#include<QDir>
#include<QCloseEvent>
#include<QTcpServer>
#include<QTcpSocket>
namespace Ui {
class SerialPort;
}

class SerialPort : public QWidget
{
    Q_OBJECT

public:
    explicit SerialPort(QWidget *parent = nullptr);
    ~SerialPort();
    void SerialPortInit();
    void RefreshSerialPort(int index);

public slots:
    void DataReceived(QByteArray data);
    void DataProcess(QByteArray data);
    void slotReadData();

private slots:
    void DataSend();

    void on_OpenSerialButton_clicked();

    void on_ClearButton_clicked();

    void LED(bool changeColor);

    void on_ClearShowButton_clicked();

    void on_SendButton_clicked();

    void timeUpdate();

private:
    Ui::SerialPort *ui;

    //serial variable
    QSerialPort *serial;
    QStringList baudList;
    QStringList parityist;
    QStringList dataBitslist;
    QStringList stopBitslist;
    QStringList flowControlList;

};

#endif // SERIALPORT_H
