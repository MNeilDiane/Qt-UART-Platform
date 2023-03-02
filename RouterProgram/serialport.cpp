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
    ui->setupUi(this);
    resize(QSize(800,600));
    SerialPortInit();
}

SerialPort::~SerialPort()
{
    delete ui;
}


void SerialPort::SerialPortInit()
{
    serial = new QSerialPort;
    QTimer *timer = new QTimer();
    //
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        serial->setPort(info);
        if(serial->open(QIODevice::ReadWrite))
        {
            ui->PortBox->addItem(info.portName());
            serial->close();
        }else
        {
            qDebug()<<"serial opened failed!";

        }

    }
    serial->setBaudRate(QSerialPort::Baud57600);
    serial->setParity(QSerialPort::NoParity);
    serial->setDataBits(QSerialPort::Data8);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    ui->DataReceived->setMaximumBlockCount(5);

    RefreshSerialPort(0);

    //connect(serial,&QSerialPort::readyRead,this,&SerialPort::DataReceived);
    connect(serial,&QSerialPort::readyRead,this,&SerialPort::slotReadData);
    connect(ui->SendButton,&QPushButton::clicked,this,&SerialPort::DataSend);
    connect(timer,SIGNAL(timeout()),this,SLOT(timeUpdate()));
    timer->start(1000);



}

void SerialPort::RefreshSerialPort(int index)
{
    QStringList portNameList;
    if(index!=0)
    {
        serial->setPortName(ui->PortBox->currentText());

    }
    else
    {
        ui->PortBox->clear();
        ui->PortBox->addItem("flash");
        foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
        {
            portNameList.append(info.portName());

        }
        ui->PortBox->addItems(portNameList);
        ui->PortBox->setCurrentIndex(1);
        serial->setPortName(ui->PortBox->currentText());
    }
}

void SerialPort::slotReadData()
{
    static QByteArray sumData;
    QByteArray tempData = serial->readAll();
    if(!tempData.isEmpty())
    {
        sumData.append(tempData);
        if(sumData.contains("END"))
        {
            DataReceived(sumData);
            DataProcess(sumData);
            sumData.clear();
        }
    }
    tempData.clear();
}

void SerialPort::DataReceived(QByteArray data)
{
    if(!data.isEmpty())
    {
        QString str = ui->DataReceived->toPlainText();
        str += tr(data);
        qDebug("Info:%s",data.toStdString().data());
        ui->DataReceived->clear();
        QDateTime timecur = QDateTime::currentDateTime();
        QString timestr = timecur.toString("[yy-MM-dd hh:mm:ss]");
        ui->DataReceived->appendPlainText(str+timestr+"\n");
        //qDebug()<<"Str info:"<<ui->DataReceived->toPlainText();
        //int index = str.indexOf("\r\n");
        //if(index!=-1)
        //{
        //    snprintf(BUF,500,"%s",str.left(index+2).toUtf8().data());
        //    qDebug()<<"BUF INFO:"<<BUF;
        //    str.remove(0,index+2);
       // }
    }
}


void SerialPort::DataSend()
{
    serial->write(ui->DataSend->toPlainText().toLatin1());

}

void  SerialPort::LED(bool changeColor)
{
    if(changeColor == false)
    {
        // 显示绿色
        ui->LED->setStyleSheet("background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0 rgba(0, 229, 0, 255), stop:1 rgba(255, 255, 255, 255));border-radius:12px;");
    }
    else
    {
        // 显示红色
        ui->LED->setStyleSheet("background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0 rgba(255, 0, 0, 255), stop:1 rgba(255, 255, 255, 255));border-radius:12px;");
    }
}


void SerialPort::on_ClearButton_clicked()
{

    ui->DataSend->setText("");
}


//void SerialPort::on_SendEditBtn1_clicked()
//{
//    on_ClearButton_clicked();
//    QString EditText = ui->Edit1->text();               //获取发送框内容
//    ui->DataSend->setText(EditText);
//}

void SerialPort::on_OpenSerialButton_clicked()
{
    if(serial->isOpen())                                        // 如果串口打开了，先给他关闭
        {
            serial->clear();
            serial->close();
            // 关闭状态，按钮显示“打开串口”
            ui->OpenSerialButton->setText("打开串口");
            // 关闭状态，允许用户操作
            // 禁止操作“发送字符操作”

            ui->SendButton->setDisabled(true);
            // 关闭状态，颜色为绿色
            ui->OpenSerialButton->setStyleSheet("color: green;");
            // 关闭，显示灯为红色
            LED(true);
            // 清空数据
            ui->DataReceived->clear();
            ui->DataSend->clear();
        }
        else                                                        // 如果串口关闭了，先给他打开
        {
            //当前选择的串口名字
            serial->setPortName(ui->PortBox->currentText());
            //用ReadWrite 的模式尝试打开串口，无法收发数据时，发出警告
            if(!serial->open(QIODevice::ReadWrite))
            {
                QMessageBox::warning(this,tr("提示"),tr("串口打开失败!"),QMessageBox::Ok);
                return;
             }
            // 打开状态，按钮显示“关闭串口”
            ui->OpenSerialButton->setText("关闭串口");
            // 打开状态，禁止用户操作
            // 允许操作“发送字符操作”
            ui->SendButton->setDisabled(false);
            // 打开状态，颜色为红色
            ui->OpenSerialButton->setStyleSheet("color: red;");
            // 打开，显示灯为绿色
            LED(false);
        }

}

void SerialPort::on_ClearShowButton_clicked()
{
   ui->DataReceived->setPlainText("");
}

void SerialPort::on_SendButton_clicked()
{
    //on_ClearButton_clicked();
    QString EditText = ui->DataSend->toPlainText();
    ui->DataSend->setText(EditText);
}
void SerialPort::DataProcess(QByteArray data)
{


    Serialdata* serialdata = new Serialdata();
    QString serialstr = QString(data);
    //qDebug("serialinfo:%s",serialstr.toStdString().data());
    QStringList list = serialstr.split(":");
    serialdata->Stationcode.append(list[1]);
    //qDebug("%s",serialdata->Stationcode.toStdString().data());
    serialdata->Sensortotal.append(list[2]);
    QString temp=list[3];
    QStringList temp_spilt = temp.split(" ");
    serialdata->Sensornum.append(temp_spilt[0]);
    serialdata->Errorcode.append(temp_spilt[1]);
    //QStringList strArray = temp_spilt[2].split("|");
    //qDebug("%s",strArray[1].mid(0,1).toStdWString().data());
    QStringList strlist= serialdata->FormatData(temp_spilt[2]);
    serialdata->Sensordata = strlist.join(" ");
    qDebug("%s",serialdata->Sensordata.toStdString().data());
    QStringList battery_spilt = list[4].split(" ");
    serialdata->Batterynum = battery_spilt[0];
    serialdata->Batteryvoltage = serialdata->FormatData(battery_spilt[2])[0];



    ui->TNS->setText(serialdata->Stationcode);
    ui->Sersornum->setText(serialdata->Sensortotal);
    ui->Sensordata->setText(serialdata->Sensordata);
    ui->Errorcode->setText(serialdata->Errorcode);
    ui->Batterynum->setText(serialdata->Batterynum);
    ui->Batteryvoltage->setText(serialdata->Batteryvoltage);

}

void SerialPort::timeUpdate()
{
    QFont font("Micsoft Yahei", 8,50);
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy-MM-dd hh:mm:ss");
    ui->Timeshow->setFont(font);
    ui->Timeshow->setText(str);
}
