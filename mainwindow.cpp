/**
 * @file mainwindow.cpp
 * @author wujunzhe/302538094@qq.com
 * @version 1.0.1
 * @date 2023.3.19
 * @brief 概述：MainWindow头文件
 * @details 无详细说明
 * @note 描述需要注意的问题
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "serialport.h"
#include "socketset.h"


/**
* @brief 函数定义
* @details xx
* @param QWidget*
* @return none
* @warning none
* @note 注解
*/
MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setWindowTitle("视频遥测终端");
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
    setFixedSize(this->width(), this->height());
}

MainWindow::~MainWindow() {
    delete ui;
}
