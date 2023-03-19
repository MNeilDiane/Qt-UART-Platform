#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "serialport.h"
#include "socketset.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //设置标题
    setWindowTitle("视频遥测终端");
    //取消最大化按钮
    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint);
    setFixedSize(this->width(),this->height());

    //connect(ui->centralWidget,SIGNAL(senddata_tosocket(QByteArray)),ui->centralWidget,SLOT(getdata_fromserial(QByteArray)));


}

MainWindow::~MainWindow()
{
    delete ui;
}
