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
#include<QProcess>

#include <vector>
#include <socketchannel.h>
#include <socketset.h>
#include <socketdata.h>
#include <signaldetect.h>


using namespace std;

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
    //void RefreshSerialPort(int index);


    //FROM SOCKET
    //void initialize_socket();
    void socket_setting_fun();
    void senddata_toserver(QByteArray data);
    void senddata_toserial(QByteArray data);
    void getdata_fromserver();

public slots:
    void DataReceived(QByteArray data);
    void DataProcess(QByteArray data);
    void slotReadData();

private slots:
    //void DataSend();

    //void on_OpenSerialButton_clicked();

    //void LED(bool changeColor);

    void timeUpdate();

    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;


    void on_SettingSocketButton();
    void initialize_socket();
    void reconnect_byclock(int ms);


    void on_serial_btn_clicked();

    void on_Poweroff_btn_clicked();

    void eth1_judge();

    void eth2_judge();

    void get_signal(int csq);

//    void eth1_judge(int status);

//    void eth2_judge(int status);

private:
    Ui::SerialPort *ui;

    //serial variable
    QSerialPort *serial;
    QStringList baudList;
    QStringList parityist;
    QStringList dataBitslist;
    QStringList stopBitslist;
    QStringList flowControlList;

    //FROM SOCKET
    QSettings *settings;
    vector<Socketchannel *> current_socket;

    Socketset *socketset;
    Socketdata *port1;
    Socketdata *port2;
    Socketdata *port3;




    //FROM SERIAL
    QSettings *serial_settings;
    vector<string>serial_params;

    int timer1;
    int timer2;
    int timer3;

signals:
    void deletethread();


};

#endif // SERIALPORT_H
