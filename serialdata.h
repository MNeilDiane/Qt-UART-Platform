/**
 * @file serialdata.h
 * @author wujunzhe/302538094@qq.com
 * @version 1.0.1
 * @date 2023.3.19
 * @brief Class"Serialdata"的头文件，其中声明了Serialport类，
 * @details 无详细说明
 * @note 描述需要注意的问题
 */
#ifndef SERIALDATA_H
#define SERIALDATA_H

#include<QString>
#include <QStringList>
#include<vector>

/**
 *  @brief Serialdata类声明
 *  @details Serialdata是报文数据类，数据成员包括报文基本信息，在数据解析时实例化该类进行数据显示，成员函数FormatData（QString data）
 *  用来解析报文的数据部分
 *  @see 对应的源文件参见 serialdata.cpp
 */
class Serialdata {
public:
    Serialdata();

    QString Stationcode;///< 设备编号
    QString Sensortotal;///< 传感器数量
    QString Sensornum;///< 传感器编号
    std::vector <QString> Sensordata; ///< 传感器传输的数据部分，以vector<QString>存储
    QString Batterynum;///< 电池组编号
    QString Batteryvoltage;///< 电池组电压
    QString Errorcode;///< 错误编码
    QString Messagenum;///< 信息编号
    QString Rainfall;///< 雨量
    QString Waterlevel;///< 水位
    QString Flowvelocity;///< 流速
    QString Flow;///< 流量
    QString Totalflow;///< 累计流量
    QStringList FormatData(QString data);


};

#endif // SERIALDATA_H
