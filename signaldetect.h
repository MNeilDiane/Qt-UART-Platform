/**
 * @file signaldetect.h
 * @author wujunzhe/302538094@qq.com
 * @version 1.0.1
 * @date 2023.3.19
 * @brief
 * @details 无详细说明
 * @note 描述需要注意的问题
 */

#ifndef SIGNALDETECT_H
#define SIGNALDETECT_H

#include <QThread>
#include <QProcess>
#include <QDebug>
#include <QTimer>
#include<QtSerialPort>
#include<QtSerialPort/QSerialPortInfo>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <QMessageBox>

/**
 *  @brief Signaldetect类声明
 *  @details Signaldetect类继承自QThread，在Serialport中实例化一个子线程，该子线程会持续与4G模块进行
 *  串口通信，获取信号强度
 *  @see 对应的源文件参见 signaldetect.cpp
 */
class Signaldetect : public QThread {
    Q_OBJECT
public:
    Signaldetect();

    void run();///< 子线程run（）函数

    static int openport(char *Dev);///< 打开串口

    static int setport(int fd);///< 设置串口参数


    signals:
            void sendsignals(int
    strength);///< 信号函数 一旦查询到信号强度将触发该函数 该函数对应的槽函数 @ref void get_signal(int csq) serialport.h
private:
    QSerialPort *serial1;///< 与4G模块通信的串口
};

#endif // SIGNALDETECT_H
