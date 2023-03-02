#ifndef SERIALDATA_H
#define SERIALDATA_H

#include <QWidget>

class Serialdata : public QWidget
{
    Q_OBJECT
public:
    explicit Serialdata(QWidget *parent = nullptr);
    ~Serialdata();

    QString Stationcode;
    QString Sensortotal;
    QString Sensornum;
    QString Sensordata;
    QString Batterynum;
    QString Batteryvoltage;
    QString Errorcode;

signals:

public slots:
    QStringList FormatData(QString data);


};

#endif // SERIALDATA_H
