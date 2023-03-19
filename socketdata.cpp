#include "socketdata.h"

Socketdata::Socketdata(Socketchannel *current_socket,int num){
    this->current_socket=current_socket;
    this->num=num;

    connect(this,&Socketdata::finished,this,[=](){
        delete this;
        qDebug()<<"port"<<num<<" older thread has been destoryed["<<currentThreadId()<<"],please check the link,retry in 10 minutes later";
    });
}



void Socketdata::run(){
    while(1){
        if(current_socket->connectstatus!=0){
            break;
        }else{
            this->data=this->current_socket->RecieveMessage();
            if(this->data!=nullptr){
                cout<<data<<endl;
                this->trigger(this->data);
            }else{
                current_socket->connectstatus=-2;
            }
        }
    }
}

Socketdata::~Socketdata(){

}

void Socketdata::deleteself(){

    this->terminate();
    this->wait();
    this->deleteLater();
}
