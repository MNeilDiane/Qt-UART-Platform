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
#define MAXLINE 4096
using namespace std;
class Socketchannel{
public:
    Socketchannel(char *protocol,char* ip,int port);
    int SocketCreate();
    int SocketConnect();
    void SocketDisconnect() const;
    void SendMessage(char *sendline) const;
    char *RecieveMessage() const;
    void printinfo();
    int connectstatus{};
    int connect_test(int timeoutMs);
    int SocketDetect();
private:
    int socketfd{};
    int socketfd_test;
    char *protocol;
    char* ip;
    int port;
    struct sockaddr_in servaddr{};
};

#endif // SOCKETCHANNEL_H
