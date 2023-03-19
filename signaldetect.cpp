#include "signaldetect.h"



Signaldetect::Signaldetect()
{



}


void Signaldetect::run(){
    int ret = 0;
    char csq[32] = {0};
    int str_fd_ttyUSB2  =  openport("/dev/ttyUSB2");     //打开串口
    if(str_fd_ttyUSB2>0)
    {
       ret=setport(str_fd_ttyUSB2);  //设置串口，波特率，数据位，停止位，校验
    }
    while(str_fd_ttyUSB2>0)
    {
        write(str_fd_ttyUSB2,"AT+CSQ\r\n",12);//开启定位
        sleep(2);
        char rbuf[64];
        int len = read(str_fd_ttyUSB2, rbuf, sizeof(rbuf));  // 在串口读入字符串
        sleep(1);
        if(len < 0)
        {
        qDebug("Can't get /dev/ttyUSB2 Serial Port data!\n");
        }
        char *str2 = "+CSQ: ";//AT命令发送CSQ查询信号强度
        char *pdata = strstr(rbuf, str2);
        if(pdata)
        strncpy(csq, pdata+6, 6);
        else
        {

         }
        //qDebug("SIM-CSQ: %s\n", csq);
        //CSQ非空，转换为int发送给主进程
        if(!QString(csq).isEmpty())
        {
            QString res = QString(csq).split(",")[0];
            qDebug()<<"CSQ:"<<res<<endl;
            //sendsiganals信号函数发送信号
            this->sendsignals(res.toInt());
        }

    }


}
//打开4G模块的串口
int Signaldetect::openport(char *Dev)
{
    int fd = open( Dev, O_RDWR|O_NOCTTY|O_NDELAY );
    if (-1 == fd)
    {
    perror("Can''t Open Serial Port");
    return -1;
    }
    else
    return fd;

}
//设置4G模块串口的参数
int Signaldetect::setport(int fd)
{
    int baudrate;
    struct   termios   newtio;
    baudrate=B115200;//波特率
    tcgetattr(fd,&newtio);
    bzero(&newtio,sizeof(newtio));
    //setting   c_cflag
    newtio.c_cflag   &=~CSIZE;
    newtio.c_cflag |= CS8; //8位数据位
    //校验
    newtio.c_cflag &= ~PARENB;   // Clear parity enable
    newtio.c_iflag &= ~INPCK;     // Enable parity checking
    //设置停止位
    newtio.c_cflag &= ~CSTOPB;  //1
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 0;
    newtio.c_cflag   |=   (CLOCAL|CREAD);
    newtio.c_oflag|=OPOST;
    newtio.c_iflag   &=~(IXON|IXOFF|IXANY);
    cfsetispeed(&newtio,baudrate);
    cfsetospeed(&newtio,baudrate);
    tcflush(fd,   TCIFLUSH);
    if (tcsetattr(fd,TCSANOW,&newtio) != 0)
    {
    perror("SetupSerial 3");
    return -1;
    }
    return 0;
}





