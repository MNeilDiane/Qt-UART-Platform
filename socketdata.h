#ifndef SOCKETDATA_H
#define SOCKETDATA_H
#include <QThread>
#include <socketchannel.h>
#include <QDebug>
class Socketdata : public QThread
{
    Q_OBJECT
public:
    Socketdata(Socketchannel *current_socket,int num);
    virtual ~Socketdata();
    void run();
    char *data;

public slots:
    void deleteself();
private:
    Socketchannel *current_socket;
    int num;
signals:
    void trigger(char *data);
};

#endif // SOCKETDATA_H
