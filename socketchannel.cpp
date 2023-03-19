#include "socketchannel.h"


Socketchannel::Socketchannel(char *protocol,char* ip,int port){
    this->protocol=protocol;
    this->ip=ip;
    this->port=port;
}
int Socketchannel::SocketCreate(){
    if((socketfd=socket(AF_INET, SOCK_STREAM, 0))<0){
        cout<<"create failed.[ip:"<<this->ip<<",port:"<<this->port<<",protocol:"<<this->protocol<<"]"<<endl;
    }else{
        cout<<"create success.[ip:"<<this->ip<<",port:"<<this->port<<",protocol:"<<this->protocol<<"]"<<endl;
    }
    return socketfd;
}

int Socketchannel::SocketDetect(){
    int return_result=0;
    if((socketfd_test=socket(AF_INET, SOCK_STREAM, 0))<0){
        cout<<"testsocket has been created"<<endl;
    }else{
        cout<<"testsocket has not been created"<<endl;
    }
    int flags=fcntl(socketfd_test,F_GETFL,0);
    fcntl(socketfd_test,F_SETFL,flags|O_NONBLOCK);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &servaddr.sin_addr);
    if ((connectstatus=connect(socketfd_test, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0) {
        if(errno==EINPROGRESS){
            fd_set rset,wset;
            FD_ZERO(&rset);
            FD_SET(socketfd_test,&rset);
            wset=rset;
            struct timeval timeout;
            timeout.tv_sec=2;
            timeout.tv_usec=0;
            if(select(socketfd_test+1,&rset,&wset,NULL,&timeout)<=0){
                cout<<"testsocket has not been connected"<<endl;
            }else{

                return_result=1;
                close(socketfd_test);
            }
        }else{
            cout<<"testsocket has not been connected"<<endl;
        }
    }
    return return_result;
}

int Socketchannel::SocketConnect(){
    if ((connectstatus=connect(socketfd, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0) {
        cout<<"connect failed.[ip:"<<this->ip<<",port:"<<this->port<<",protocol:"<<this->protocol<<"]"<<endl;
    }else{
        cout<<"connect success.[ip:"<<this->ip<<",port:"<<this->port<<",protocol:"<<this->protocol<<"]"<<endl;
    }
}
void Socketchannel::SocketDisconnect() const{
    close(socketfd);
}
void Socketchannel::SendMessage(char *sendline) const{
    if (send(socketfd, sendline, strlen(sendline), 0) < 0) {
        cout<<"send failed."<<endl;
    }else{
        cout<<"send success."<<endl;
    }
}
char *Socketchannel::RecieveMessage() const{
    int result=0;
    char buf[1000];
    int recievelength;
    if ((recievelength = recv(socketfd, buf, MAXLINE, 0)) == 0) {

        return nullptr;
    }else {
        result=1;
    }
    buf[recievelength] = '\0';
    //printf("Received : %s\n", buf);
    char *a=buf;
    return a;
}

void Socketchannel::printinfo(){
    cout<<this->ip<<":"<<this->port<<":"<<this->protocol<<endl;
}

int Socketchannel::connect_test(int timeoutMs){
    int sockfd = -1;
        struct sockaddr_in servaddr;
        int flags = 0;

        if ((sockfd = socket(AF_INET, SOCK_STREAM , 0)) < 0)
        {
            printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
            return sockfd;
        }

        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(port);
        if (inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0)
        {
            printf("inet_pton error for %s\n", ip);
            close(sockfd);
            sockfd = -1;
            return sockfd;
        }

        //把链路设置为非阻塞
        flags = fcntl(sockfd, F_GETFL, 0);
        fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

        if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
        {
            if (errno != EINPROGRESS)       /* EINPROGRESS 表示连接正在建立的过程中 */
            {
                printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
                close(sockfd);
                sockfd = -1;
            }
            else
            {
                int ret;
                fd_set write_fds;
                struct timeval timeout;

                timeout.tv_sec = 0;
                timeout.tv_usec = 100 * 1000;       /* 连接超时时长：100ms */

                FD_ZERO(&write_fds);
                FD_SET(sockfd, &write_fds);

                ret = select(sockfd + 1, NULL, &write_fds, NULL, &timeout);
                switch (ret)
                {
                    case -1:        /* select错误 */
                    {
                        printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
                        close(sockfd);
                        sockfd = -1;
                        break;
                    }
                    case 0:         /* 超时 */
                    {
                        //FK_TRACE_WARN("select timeout...\n");
                        close(sockfd);
                        sockfd = -1;
                        break;
                    }
                    default:
                    {
                        int error = -1;
                        socklen_t optLen = sizeof(socklen_t);

                        getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char *)&error, &optLen);       /* 通过 getsockopt 替代 FD_ISSET 判断是否连接 */
                        if (error != 0)
                        {
                            printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
                            close(sockfd);
                            sockfd = -1;
                        }

                        break;
                    }
                }
            }
        }

        return sockfd;
}
