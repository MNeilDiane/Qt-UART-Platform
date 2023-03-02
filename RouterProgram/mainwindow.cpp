#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    stackedWidget = new QStackedWidget;
    stackedWidget->setCurrentWidget(ui->page_1);

    connect(ui->pushButton_1,SIGNAL(clicked(bool)),this,SLOT(On_PushButton1()));
    connect(ui->pushButton_2,SIGNAL(clicked(bool)),this,SLOT(On_PushButton2()));
    connect(ui->pushButton_3,SIGNAL(clicked(bool)),this,SLOT(On_PushButton3()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::On_PushButton1(){
    ui->stackedWidget->setCurrentWidget(ui->page_1);
}

void MainWindow::On_PushButton2(){
    ui->stackedWidget->setCurrentWidget(ui->page_2);
}

void MainWindow::On_PushButton3(){
    ui->stackedWidget->setCurrentWidget(ui->page_3);
}
