#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QStackedWidget>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void On_PushButton1();
    void On_PushButton2();
    void On_PushButton3();

private:
    Ui::MainWindow *ui;
    QStackedWidget *stackedWidget;
};




#endif // MAINWINDOW_H
