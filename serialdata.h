#ifndef SERIALDATA_H
#define SERIALDATA_H

#include<QString>
#include <QStringList>
#include<vector>
class Serialdata
{
public:
    Serialdata();
    //报文中包含的各种数据的定义
    QString Stationcode;
    QString Sensortotal;
    QString Sensornum;
    std::vector<QString> Sensordata;
    QString Batterynum;
    QString Batteryvoltage;
    QString Errorcode;
    QString Messagenum;
    QString Rainfall;
    QString Waterlevel;
    QString Flowvelocity;
    QString Flow;
    QString Totalflow;
    QStringList FormatData(QString data);


};

#endif // SERIALDATA_H
