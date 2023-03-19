#ifndef SOCKETSET_H
#define SOCKETSET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QDebug>
#include <QString>
#include <socketchannel.h>
#include <QtCore/QCoreApplication>
#include <QSettings>
#include <iostream>
#include <QFileInfo>
#include <string>



using namespace std;


namespace Ui {
class Socketset;
}

class Socketset : public QWidget
{
    Q_OBJECT

public:
    explicit Socketset(QWidget *parent = nullptr);
    ~Socketset();
    //void pushButton_add();
    //void pushButton_deleteself(QLayout *current_info);
    //void pushButton_route(QString info);
    //vector<Socketchannel *> current_socket;
    void show_ini();

public slots:
    void on_save();



private:
    Ui::Socketset *ui;

signals:
    void reinitial_socket_fromserial();



};






#endif // SOCKETSET_H
