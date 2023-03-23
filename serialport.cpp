/**
 * @file serialport.cpp
 * @author wujunzhe/302538094@qq.com
 * @version 1.0.1
 * @date 2023.3.19
 * @brief
 * @details 无详细说明
 * @note 描述需要注意的问题
 */

#include "serialport.h"
#include "serialdata.h"
#include "ui_serialport.h"
#include<QDebug>
#include<QMessageBox>
#include<ctime>

/**
* @brief SerialPort 构造函数
* @details 构造函数，执行UI界面构建，页面参数设置。打开通信串口，初始化socket设置，
 *设置重连时间，开启三个端口通信子线程，开启4G信号强度检测的子线程。
* @param QWidget*
* @return none
* @warning none
* @note none
*/
SerialPort::SerialPort(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::SerialPort) {
    ui->setupUi(this);
    resize(QSize(800, 600));
    serial = new QSerialPort;
    SerialPortInit();
    socket_setting_fun();
    initialize_socket();
    reconnect_byclock(600000);
    port1 = nullptr;
    port2 = nullptr;
    port3 = nullptr;
    Signaldetect *a = new Signaldetect();
    connect(a, &Signaldetect::sendsignals, this, &SerialPort::get_signal);
    a->start();
}

/**
* @brief SerialPort类析构函数
* @details 析构时会将打开的通信串口清空并关闭
* @param QWidget*
* @return none
* @warning none
* @note none
*/
SerialPort::~SerialPort() {
    serial->clear();
    serial->close();
    delete ui;
}

/**
* @brief SerialPort 成员函数 SerialPortInit()
* @details 初始化三个定时器，分别用于刷新系统时间，网口一与网口二的连接情况。配置串口基本参数。
 * 配置UI界面控件显示参数，读取serial_setting配置文件。配置文件读取结果在UI界面显示
* @param QWidget*
* @return none
* @warning none
* @note none
*/
void SerialPort::SerialPortInit() {
    timer1 = startTimer(1000);
    timer2 = startTimer(6000);
    timer3 = startTimer(6000);
    serial->setPortName("ttyS3");
    serial->setBaudRate(QSerialPort::Baud57600);
    serial->setParity(QSerialPort::NoParity);
    serial->setDataBits(QSerialPort::Data8);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    if (!serial->open(QIODevice::ReadWrite)) {
        QMessageBox::warning(this, tr("提示"), tr("串口打开失败!"), QMessageBox::Ok);
        return;
    }
    ui->Messagetime->setWordWrap(true);
    ui->Messagetime->setAlignment(Qt::AlignLeft);
    ui->DataReceived->setMaximumBlockCount(30);
    connect(serial, &QSerialPort::readyRead, this, &SerialPort::slotReadData);
    ui->Leftwindow->setLineWidth(1);
    ui->Leftwindow->setMidLineWidth(1);
    ui->Leftwindow->setFrameShape(QFrame::Box);
    ui->Leftwindow->setFrameShadow(QFrame::Raised);

    QString serialini_filepath = QCoreApplication::applicationDirPath() + "/serial_setting.ini";
    QFileInfo fileInfo(serialini_filepath);
    if (!fileInfo.isFile()) {
        qDebug() << "not found";
        serial_settings = new QSettings(serialini_filepath, QSettings::IniFormat);
    } else {
        serial_settings = new QSettings(serialini_filepath, QSettings::IniFormat);
        serial_settings->setIniCodec(QTextCodec::codecForName("UTF-8"));
        QStringList grouplist = serial_settings->childGroups();
        for (QString group: grouplist) {
            serial_settings->beginGroup(group);
            QStringList keylist = serial_settings->childKeys();
            for (QString key: keylist) {
                serial_params.push_back(serial_settings->value(key).toString().toStdString());
            }
            serial_settings->endGroup();

        }

        ui->n_line1->setText(QString::fromStdString(serial_params[2]));
        ui->n_line2->setText(QString::fromStdString(serial_params[5]));
        ui->n_line3->setText(QString::fromStdString(serial_params[8]));
        ui->n_line4->setText(QString::fromStdString(serial_params[11]));
        ui->n_line5->setText(QString::fromStdString(serial_params[14]));

        ui->d_line1->setText(QString::fromStdString(serial_params[1]));
        ui->d_line2->setText(QString::fromStdString(serial_params[4]));
        ui->d_line3->setText(QString::fromStdString(serial_params[7]));
        ui->d_line4->setText(QString::fromStdString(serial_params[10]));
        ui->d_line5->setText(QString::fromStdString(serial_params[13]));

        eth1_judge();
        eth2_judge();
    }

}

/**
* @brief SerialPort 成员函数slotReadData（）
* @details 为了克服Qt官方接口在读取串口时存在的长信息读不全的问题，slotReadData会先将读取到的串口缓存
 * 放置到缓存内不断叠加，直到遇到“\r\n”，之后形成一个完整的报文消息进行数据显示，数据解析与socket发送，并清空缓存。
* @param QWidget*
* @return none
* @warning none
* @note none
*/
void SerialPort::slotReadData() {
    static QByteArray sumData;
    QByteArray tempData = serial->readAll();
    if (!tempData.isEmpty()) {
        sumData.append(tempData);
        //qDebug()<<sumData<<endl;
        if (sumData.contains("\r\n")) {
            DataReceived(sumData);
            DataProcess(sumData);
            senddata_toserver(sumData.simplified());
            sumData.clear();
        }
    }
    tempData.clear();
}

/**
* @brief SerialPort成员函数DataReceived
* @details 接受slotReadData发送的串口信息并在UI界面进行显示，添加时间戳与发送平台
* @param QByteArray data
* @return none
* @warning none
* @note none
*/
void SerialPort::DataReceived(QByteArray data) {
    if (!data.isEmpty()) {
        QString str = ui->DataReceived->toPlainText();
        data = data.simplified();
        QDateTime timecur = QDateTime::currentDateTime();
        QString timestr = timecur.toString("[yy-MM-dd hh:mm:ss]");
        str += timestr;
        str += tr("收到传感器串口数据: \n" + data);
        //str += tr(data);
        //qDebug("Info:%s",data.toStdString().data());
        ui->DataReceived->appendPlainText(str);
        //ui->DataReceived->clear();
    }
}

/**
* @brief SerialPort 成员函数 DataProcess
* @details 收到slotReadData发送的串口数据进行数据解析，根据传感器的数量执行data解析的循环将全部的数据解析出来。
 * 根据读取配置文件的信息，将读取报文的传感器编号进行匹配，将解析出来的data显示在指定的位置上。
* @param QByteArray data
* @return none
* @warning none
* @note 注解
*/
void SerialPort::DataProcess(QByteArray data) {

    Serialdata *serialdata = new Serialdata();
    QString serialstr = QString(data);
    QStringList list = serialstr.split(":");
    if (list[0] == "Port1") {
        ui->DataReceived->appendPlainText("发送配置平台\n");
    } else if (list[0] == "Port2") {
        ui->DataReceived->appendPlainText("发送数据平台\n");
    } else if (list[0] == "Port3") {
        ui->DataReceived->appendPlainText("发送数据推送\n");
    }
    serialdata->Stationcode.append(list[2]);
    serialdata->Sensortotal.append(list[3]);
    int tmp = list[3].toInt();
    for (int i = 0; i < tmp; i++) {
        QStringList timelist = list[5 + tmp].split(" ");
        timelist[1].replace("-", ":");
        QString Messagetime = timelist.join(" ");

        ui->Messagetime->setText(Messagetime);

        QString temp = list[4 + i];
        QStringList temp_spilt = temp.split(" ");
        serialdata->Sensornum.append(temp_spilt[0]);
        serialdata->Errorcode.append(temp_spilt[1]);
        QStringList strlist = serialdata->FormatData(temp_spilt[2]);

        serialdata->Sensordata.push_back(strlist.join(" "));

        QStringList battery_spilt = list[4 + tmp].split(" ");
        serialdata->Batterynum = battery_spilt[0];
        serialdata->Batteryvoltage = serialdata->FormatData(battery_spilt[2])[0];


        ui->TNS->setText(serialdata->Stationcode);
        ui->Batteryvoltage->setText(serialdata->Batteryvoltage);
        std::string str = serialdata->Sensornum.toStdString();

        if (str == serial_params[0]) {
            serialdata->Rainfall = serialdata->Sensordata.back();
            ui->Rainfall->setText(serialdata->Rainfall);
        } else if (str == serial_params[3]) {
            serialdata->Waterlevel = serialdata->Sensordata.back();
            ui->Waterlevel->setText(serialdata->Waterlevel);

        } else if (str == serial_params[6]) {
            serialdata->Flowvelocity = serialdata->Sensordata.back();
            ui->Flowvelocity->setText(serialdata->Flowvelocity);

        } else if (str == serial_params[9]) {
            serialdata->Flow = serialdata->Sensordata.back();
            ui->Flow->setText(serialdata->Flow);

        } else if (str == serial_params[12]) {
            serialdata->Totalflow = serialdata->Sensordata.back();
            ui->Totalflow->setText(serialdata->Totalflow);

        } else {

        }
        serialdata->Sensornum.clear();
    }

}

/**
* @brief SerialPort 成员函数timeUpdate
* @details 实时刷新系统时间
* @param none
* @return none
* @warning none
* @note none
*/
void SerialPort::timeUpdate() {
    QFont font("Micsoft Yahei", 8, 50);
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy-MM-dd hh:mm:ss");
    ui->Timeshow->setFont(font);
    ui->Timeshow->setText(str);
}

/**
* @brief SerialPort 成员函数timerEvent
* @details 重写TimeEvent，调用三个定时器。一个定时器刷新系统时间，两个定时器用来判断两个网口的连接状态。
* @param QTimerEvent *event
* @return none
* @warning none
* @note none
*/
void SerialPort::timerEvent(QTimerEvent *event) {
    if (event->timerId() == timer1) {
        timeUpdate();
    }
    if (event->timerId() == timer2) {
        eth1_judge();
    }
    if (event->timerId() == timer3) {
        eth2_judge();
    }
}

/**
* @brief SerialPort成员函数 senddata_toserial
* @details
* @param QByteArray data
* @return none
* @warning none
* @note none
*/
void SerialPort::senddata_toserial(QByteArray data,int num) {
    //qDebug("str:%s",data.toStdString().data());
    QString socketstr;
    if(num==1)
    {
        socketstr="收到配置平台数据";
    }
    else if(num==2)
    {
        socketstr="收到数据平台数据";
    }
    else if(num==3)
    {
        socketstr="收到数据推送据";
    }

    else
        socketstr="";
    QDateTime timecur = QDateTime::currentDateTime();
    QString timestr = timecur.toString("[yy-MM-dd hh:mm:ss]");
    ui->DataReceived->appendPlainText(timestr + socketstr+"\n未解析\n发送至传感器串口\n");
    QString str = QString::number(num)+":";
    serial->write(str.toUtf8().append(data));
}

/**
* @brief Serialport成员函数senddata_toserver
* @details 用于将串口数据转发至指定的Socket服务器上
* @param QByteArray data
* @return none
* @warning none
* @note none
*/
void SerialPort::senddata_toserver(QByteArray data) {

    qDebug() << data << endl;
    int i = data[4] - 49;
    if (!current_socket.empty() && current_socket[i]->connectstatus >= 0) {
        this->current_socket[i]->SendMessage(QString(data + "\r\n").toUtf8().data());
        //if(this->current_socket[0]->RecieveMessage()){
        //    getdata_fromserver("ok");
        //}
    }
}

/**
* @brief Serialport成员函数initialize_socket
* @details 该函数有多个功能，1.读取socket_setting.ini，并通过读取的数据实例化三个Socketchannel对象，将其存入current_socket
* 2.绑定Socketset页面"保存"按钮触发的信号函数reinitial_socket_fromserial与自身initialize_socket 3.实例化三个Socketdata子线程
* 用于实现接收下行数据功能 4.该函数设定了计时器，拟定10分钟自动调用一次，在reconnect_byclock(int ms)中设置，
* 每次重新调用都会清空current_socket和三个子线程
* @param none
* @return none
* @warning none
* @note 注解
*/
void SerialPort::initialize_socket() {
    socketset = new Socketset();
    connect(socketset, SIGNAL(reinitial_socket_fromserial()), this, SLOT(initialize_socket()));
    for(Socketchannel *socket:current_socket){
        socket->SocketDisconnect();
    }
    current_socket.clear();
    QString ini_filepath = QCoreApplication::applicationDirPath() + "/socket_setting.ini";
    QFileInfo fileInfo(ini_filepath);
    if (!fileInfo.isFile()) {
        qDebug() << "not found";
        settings = new QSettings(ini_filepath, QSettings::IniFormat);
    } else {
        settings = new QSettings(ini_filepath, QSettings::IniFormat);
        QStringList grouplist = settings->childGroups();

        for (QString group: grouplist) {
            vector <string> ini_params;
            ini_params.clear();
            settings->beginGroup(group);
            QStringList keylist = settings->childKeys();
            for (QString key: keylist) {
                ini_params.push_back(settings->value(key).toString().toStdString());
            }
            const char *port_num = ini_params[1].c_str();
            char *protocol = (char *)malloc(ini_params[2].size()+1);
            strcpy(protocol,&ini_params[2][0]);
            char *ip= (char *)malloc(ini_params[0].size()+1);
            strcpy(ip,&ini_params[0][0]);
            Socketchannel *socket = new Socketchannel(protocol, ip, std::atoi(port_num));
            if (socket->SocketDetect() == 1) {
                socket->SocketCreate();
                socket->SocketConnect();
            } else {
                cout << "ERROR in ";
                socket->printinfo();
            }
            //socket->connect_test(2000);
            current_socket.push_back(socket);
            settings->endGroup();
        }
        for(Socketchannel *socket:current_socket){
            socket->printinfo();
        }

        emit deletethread();
        port1 = new Socketdata(current_socket[0], 1);
        port2 = new Socketdata(current_socket[1], 2);
        port3 = new Socketdata(current_socket[2], 3);
        connect(port1, &Socketdata::trigger, this, &SerialPort::senddata_toserial);
        connect(port2, &Socketdata::trigger, this, &SerialPort::senddata_toserial);
        connect(port3, &Socketdata::trigger, this, &SerialPort::senddata_toserial);
        connect(this, &SerialPort::deletethread, port1, &Socketdata::deleteself);
        connect(this, &SerialPort::deletethread, port2, &Socketdata::deleteself);
        connect(this, &SerialPort::deletethread, port3, &Socketdata::deleteself);
        if(strcmp(current_socket[0]->protocol,"TCP")==0){
            port1->start();
        }
        if(strcmp(current_socket[1]->protocol,"TCP")==0){
            port2->start();
        }
        if(strcmp(current_socket[2]->protocol,"TCP")==0){
            port3->start();
        }
    }
}

/**
* @brief SerialPort 成员函数 on_SettingSocketButton()
* @details socket管理界面开启与关闭
* @param
* @return none
* @warning none
* @note none
*/
void SerialPort::on_SettingSocketButton() {
    socketset->show();
}

/**
* @brief SerialPort 成员函数 socket_setting_fun
* @details UI界面控价图标初始化 使用QIcon创建对象，在QPushbutton上显示
* @param none
* @return none
* @warning none
* @note none
*/
void SerialPort::socket_setting_fun() {
    QIcon ico(":/img/img/shezhi.png");
    ui->setting_btn->setIcon(ico);
    ui->setting_btn->setIconSize(QSize(40, 40));
    ui->setting_btn->setFlat(true);
    connect(ui->setting_btn, &QPushButton::clicked, this, &SerialPort::on_SettingSocketButton);

    QIcon ico_battery(":/img/img/Battary.png");
    ui->battery_btn->setIcon(ico_battery);
    ui->battery_btn->setIconSize(QSize(40, 40));
    ui->battery_btn->setFlat(true);

    QIcon ico_4G(":/img/img/signal1.png");
    ui->G4_btn->setIcon(ico_4G);
    ui->G4_btn->setIconSize(QSize(40, 70));
    ui->G4_btn->setFlat(true);

    QIcon ico_eth1(":/img/img/ethernet.png");
    ui->Eth1_btn->setIcon(ico_eth1);
    ui->Eth1_btn->setIconSize(QSize(40, 40));
    ui->Eth1_btn->setFlat(true);


    QIcon ico_eth2(":/img/img/ethernet.png");
    ui->Eth2_btn->setIcon(ico_eth2);
    ui->Eth2_btn->setIconSize(QSize(40, 40));
    ui->Eth2_btn->setFlat(true);

    QIcon ico_serial(":/img/img/serial.png");
    ui->serial_btn->setIcon(ico_serial);
    ui->serial_btn->setIconSize(QSize(40, 40));
    ui->serial_btn->setFlat(true);

    QIcon ico_off(":/img/img/power off.png");
    ui->Poweroff_btn->setIcon(ico_off);
    ui->Poweroff_btn->setIconSize(QSize(40, 40));
    ui->Poweroff_btn->setFlat(true);

}

/**
* @brief Serialport成员函数
* @details 给initiallize_socket设定计时器，即主程序每过指定时间执行一次initialize_socket
* @param int ms
* @return none
* @warning none
* @note none
*/
void SerialPort::reconnect_byclock(int ms) {
    QTimer *timer_reconnect = new QTimer(this);
    connect(timer_reconnect, &QTimer::timeout, this, QOverload<>::of(&SerialPort::initialize_socket));
    timer_reconnect->start(ms);
}

/**
* @brief SerialPort 成员函数 on_serial_btn_clicked
* @details 管理通信接口的开启与关闭
* @param none
* @return none
* @warning none
* @note none
*/
void SerialPort::on_serial_btn_clicked() {
    if (serial->isOpen()) {
        serial->close();
        ui->DataReceived->clear();
    } else {
        serial->setPortName("ttyS3");
        //用ReadWrite 的模式尝试打开串口，无法收发数据时，发出警告
        if (!serial->open(QIODevice::ReadWrite)) {
            QMessageBox::warning(this, tr("提示"), tr("串口打开失败!"), QMessageBox::Ok);
            return;
        }
    }

}

/**
* @brief SerialPort 成员函数 on_Poweroff_btn_clicked
* @details 退出整个程序
* @param none
* @return none
* @warning none
* @note none
*/
void SerialPort::on_Poweroff_btn_clicked() {
    qApp->quit();

}

/**
* @brief SerialPort 成员函数 eth1_judge
* @details 从配置文件读取网口一设备的IP，通过QProcess调用终端ping该地址，如果能ping通网口一的图标变亮，反之，变灰。
* @param none
* @return none
* @warning none
* @note none
*/
void SerialPort::eth1_judge() {
    QProcess *cmd = new QProcess();
    QString A_ip = QString::fromStdString(serial_params[15]);
    QString ping = "ping ";
    QString pingA = ping + A_ip;
    cmd->start(pingA);
    while (cmd->waitForFinished(20) == false) {
        QString result = QString::fromLocal8Bit(cmd->readAll());
        {
            //qDebug()<<result;
            if (result.indexOf(A_ip) != -1) {
                //qDebug("online\n");
                ui->Eth1_btn->setDisabled(false);
                break;
            } else {
                //qDebug("offline\n");
                ui->Eth1_btn->setDisabled(true);
                break;

            }
        }
    }
}

/**
* @brief SerialPort 成员函数 eth2_judge
* @details 从配置文件读取网口二设备的IP，通过QProcess调用终端ping该地址，如果能ping通网口一的图标变亮，反之，变灰。
* @param none
* @return none
* @warning none
* @note none
*/
void SerialPort::eth2_judge() {
    QProcess *cmd = new QProcess();
    QString A_ip = QString::fromStdString(serial_params[16]);
    QString ping = "ping ";
    QString pingA = ping + A_ip;
    cmd->start(pingA);
    while (cmd->waitForFinished(20) == false) {
        QString result = QString::fromLocal8Bit(cmd->readAll());
        {
            if (result.indexOf(A_ip) != -1) {
                ui->Eth2_btn->setDisabled(false);
                break;
            } else {
                ui->Eth2_btn->setDisabled(true);
                break;
            }
        }
    }
}

/**
* @brief SerialPort成员函数 get_signal
* @details 初始化4G信号图标，从检测4G信号强度的子线程获取信号强度，信号强度的数值为0-31.
 * 根据获得的值分为4档，每一档信号有不同的UI显示。
* @param none
* @return none
* @warning none
* @note none
*/
void SerialPort::get_signal(int csq) {
    //qDebug() << "Signal is: " << csq << endl;
    QIcon *ico_4G;
    ui->G4_btn->setIconSize(QSize(40, 40));
    ui->G4_btn->setFlat(true);
    if (csq < 8) {
        ico_4G = new QIcon(":/img/img/signal1.png");
    } else if (csq >= 8 && csq < 16) {
        ico_4G = new QIcon(":/img/img/signal2.png");
    } else if (csq >= 16 && csq < 24) {
        ico_4G = new QIcon(":/img/img/signal3.png");
    } else {
        ico_4G = new QIcon(":/img/img/signal4.png");
    }
    ui->G4_btn->setIcon(*ico_4G);
    ui->signal_strength->setText(QString::number(csq));
}







