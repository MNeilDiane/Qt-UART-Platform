/**
 * @file mainwindow.h
 * @author wujunzhe/302538094@qq.com
 * @version 1.0.1
 * @date 2023.3.19
 * @brief 概述：MainWindow头文件
 * @details 无详细说明
 * @note 描述需要注意的问题
 */


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

/**
 *  @brief 类声明，主窗口
 *  @see 对应的源文件参见 mainwindow.cpp
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTabWidget *tabWidget;
};

#endif // MAINWINDOW_H
