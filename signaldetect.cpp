/**
 * @file signaldetect.cpp
 * @author wujunzhe/302538094@qq.com
 * @version 1.0.1
 * @date 2023.3.19
 * @brief
 * @details 无详细说明
 * @note 描述需要注意的问题
 */

#include "signaldetect.h"

/**
* @brief Signaldetect构造函数
* @details none
* @param none
* @return none
* @warning none
* @note none
*/
Signaldetect::Signaldetect() {


}

/**
* @brief run（）函数
* @details run（）函数首先打开4G模块通信串口openport（），并进行串口设置setport（），通过向串口发送“CSQ”
 * 获取信号强度，并且通过信号函数sendsignals（int strength）向主线程发送信号强度
* @param QWidget*
* @return none
* @warning none
* @note none
*/
void Signaldetect::run() {
    int ret = 0;
    char csq[32] = {0};
    int str_fd_ttyUSB2 = openport("/dev/ttyUSB2");     //打开串口
    if (str_fd_ttyUSB2 > 0) {
        ret = setport(str_fd_ttyUSB2);  //设置串口，波特率，数据位，停止位，校验
    }
    while (str_fd_ttyUSB2 > 0) {
        write(str_fd_ttyUSB2, "AT+CSQ\r\n", 12);//开启定位
        sleep(2);
        char rbuf[64];
        int len = read(str_fd_ttyUSB2, rbuf, sizeof(rbuf));  // 在串口读入字符串
        sleep(1);
        if (len < 0) {
            qDebug("Can't get /dev/ttyUSB2 Serial Port data!\n");
        }
        char *str2 = "+CSQ: ";
        char *pdata = strstr(rbuf, str2);

        if (pdata)
            strncpy(csq, pdata + 6, 6);
        else {

        }
        //qDebug("SIM-CSQ: %s\n", csq);
        if (!QString(csq).isEmpty()) {
            QString res = QString(csq).split(",")[0];

            //qDebug() << "CSQ:" << res << endl;
            this->sendsignals(res.toInt());
        }


    }



//    int ret = 0;
//    char csq[32] = {0};
//    int str_fd_ttyUSB2=open("/dev/ttyUSB2",O_RDWR|O_NOCTTY|O_NDELAY);
//    //打开串口
//    qDebug()<<"FD:"<<str_fd_ttyUSB2<<endl;

//    /*ret=setport(str_fd_ttyUSB2); */ //设置串口，波特率，数据位，停止位，校验
//    int baudrate;
//    struct   termios   newtio;
//    baudrate=B115200;//波特率
//    tcgetattr(str_fd_ttyUSB2,&newtio);
//    bzero(&newtio,sizeof(newtio));
//    //setting   c_cflag
//    newtio.c_cflag   &=~CSIZE;
//    newtio.c_cflag |= CS8; //8位数据位
//    //校验
//    newtio.c_cflag &= ~PARENB;   // Clear parity enable
//    newtio.c_iflag &= ~INPCK;     // Enable parity checking
//    //设置停止位
//    newtio.c_cflag &= ~CSTOPB;  //1
//    newtio.c_cc[VTIME] = 0;
//    newtio.c_cc[VMIN] = 0;
//    newtio.c_cflag   |=   (CLOCAL|CREAD);
//    newtio.c_oflag|=OPOST;
//    newtio.c_iflag   &=~(IXON|IXOFF|IXANY);
//    cfsetispeed(&newtio,baudrate);
//    cfsetospeed(&newtio,baudrate);
//    tcflush(str_fd_ttyUSB2,   TCIFLUSH);
//    qDebug()<<"Set\n"<<endl;



//    qDebug("Openserial!\n");


//    write(str_fd_ttyUSB2,"AT+CSQ\r\n",12);//开启定位
//    sleep(2);
//    char rbuf[64];
//    int len = read(str_fd_ttyUSB2, rbuf, sizeof(rbuf));  // 在串口读入字符串
//    sleep(1);
//    if(len < 0)
//    {
//    qDebug("Can't get /dev/ttyUSB2 Serial Port data!\n");
//    }

//    char *str2 = "+CSQ: ";
//    char *pdata = strstr(rbuf, str2);
//        if(pdata)
//        strncpy(csq, pdata+6, 6);
//        else {
//            qDebug("No data\n");
//        }
//    qDebug("SIM-CSQ: %s\n", csq);

}

/**
* @brief Signaldetect成员函数 openport
* @details 该函数用来处理打开串口的请求
* @param char *Dev
* @return int fd
* @warning none
* @note none
*/
int Signaldetect::openport(char *Dev) {
    int fd = open(Dev, O_RDWR | O_NOCTTY | O_NDELAY);
    if (-1 == fd) {
        perror("Can''t Open Serial Port");
        return -1;
    } else
        return fd;

}

/**
* @brief Signdetect成员函数 setport
* @details 该函数用来为串口配置基本参数
* @param int fd
* @return int n
* @warning none
* @note none
*/
int Signaldetect::setport(int fd) {
    int baudrate;
    struct termios newtio;
    baudrate = B115200;//波特率
    tcgetattr(fd, &newtio);
    bzero(&newtio, sizeof(newtio));
    //setting   c_cflag
    newtio.c_cflag &= ~CSIZE;
    newtio.c_cflag |= CS8; //8位数据位
    //校验
    newtio.c_cflag &= ~PARENB;   // Clear parity enable
    newtio.c_iflag &= ~INPCK;     // Enable parity checking
    //设置停止位
    newtio.c_cflag &= ~CSTOPB;  //1
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 0;
    newtio.c_cflag |= (CLOCAL | CREAD);
    newtio.c_oflag |= OPOST;
    newtio.c_iflag &= ~(IXON | IXOFF | IXANY);
    cfsetispeed(&newtio, baudrate);
    cfsetospeed(&newtio, baudrate);
    tcflush(fd, TCIFLUSH);
    if (tcsetattr(fd, TCSANOW, &newtio) != 0) {
        perror("SetupSerial 3");
        return -1;
    }
    return 0;
}





