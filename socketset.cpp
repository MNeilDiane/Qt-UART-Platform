/**
 * @file socketset.cpp
 * @author wujunzhe/302538094@qq.com
 * @version 1.0.1
 * @date 2023.3.19
 * @brief
 * @details 无详细说明
 * @note 描述需要注意的问题
 */
#include "socketset.h"
#include "ui_socketset.h"
#include <serialport.h>

/**
* @brief Socketset构造函数
* @details 该类会在Serialport初始化的时候初始化，并会通过show_ini()将程序文件夹下的配置文件读取到配置界面
* @param QWidget*
* @return none
* @warning none
* @note none
*/
Socketset::Socketset(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::Socketset) {
    ui->setupUi(this);
    show_ini();
    connect(ui->pushButton_cancel, &QPushButton::clicked, this, &Socketset::close);
    connect(ui->pushButton_save, &QPushButton::clicked, this, &Socketset::on_save);

}

/**
* @brief Socketset析构函数
* @details 对象销毁时运行的函数
* @param none
* @return none
* @warning none
* @note none
*/
Socketset::~Socketset() {
    delete ui;
}

/**
* @brief Socketset成员函数show_ini
* @details 将项目"Debug"目录下的socket_setting.ini文件读出，并显示在页面的控件上
* @param none
* @return none
* @warning none
* @note onne
*/
void Socketset::show_ini() {
    QSettings *settings;
    QLineEdit *lineedit_vector[3][2] = {{ui->lineEdit_1_ip, ui->lineEdit_1_port},
                                        {ui->lineEdit_2_ip, ui->lineEdit_2_port},
                                        {ui->lineEdit_3_ip, ui->lineEdit_3_port}};
    QComboBox *combox_vector[3] = {ui->comboBox_1, ui->comboBox_2, ui->comboBox_3};
    int count = 0;
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
            settings->beginGroup(group);
            QStringList keylist = settings->childKeys();
            for (QString key: keylist) {
                ini_params.push_back(settings->value(key).toString().toStdString());

            }

            lineedit_vector[count][0]->setText(QString::fromStdString(ini_params[0]));
            lineedit_vector[count][1]->setText(QString::fromStdString(ini_params[1]));
            combox_vector[count]->setCurrentText(QString::fromStdString(ini_params[2]));
            count++;
            //cout<<ini_params[0]<<ini_params[1]<<ini_params[2]<<endl;
            //const char *port_num= ini_params[1].c_str();
            //Socketchannel *socket = new Socketchannel(&ini_params[2][0],&ini_params[0][0],std::atoi(port_num));
            settings->endGroup();
        }

    }
}

/**
* @brief Scoketset成员函数on_save
* @details 作为"保存"按钮的槽函数，用户点击"保存"按钮后会将文本框里的IP、端口、协议覆盖至socket_setting.ini文件中
* @param none
* @return none
* @warning none
* @note none
*/
void Socketset::on_save() {
    QSettings *settings;
    QLineEdit *lineedit_vector[3][2] = {{ui->lineEdit_1_ip, ui->lineEdit_1_port},
                                        {ui->lineEdit_2_ip, ui->lineEdit_2_port},
                                        {ui->lineEdit_3_ip, ui->lineEdit_3_port}};
    QComboBox *combox_vector[3] = {ui->comboBox_1, ui->comboBox_2, ui->comboBox_3};
    QString ini_filepath = QCoreApplication::applicationDirPath() + "/socket_setting.ini";
    QFileInfo fileInfo(ini_filepath);
    if (!fileInfo.isFile()) {
        qDebug() << "not found";
        settings = new QSettings(ini_filepath, QSettings::IniFormat);
    } else {
        settings = new QSettings(ini_filepath, QSettings::IniFormat);
        QStringList grouplist = settings->childGroups();
        int count = 0;
        for (QString group: grouplist) {
            settings->beginGroup(group);
            settings->setValue("ip", lineedit_vector[count][0]->text());
            settings->setValue("port", lineedit_vector[count][1]->text());
            settings->setValue("protocol", combox_vector[count]->currentText());
            count++;
            settings->endGroup();
        }
    }
    reinitial_socket_fromserial();
    close();
}

void Socketset::set_edit(QString ip,QString port,QString protocol,int num){
    QLineEdit *lineedit_vector[3][2] = {{ui->lineEdit_1_ip, ui->lineEdit_1_port},
                                        {ui->lineEdit_2_ip, ui->lineEdit_2_port},
                                        {ui->lineEdit_3_ip, ui->lineEdit_3_port}};
    QComboBox *combox_vector[3] = {ui->comboBox_1, ui->comboBox_2, ui->comboBox_3};
    lineedit_vector[num][0]->setText(ip);
    lineedit_vector[num][1]->setText(port);
    combox_vector[num]->setCurrentText(protocol);
    on_save();
}

//void Socketset::pushButton_add(){
//    QHBoxLayout *info =new QHBoxLayout();

//    QLabel *iplabel = new QLabel(tr("ip:"));
//    QLineEdit *ipedit = new QLineEdit();

//    QLabel *portlabel = new QLabel(tr("port:"));
//    QLineEdit *portedit = new QLineEdit();

//    QLabel *protocollabel = new QLabel(tr("protocol:"));
//    QComboBox *protocolselect = new QComboBox();

//    QPushButton *route = new QPushButton();
//    route->setText("route");

//    QPushButton *deleteself = new QPushButton();
//    deleteself->setText("delete");

//    protocolselect->addItem(tr("TCP"));
//    protocolselect->addItem(tr("UDP"));
//    protocolselect->addItem(tr("MQTT"));

//    info->addWidget(iplabel);
//    info->addWidget(ipedit);
//    info->addWidget(portlabel);
//    info->addWidget(portedit);
//    info->addWidget(protocollabel);
//    info->addWidget(protocolselect);
//    info->addWidget(route);
//    info->addWidget(deleteself);

//    ui->verticalLayout->addLayout(info);
//    connect(route,&QPushButton::clicked,this,[=](){pushButton_route(ipedit->text()+","+portedit->text()+","+protocolselect->currentText());});
//    connect(deleteself,&QPushButton::clicked,this,[=](){pushButton_deleteself(info);});
//}

//void Socketset::pushButton_route(QString info){
//    QStringList a =info.split(",");
//    //string a =info.toUtf8().data();

//    Socketchannel *current_socket = new Socketchannel(0,a[0].toUtf8().data(),stoi(a[1].toUtf8().data()));
//    current_socket->SocketCreate();
//    current_socket->SocketConnect();
//    while(1){
//        current_socket->SendMessage("1234");
//        sleep(1);
//    }
//}



//void Socketset::pushButton_deleteself(QLayout *current_info){
//    //QPushButton *current_btn = qobject_cast<QPushButton *>(sender());
//    //ui->verticalLayout->removeItem(ui->verticalLayout->itemAt(0));
//    //delete current_btn;

//    //qDebug("%d", ui->verticalLayout->removeWidget(ui->verticalLayout->layout()->widget()));
//    //ui->verticalLayout->removeWidget(current_info->widget());
//    //qDebug("%S",ui->verticalLayout->removeWidget(current_info->widget()));
//    if(current_info==nullptr){
//        return;
//    }
//    QLayoutItem *child;
//    while(child=current_info->takeAt(0)){
//        if(child->widget()){
//            child->widget()->setParent(nullptr);
//        }
//        current_info->removeItem(child);
//        delete child;
//    }
//    current_info->update();
//    delete  current_info;

//}


