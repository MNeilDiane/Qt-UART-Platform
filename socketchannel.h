/**
 * @file socketchannel.h
 * @author wujunzhe/302538094@qq.com
 * @version 1.0.1
 * @date 2023.3.19
 * @brief
 * @details 无详细说明
 * @note 描述需要注意的问题
 */
#ifndef SOCKETCHANNEL_H
#define SOCKETCHANNEL_H

#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<cerrno>
//#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctime>
#include <string>
#include <iostream>
#include <vector>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <QDebug>

#define MAXLINE 4096
using namespace std;

/**
 *  @brief Socketchannel类声明
 *  @details Socketchannel类封装了一个Socket通道，其中包含该通道的创建、连接和销毁等操作
 *  @see 对应的源文件参见 socketchannel.cpp
 */
class Socketchannel {
public:
    Socketchannel(char *protocol, char *ip, int port);

    int SocketCreate();

    int SocketConnect();

    void SocketDisconnect() const;

    void SendMessage(char *sendline) const;

    void SendMessage(uint8_t *data,int data_len) const;

    char *RecieveMessage() const;

    char *printinfo();

    int connectstatus{}; ///< 连接状态，connect后将结果返回至该变量

    int SocketDetect();

    char *protocol; ///< 协议
private:
    int socketfd{}; ///< 建立连接后的句柄
    int socketfd_test; ///< none
    char *ip; ///< 服务器的公网IP
    int port; ///< 服务器监听的Socket端口号
    struct sockaddr_in servaddr{}; ///< 将protocol、ip、port封装在一起的结构体，用于连接时使用
    struct sockaddr_in *servaddr_udp; ///< server address saving for UDP
};

#endif // SOCKETCHANNEL_H
