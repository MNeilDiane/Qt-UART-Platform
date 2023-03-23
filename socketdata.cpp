/**
 * @file socketdata.cpp
 * @author wujunzhe/302538094@qq.com
 * @version 1.0.1
 * @date 2023.3.19
 * @brief
 * @details 无详细说明
 * @note 描述需要注意的问题
 */

#include "socketdata.h"

/**
* @brief Socketdata构造函数
* @details 将一个Socketchannel对象传入该构造函数，以便后期对服务器端发送的数据进行监听
* @param Socketchannel *current_socket
* @param int num
* @return none
* @warning none
* @note none
*/
Socketdata::Socketdata(Socketchannel *current_socket, int num) {
    this->current_socket = current_socket;
    this->num = num;
    connect(this, &Socketdata::finished, this, [=]() {
        delete this;
        qDebug() << "port" << num << " older thread has been destoryed[" << currentThreadId()
                 << "],please check the link,retry in 10 minutes later";
    });
}

/**
* @brief Socketdata成员函数，重写
* @details QThread子类会继承该函数，在该子类被实例化并调用start()后，会执行run()函数
* @param none
* @return none
* @warning none
* @note 注解
*/
void Socketdata::run() {
    while (1) {
        if (current_socket->connectstatus != 0) {
            break;
        } else {
            this->data = this->current_socket->RecieveMessage();
            if (this->data != nullptr) {
                this->trigger(this->data,num);
            } else {
                current_socket->connectstatus = -2;
            }
        }
    }
}

/**
* @brief Socketdata析构函数
* @details 对象被销毁后执行的函数
* @param none
* @return none
* @warning none
* @note 注解
*/
Socketdata::~Socketdata() {

}

/**
* @brief Socketdata成员函数deleteself
* @details 该函数是槽函数，对应的信号函数为@ref deletethread() serialport.h，当主程序定时器触发时、用户手动设置socket配置时会
* 重新调用@ref serialport.h中的initialize_socket()进行saocket重连，每次调用就会将之前开启的子线程删除掉，并开启新的线程以达到同步作用，本函数
* 的作用则是一旦重连则将之前的线程自我销毁
* @param none
* @return none
* @warning none
* @note none
*/
void Socketdata::deleteself() {
    this->terminate();
    this->wait();
    this->deleteLater();
}
