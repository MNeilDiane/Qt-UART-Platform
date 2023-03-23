/**
 * @file serialport.h
 * @author wujunzhe/302538094@qq.com
 * @version 1.0.1
 * @date 2023.3.19
 * @brief
 * @details 无详细说明
 * @note 描述需要注意的问题
 */

#ifndef SOCKETDATA_H
#define SOCKETDATA_H

#include <QThread>
#include <socketchannel.h>
#include <QDebug>

/**
 *  @brief Socketdata类声明
 *  @details Socketdata继承于Qthread，在serialport.cpp中被实例化开启了三个子线程，其用于使三个Socketchannel对象
 *  收取三个不同服务器端发来的数据（由于Scoketchannel的RecieveMessage函数是阻塞的，并且如果服务器端断开则会引起程序崩溃，
 *  所以采用子线程来处理以上问题）
 *  @see 对应的源文件参见 socketdata.cpp
 */
class Socketdata : public QThread {
    Q_OBJECT
public:
    Socketdata(Socketchannel *current_socket, int num);

    virtual ~Socketdata();

    void run();

    char *data; ///< 保存服务器端发来的数据

    Socketchannel *current_socket; ///< 该子线程绑定的Socketchannel对象

public
    slots:
            void deleteself();
private:
    int num; ///< Socketchannel编号，用于区分不同的Socketchannel
    signals:
            void trigger(char * data,int num); ///< 信号函数，一旦该线程接收到服务器发来的数据就触发该函数，该信号函数对应的槽函数 @ref senddata_toserial(QByteArray data) serialport.h
};

#endif // SOCKETDATA_H
