#include "serialdata.h"

Serialdata::Serialdata(QWidget *parent) : QWidget(parent)
{

}
Serialdata::~Serialdata()
{

}
QStringList Serialdata::FormatData(QString data)
{
    QStringList strArray = data.split("|");
    for(int index=0;index<strArray.length();index++)
    {
        if(strArray[index].mid(0,1)=="+")
        {
            strArray[index] = strArray[index].mid(1);

        }
        if(strArray[index].contains("*"))
        {
            int end = strArray[index].indexOf("*");
            strArray[index] = strArray[index].mid(0,end);
        }
    }
    return strArray;

}
