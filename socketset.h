/**
 * @file socketset.h
 * @author wujunzhe/302538094@qq.com
 * @version 1.0.1
 * @date 2023.3.19
 * @brief
 * @details 无详细说明
 * @note 描述需要注意的问题
 */
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

/**
 *  @brief Socketset类声明
 *  @details Socketdata继承于QWidget，是作为socket设置界面的后端代码
 *  @see 对应的源文件参见 socketdata.cpp
 */
class Socketset : public QWidget {
    Q_OBJECT

public:
    explicit Socketset(QWidget *parent = nullptr);

    ~Socketset();

    //void pushButton_add();
    //void pushButton_deleteself(QLayout *current_info);
    //void pushButton_route(QString info);
    //vector<Socketchannel *> current_socket;
    void show_ini();
    void set_edit(QString ip,QString port,QString protocol,int num);

public
    slots:
            void on_save();


private:
    Ui::Socketset *ui;
    signals:
            void reinitial_socket_fromserial();///< 信号函数，在on_save()中被调用，其对应的槽函数为@ref initialize_socket() serialport.h
};

#endif // SOCKETSET_H
