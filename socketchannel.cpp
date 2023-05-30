/**
 * @file socketchannel.cpp
 * @author wujunzhe/302538094@qq.com
 * @version 1.0.1
 * @date 2023.3.19
 * @brief
 * @details 无详细说明
 * @note 描述需要注意的问题
 */

#include "socketchannel.h"

/**
* @brief Socketchannel构造函数
* @details 该函数是构造函数的重写，作用是在实例化时获取用户想连接的服务器的基本参数（protocol、ip、port）以便进行以后的操作
* @param char *protocol
* @param char *ip
* @param int port
* @return none
* @warning none
* @note none
*/
Socketchannel::Socketchannel(char *protocol, char *ip, int port) {
    this->protocol = protocol;
    this->ip = ip;
    this->port = port;
}

/**
* @brief Socketchannel成员函数SocketCreate
* @details 该函数用于创建一个套接字，将socket句柄保存至成员变量socketfd中，socket(int ip_type,int trans_mode,int protocol)
* @param none
* @return int,将socketfd作为返回
* @retval >=0,创建成功
* @retval <0,创建失败
* @warning none
* @note none
*/
int Socketchannel::SocketCreate() {
    if(strcmp(this->protocol,"TCP")==0){
        socketfd = socket(AF_INET, SOCK_STREAM, 0);
    }else if(strcmp(this->protocol,"UDP")==0){
        socketfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    }
    if (socketfd < 0) {
        cout << "create failed.[ip:" << this->ip << ",port:" << this->port << ",protocol:" << this->protocol << "]"
             << endl;
    } else {
        cout << "create success.[ip:" << this->ip << ",port:" << this->port << ",protocol:" << this->protocol << "]"
             << endl;
    }
    return socketfd;
}

/**
* @brief Socketchannel成员函数SocketDetect
* @details 该函数用于处理套接字连接超时问题，套接字如果直接进行连接的话,会出现超时问题导致程序阻塞，所以
* 定义该函数提前检测该IP是否可达并且有监听，若确认该IP在对应的端口提供了socket的监听，再进行连接
* @param none
* @return int,返回检测结果return_result
* @retval 1,可以对该服务器进行进一步连接
* @retval 0,该服务器不可达，或没有提供socket服务，或链路有问题，不可进行连接
* @warning none
* @note none
*/
int Socketchannel::SocketDetect() {
    int return_result = 0;
    if(strcmp(this->protocol,"UDP")==0){
        servaddr_udp=(sockaddr_in *)malloc(sizeof(struct sockaddr_in));
//        servaddr_udp->sin_family=AF_INET;
//        servaddr_udp->sin_port=htons(port);
//        inet_pton(AF_INET, ip, &servaddr_udp->sin_addr);
        return_result=1;
    }else if(strcmp(this->protocol,"TCP")==0){
        if ((socketfd_test = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            cout << "[detectsocket has not been created]" << endl;
        } else {
            cout << "[detectsocket has been created]" << endl;
        }
        int flags = fcntl(socketfd_test, F_GETFL, 0);
        fcntl(socketfd_test, F_SETFL, flags | O_NONBLOCK);
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(port);
        inet_pton(AF_INET, ip, &servaddr.sin_addr);
        if ((connectstatus = connect(socketfd_test, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0) {
            if (errno == EINPROGRESS) {
                fd_set rset, wset;
                FD_ZERO(&rset);
                FD_SET(socketfd_test, &rset);
                wset = rset;
                struct timeval timeout;
                timeout.tv_sec = 2;
                timeout.tv_usec = 0;
                if (select(socketfd_test + 1, &rset, &wset, NULL, &timeout) <= 0) {
                    cout << "testsocket has not been connected" << endl;
                } else {
                    return_result = 1;
                }
            } else {
                cout << "testsocket has not been connected" << endl;
            }
        }
    }
    close(socketfd_test);
    return return_result;
}

/**
* @brief Socketchannel成员函数SocketConnect
* @details 进行套接字连接，在SocketCreate、SocketDetect通过后进行调用，连接成功将把连接结果存入connectstatus
* @param none
* @return int,返回连接状态connectstatus
* @retval >=0,连接成功
* @retval <0,连接失败
* @warning none
* @note none
*/
int Socketchannel::SocketConnect() {
    if(strcmp(this->protocol,"TCP")==0){
        if ((connectstatus = connect(socketfd, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0) {
            cout << "connect failed.[ip:" << this->ip << ",port:" << this->port << ",protocol:" << this->protocol << "]"
                 << endl;
        } else {
            cout << "connect success.[ip:" << this->ip << ",port:" << this->port << ",protocol:" << this->protocol << "]"
                 << endl;
        }
        return connectstatus;
    }else if(strcmp(this->protocol,"UDP")==0){
        cout << "UDP don't need connect,just send.[ip:" << this->ip << ",port:" << this->port << ",protocol:" << this->protocol << "]"<<endl;
        connectstatus=1;
        return connectstatus;
    }
}


/**
* @brief Scoketchannel成员函数SocketDisconnect
* @details 销毁该套接字，若需重连，需要重新创建并连接
* @param none
* @return none
* @warning none
* @note none
*/
void Socketchannel::SocketDisconnect() const {
    close(socketfd);
}

/**
* @brief Socketchannel成员函数SendMessage
* @details 在套接字成功连接后，可以通过该函数对服务器发送信息
* @param char *sendline
* @return none
* @warning none
* @note none
*/
void Socketchannel::SendMessage(char *sendline) const {

    if(strcmp(this->protocol,"TCP")==0){
        if (send(socketfd, sendline, strlen(sendline), 0) < 0) {
            qDebug()<<"send failed.";
        } else {
            qDebug()<<"send success.";
        }
    }else if(strcmp(this->protocol,"UDP")==0){
        servaddr_udp->sin_family=AF_INET;
        servaddr_udp->sin_port=htons(port);
        inet_pton(AF_INET, ip, &servaddr_udp->sin_addr);
        cout<<sendline<<endl;
        cout<<sizeof(sendline)<<endl;
        if(sendto(socketfd,sendline,strlen(sendline),0,reinterpret_cast<const sockaddr*>(servaddr_udp),sizeof (*servaddr_udp))!=-1){
            cout<<"UDP packet send success"<<endl;
        }else{
            cout<<errno<<endl;
        }
    }
}

void Socketchannel::SendMessage(uint8_t data[],int data_len) const {

    if(strcmp(this->protocol,"TCP")==0){
        if (send(socketfd, data, data_len, 0) < 0) {
            qDebug()<<"send failed.";
        } else {
            qDebug()<<"send success.";
        }
    }else if(strcmp(this->protocol,"UDP")==0){
        servaddr_udp->sin_family=AF_INET;
        servaddr_udp->sin_port=htons(port);
        inet_pton(AF_INET, ip, &servaddr_udp->sin_addr);
        //cout<<sendline<<endl;
        //cout<<sizeof(sendline)<<endl;
        if(sendto(socketfd,data,data_len,0,reinterpret_cast<const sockaddr*>(servaddr_udp),sizeof (*servaddr_udp))!=-1){
            qDebug()<<"UDP packet send success";
        }else{
            qDebug()<<errno;
        }
    }
}

/**
* @brief Socketchannel成员函数RecieveMessage
* @details 用于接收服务器端send函数发送的信息，服务器每一次发送的数据会存入buf中，该函数适用于循环中来监听
* 服务器端发送的数据，由于该函数是阻塞的所以被应用于子线程中，如果服务器端突然断开，该函数会一直返回nullptr
* @param none
* @return char *,将返回服务器端发送的字符串，如果服务器端突然断开，该函数会一直返回nullptr
* @warning none
* @note none
*/
char *Socketchannel::RecieveMessage() const {
    char buf[1000];
    int recievelength=recv(socketfd, buf, MAXLINE, 0);
    if (recievelength == 0) {
        return nullptr;
    }
    buf[recievelength] = '\0';
    cout<<recievelength<<endl;
    //printf("Received : %s\n", buf);
    char *a = buf;
    return a;
}

/**
* @brief Socketchannel成员函数prininfo
* @details 打印成员变量信息，用于调试
* @param none
* @return none
* @warning none
* @note none
*/
char * Socketchannel::printinfo() {
    cout << this->ip << ":" << this->port << ":" << this->protocol << endl;
    return this->ip;
}

