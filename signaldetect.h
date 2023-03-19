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

class Signaldetect:public QThread
{
    Q_OBJECT
public:
    Signaldetect();
    void run();

    void getoutput(int val);

    static int openport(char *Dev);

    static int setport(int fd);


signals:
    void sendsignals(int strength);
private:
    QSerialPort *serial1;
};

#endif // SIGNALDETECT_H
