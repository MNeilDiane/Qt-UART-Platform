/**
 * @file serialport.h
 * @author wujunzhe/302538094@qq.com
 * @version 1.0.1
 * @date 2023.3.19
 * @brief
 * @details 无详细说明
 * @note 描述需要注意的问题
 */

#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QWidget>
#include<QApplication>
#include<QSerialPort>
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
#include <string.h>
#include <QJsonArray>
#include <QJsonObject>


using namespace std;

namespace Ui {
    class SerialPort;
}

/**
 *  @brief 类声明，主窗口
 *  @see 对应的源文件参见
 */
class SerialPort : public QWidget {
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

    void senddata_toserial(QByteArray data,int num);

    void getdata_fromserver();

    bool serialdata_islegal(QString data);

public
    slots:
    void DataReceived(QByteArray data);

    void DataProcess(QByteArray data);

    void slotReadData();

private
    slots:
            //void DataSend();

            //void on_OpenSerialButton_clicked();

            //void LED(bool changeColor);

            void timeUpdate();

    void timerEvent(QTimerEvent *event)

    Q_DECL_OVERRIDE;


    void on_SettingSocketButton();

    void initialize_socket();

    void reconnect_byclock(int ms);


    void on_serial_btn_clicked();

    void on_Poweroff_btn_clicked();

    void eth1_judge();

    void eth2_judge();

    void get_signal(int csq);

    void set_ini_fromsocket(char *data);

    void send_ini_toserver(Socketchannel *socket);

    void start_smode_timer();

    void stop_smode_timer(QTimer *smode);

//    void eth1_judge(int status);

//    void eth2_judge(int status);

private:
    Ui::SerialPort *ui;///< UI界面定义

    //serial variable
    QSerialPort *serial;///< 通信串口
    QStringList baudList;///< 波特率
    QStringList parityist;///< 校验
    QStringList dataBitslist;///< 数据位
    QStringList stopBitslist;///< 停止位
    QStringList flowControlList;///< 流控制

    //FROM SOCKET
    QSettings *settings;///< QSetting 对象 读取socket配置文件
    vector<Socketchannel *> current_socket; ///< 装Socketchannel的容器，每次调用initialize_socket都会清空该容器，并把新实例化的容器放入

    Socketset *socketset; ///<Socketset界面
    //Socketdata *port1; ///<
    //Socketdata *port2;
    //Socketdata *port3;


    //FROM SERIAL
    QSettings *serial_settings;///< QSetting对象 读取serial配置文件
    vector <string> serial_params; /// vector<string> 放置serial_setting 配置文件读取结果

    int timer1;///< 定时器一 用来更新系统时间
    int timer2;///< 定时器二 用来更新网口一状态
    int timer3;///< 定时器三 用来更新网口三状态

    signals:
            void deletethread();///< 在每一次计时器结束调用、用户配置Socket后调用initialize_socket函数，都会将上次initialize_sockt中开启的三个线程删除并重新开启，该信号函数对应的槽函数为@ref deleteself() socketdata.h
            void stop_smode_signal();
};

#endif // SERIALPORT_H
