#include "serialport.h"
#include "serialdata.h"
#include "ui_serialport.h"
#include<QDebug>
#include<QMessageBox>
#include<ctime>
SerialPort::SerialPort(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SerialPort)
{
    //UI设置分辨率 串口初始化 图标初始化
    ui->setupUi(this);
    resize(QSize(800,600));
    serial = new QSerialPort;
    SerialPortInit();
    socket_setting_fun();



    //初始化socket参数
    initialize_socket();


    //set in to initial socket function
    //断线后10分钟重连
    reconnect_byclock(600000);
    //SOCKET三个端口
    port1=nullptr;
    port2=nullptr;
    port3=nullptr;
    //实例化检测4G信号强度的类,打开信号强度检测的子进程
    Signaldetect *a=new Signaldetect();
    connect(a,&Signaldetect::sendsignals,this,&SerialPort::get_signal);
    a->start();
}
//析构函数
SerialPort::~SerialPort()
{
    serial->clear();
    serial->close();
    delete ui;
}


void SerialPort::SerialPortInit()
{
    //设置了三个定时器，分别是刷新时间，检测两个网口是否在线
    timer1 = startTimer(1000);
    timer2 = startTimer(6000);
    timer3 = startTimer(6000);

    //设置串口名称，波特率57600 数据为 停止位等参数
    serial->setPortName("ttyS3");
    serial->setBaudRate(QSerialPort::Baud57600);
    serial->setParity(QSerialPort::NoParity);
    serial->setDataBits(QSerialPort::Data8);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    //读写模式打开串口
    if(!serial->open(QIODevice::ReadWrite))
    {
        QMessageBox::warning(this,tr("提示"),tr("串口打开失败!"),QMessageBox::Ok);
        return;
    }
    // 设置雨量旁的时间显示及换行
    ui->Messagetime->setWordWrap(true);
    ui->Messagetime->setAlignment(Qt::AlignLeft);
    //设置数据显示的最大块数
    ui->DataReceived->setMaximumBlockCount(30);
    //串口数据读取函数
    connect(serial,&QSerialPort::readyRead,this,&SerialPort::slotReadData);
    //设置控件的边框
    ui->Leftwindow->setLineWidth(1);
    ui->Leftwindow->setMidLineWidth(1);
    ui->Leftwindow->setFrameShape(QFrame::Box);
    ui->Leftwindow->setFrameShadow(QFrame::Raised);


   //读取serial_setting配置文件
    QString serialini_filepath=QCoreApplication::applicationDirPath()+"/serial_setting.ini";
    QFileInfo fileInfo(serialini_filepath);
    if(!fileInfo.isFile()){
        qDebug()<<"not found";
        serial_settings = new QSettings(serialini_filepath,QSettings::IniFormat);
    }else
    {
        serial_settings = new QSettings(serialini_filepath,QSettings::IniFormat);
        serial_settings->setIniCodec(QTextCodec::codecForName("UTF-8"));
        QStringList grouplist = serial_settings->childGroups();
        for(QString group:grouplist)
        {
            serial_settings->beginGroup(group);
            QStringList keylist=serial_settings->childKeys();
            for(QString key:keylist)
            {
                serial_params.push_back(serial_settings->value(key).toString().toStdString());
            }
            serial_settings->endGroup();

        }
        //根据配置文件读入结果来进行显示
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
        //判断两个网口是否连接
        eth1_judge();
        eth2_judge();


    }

}

//串口数据读取函数
void SerialPort::slotReadData()
{
    static QByteArray sumData;
    //从串口缓存里读取数据
    QByteArray tempData = serial->readAll();
    if(!tempData.isEmpty())
    {
        sumData.append(tempData);
        //qDebug()<<sumData<<endl;
        //没有匹配帧尾，sunData一直添加tempData
        //以\r\n作为报文结尾
        if(sumData.contains("\r\n"))
        {
            //数据显示
            DataReceived(sumData);
            //数据处理
            DataProcess(sumData);
            //发送给SOCKET
            senddata_toserver(sumData.simplified());
            //每一条报文结束都清除掉缓存
            sumData.clear();
        }
    }
    tempData.clear();
}
//数据显示
void SerialPort::DataReceived(QByteArray data)
{
    if(!data.isEmpty())
    {
        //进行日志数据的显示
        QString str = ui->DataReceived->toPlainText();
        data = data.simplified();
        //添加时间戳
        QDateTime timecur = QDateTime::currentDateTime();
        QString timestr = timecur.toString("[yy-MM-dd hh:mm:ss]");
        str+=timestr;
        str += tr("收到传感器串口数据: \n"+data);
        //str += tr(data);
        //qDebug("Info:%s",data.toStdString().data());
        ui->DataReceived->appendPlainText(str);
        //ui->DataReceived->clear();
    }
}

//报文数据解析

void SerialPort::DataProcess(QByteArray data)
{
    //实例化Serialdata 对象
    Serialdata* serialdata = new Serialdata();
    //QByteArray 转 QString 方便处理
    QString serialstr = QString(data);
    //以：进行字符串拆分
    QStringList list = serialstr.split(":");
    if(list[0]=="Port1")
    {
        ui->DataReceived->appendPlainText("发送配置平台\n");
    }
    else if(list[0]=="Port2")
    {
        ui->DataReceived->appendPlainText("发送数据平台\n");
    }
    else if(list[0]=="Port3")
    {
        ui->DataReceived->appendPlainText("发送数据推送\n");
    }
    //对应数据的添加
    serialdata->Stationcode.append(list[2]);
    serialdata->Sensortotal.append(list[3]);
    int tmp=list[3].toInt();
    //根据传感器数量进行循环
    for(int i=0;i<tmp;i++)
    {
        QStringList timelist=list[5+tmp].split(" ");
        timelist[1].replace("-",":");
        QString Messagetime = timelist.join(" ");

        ui->Messagetime->setText(Messagetime);

        QString temp=list[4+i];
        QStringList temp_spilt = temp.split(" ");
        serialdata->Sensornum.append(temp_spilt[0]);
        serialdata->Errorcode.append(temp_spilt[1]);
        //数据部分使用Formatdata函数处理
        QStringList strlist= serialdata->FormatData(temp_spilt[2]);

        serialdata->Sensordata.push_back(strlist.join(" "));

        QStringList battery_spilt = list[4+tmp].split(" ");
        serialdata->Batterynum = battery_spilt[0];
        serialdata->Batteryvoltage = serialdata->FormatData(battery_spilt[2])[0];


         ui->TNS->setText(serialdata->Stationcode);
         ui->Batteryvoltage->setText(serialdata->Batteryvoltage);
         std::string str = serialdata->Sensornum.toStdString();
        //str代表传感器编号将他与配置文件进行比对，从而将数据送入对应的显示框

         if(str==serial_params[0])
         {
             serialdata->Rainfall=serialdata->Sensordata.back();
             ui->Rainfall->setText(serialdata->Rainfall);
         }
         else if(str==serial_params[3])
         {
             serialdata->Waterlevel = serialdata->Sensordata.back();
             ui->Waterlevel->setText(serialdata->Waterlevel);

         }
         else if(str==serial_params[6])
         {
             serialdata->Flowvelocity= serialdata->Sensordata.back();
             ui->Flowvelocity->setText(serialdata->Flowvelocity);

         }
         else if(str==serial_params[9])
         {
             serialdata->Flow= serialdata->Sensordata.back();
             ui->Flow->setText(serialdata->Flow);

         }

         else if(str==serial_params[12])
         {
             serialdata->Totalflow=serialdata->Sensordata.back();
             ui->Totalflow->setText(serialdata->Totalflow);

         }
         else
         {

         }
         serialdata->Sensornum.clear();
    }

}


void SerialPort::timeUpdate()
{
    //实时时间的获取
    QFont font("Micsoft Yahei", 8,50);
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy-MM-dd hh:mm:ss");
    ui->Timeshow->setFont(font);
    ui->Timeshow->setText(str);
}

void SerialPort::timerEvent(QTimerEvent *event)
{
    //重写timeevent根据不同的定时器来触发不同的时间
    if(event->timerId()==timer1)
    {
        //刷新时间
        timeUpdate();
    }
    if(event->timerId()==timer2)
    {
        //判断网口一的连接
        eth1_judge();
    }
    if(event->timerId()==timer3)
    {
        //判断网口二的连接
        eth2_judge();
    }
}

void SerialPort::senddata_toserial(QByteArray data)
{
    //从socket获取数据后，将数据发送至串口
    //添加时间戳
    //qDebug("str:%s",data.toStdString().data());
    QDateTime timecur = QDateTime::currentDateTime();
    QString timestr = timecur.toString("[yy-MM-dd hh:mm:ss]");
    ui->DataReceived->appendPlainText(timestr+"收到平台数据\n未解析\n发送至传感器串口\n");
    serial->write(data);
}

void SerialPort::senddata_toserver(QByteArray data){

    qDebug()<<data<<endl;
    int i=data[4]-49;
    if(!current_socket.empty()&&current_socket[i]->connectstatus>=0){
        this->current_socket[i]->SendMessage(QString(data+"\r\n").toUtf8().data());
        //if(this->current_socket[0]->RecieveMessage()){
        //    getdata_fromserver("ok");
        //}
    }
}


void SerialPort::initialize_socket(){
    socketset=new Socketset();
    connect(socketset,SIGNAL(reinitial_socket_fromserial()),this,SLOT(initialize_socket()));
    current_socket.clear();
    QString ini_filepath=QCoreApplication::applicationDirPath()+"/socket_setting.ini";
    QFileInfo fileInfo(ini_filepath);
    if(!fileInfo.isFile()){
        qDebug()<<"not found";
        settings = new QSettings(ini_filepath,QSettings::IniFormat);
    }else{
        settings = new QSettings(ini_filepath,QSettings::IniFormat);
        QStringList grouplist = settings->childGroups();
        for(QString group:grouplist){
            vector<string> ini_params;
            settings->beginGroup(group);
            QStringList keylist=settings->childKeys();
            for(QString key:keylist){
                ini_params.push_back(settings->value(key).toString().toStdString());
            }
            const char *port_num= ini_params[1].c_str();
            Socketchannel *socket = new Socketchannel(&ini_params[2][0],&ini_params[0][0],std::atoi(port_num));
            if(socket->SocketDetect()==1){
                socket->SocketCreate();
                socket->SocketConnect();
            }else{
                cout<<"ERROR!"<<endl;
            }
            //socket->connect_test(2000);
            current_socket.push_back(socket);
            settings->endGroup();
        }

        emit deletethread();

        port1=new Socketdata(current_socket[0],1);
        port2=new Socketdata(current_socket[1],2);
        port3=new Socketdata(current_socket[2],3);
        connect(port1,&Socketdata::trigger,this,&SerialPort::senddata_toserial);
        connect(port2,&Socketdata::trigger,this,&SerialPort::senddata_toserial);
        connect(port3,&Socketdata::trigger,this,&SerialPort::senddata_toserial);
        connect(this,&SerialPort::deletethread,port1,&Socketdata::deleteself);
        connect(this,&SerialPort::deletethread,port2,&Socketdata::deleteself);
        connect(this,&SerialPort::deletethread,port3,&Socketdata::deleteself);
        port1->start();
        port2->start();
        port3->start();
    }
}

void SerialPort::on_SettingSocketButton(){
    socketset->show();
}
void SerialPort::socket_setting_fun(){
    //设置界面上的图标
    QIcon ico(":/img/img/shezhi.png");
    ui->setting_btn->setIcon(ico);
    ui->setting_btn->setIconSize(QSize(40,40));
    ui->setting_btn->setFlat(true);
    connect(ui->setting_btn,&QPushButton::clicked,this,&SerialPort::on_SettingSocketButton);

    QIcon ico_battery(":/img/img/Battary.png");
    ui->battery_btn->setIcon(ico_battery);
    ui->battery_btn->setIconSize(QSize(40,40));
    ui->battery_btn->setFlat(true);

    QIcon ico_4G(":/img/img/signal1.png");
    ui->G4_btn->setIcon(ico_4G);
    ui->G4_btn->setIconSize(QSize(40,70));
    ui->G4_btn->setFlat(true);

    QIcon ico_eth1(":/img/img/ethernet.png");
    ui->Eth1_btn->setIcon(ico_eth1);
    ui->Eth1_btn->setIconSize(QSize(40,40));
    ui->Eth1_btn->setFlat(true);


    QIcon ico_eth2(":/img/img/ethernet.png");
    ui->Eth2_btn->setIcon(ico_eth2);
    ui->Eth2_btn->setIconSize(QSize(40,40));
    ui->Eth2_btn->setFlat(true);

    QIcon ico_serial(":/img/img/serial.png");
    ui->serial_btn->setIcon(ico_serial);
    ui->serial_btn->setIconSize(QSize(40,40));
    ui->serial_btn->setFlat(true);

    QIcon  ico_off(":/img/img/power off.png");
    ui->Poweroff_btn->setIcon(ico_off);
    ui->Poweroff_btn->setIconSize(QSize(40,40));
    ui->Poweroff_btn->setFlat(true);

}

void SerialPort::reconnect_byclock(int ms){
    QTimer *timer_reconnect=new QTimer(this);
    connect(timer_reconnect,&QTimer::timeout,this,QOverload<>::of(&SerialPort::initialize_socket));
    timer_reconnect->start(ms);
}



void SerialPort::on_serial_btn_clicked()
{
    //打开或者关闭串口
    if(serial->isOpen())
    {
        serial->close();
        ui->DataReceived->clear();
    }
    else {
        serial->setPortName("ttyS3");
        //用ReadWrite 的模式尝试打开串口，无法收发数据时，发出警告
        if(!serial->open(QIODevice::ReadWrite))
        {
            QMessageBox::warning(this,tr("提示"),tr("串口打开失败!"),QMessageBox::Ok);
            return;
         }

    }

}

void SerialPort::on_Poweroff_btn_clicked()
{
    //退出整个进程
    qApp->quit();

}

void SerialPort::eth1_judge()
{
    //判断网口是否在线
    //调用一个新的process打开终端
    QProcess *cmd = new QProcess();
    //从配置文件读取IP地址
    QString A_ip = QString::fromStdString(serial_params[15]);
    QString ping = "ping ";
    QString pingA= ping+A_ip;
    //尝试能否ping通这个IP地址
    cmd->start(pingA);
    while(cmd->waitForFinished(20)==false)
    {
        QString result = QString::fromLocal8Bit(cmd->readAll());
        {
                //qDebug()<<result;
                if(result.indexOf(A_ip)!=-1)
                {
                   //qDebug("online\n");
                   //设备在线图标变亮
                   ui->Eth1_btn->setDisabled(false);
                   break;
                }
                else
                {
                    //qDebug("offline\n");
                    //设备不在线图标变灰
                    ui->Eth1_btn->setDisabled(true);
                    break;

                }
    }
    }



}

void SerialPort::eth2_judge()
{
    //判断网口是否在线
    //调用一个新的process打开终端
    QProcess *cmd = new QProcess();
    //从配置文件读取IP地址

    QString A_ip = QString::fromStdString(serial_params[16]);
    QString ping = "ping ";
    QString pingA= ping+A_ip;
    cmd->start(pingA);
    //尝试能否ping通这个IP地址
    while(cmd->waitForFinished(20)==false)
    {
        QString result = QString::fromLocal8Bit(cmd->readAll());
        {
                if(result.indexOf(A_ip)!=-1)
                {
                    //设备在线图标变亮
                   ui->Eth2_btn->setDisabled(false);
                   break;
                }
                else
                {
                    //设备不在线图标变灰
                    ui->Eth2_btn->setDisabled(true);
                    break;
                }
    }
    }

}

void SerialPort::get_signal(int csq)
{
    //获取4G模块信号强度
    qDebug()<<"Signal is: "<<csq<<endl;
    QIcon *ico_4G;
    ui->G4_btn->setIconSize(QSize(40,40));
    ui->G4_btn->setFlat(true);
    //信号强度分为4档，对应不同的状态图标显示
    if(csq<8){
        ico_4G=new QIcon(":/img/img/signal1.png");
    }else if(csq>=8 && csq <16){
        ico_4G=new QIcon(":/img/img/signal2.png");
    }else if(csq >=16 && csq <24){
        ico_4G=new QIcon(":/img/img/signal3.png");
    }else{
        ico_4G=new QIcon(":/img/img/signal4.png");
    }
    ui->G4_btn->setIcon(*ico_4G);
    ui->signal_strength->setText(QString::number(csq));
}







