#include "socketset.h"
#include "ui_socketset.h"

Socketset::Socketset(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Socketset)
{
    ui->setupUi(this);
    ui->verticalLayout->addStretch();
    connect(ui->pushButton_add,&QPushButton::clicked,this,&Socketset::pushButton_add);
}

Socketset::~Socketset()
{
    delete ui;
}

void Socketset::pushButton_add(){
    QHBoxLayout *info =new QHBoxLayout();

    QLabel *iplabel = new QLabel(tr("ip:"));
    QLineEdit *ipedit = new QLineEdit();

    QLabel *portlabel = new QLabel(tr("port:"));
    QLineEdit *portedit = new QLineEdit();

    QLabel *protocollabel = new QLabel(tr("protocol:"));
    QComboBox *protocolselect = new QComboBox();

    QPushButton *route = new QPushButton();
    route->setText("route");

    QPushButton *deleteself = new QPushButton();
    deleteself->setText("delete");

    protocolselect->addItem(tr("TCP"));
    protocolselect->addItem(tr("UDP"));
    protocolselect->addItem(tr("MQTT"));

    info->addWidget(iplabel);
    info->addWidget(ipedit);
    info->addWidget(portlabel);
    info->addWidget(portedit);
    info->addWidget(protocollabel);
    info->addWidget(protocolselect);
    info->addWidget(route);
    info->addWidget(deleteself);

    ui->verticalLayout->addLayout(info);
    connect(deleteself,&QPushButton::clicked,this,&Socketset::pushButton_deleteself);
}

void Socketset::pushButton_deleteself(){
    QPushButton *current_btn = qobject_cast<QPushButton *>(sender());
    ui->verticalLayout->removeItem(ui->verticalLayout->itemAt(0));
    delete current_btn;
}
