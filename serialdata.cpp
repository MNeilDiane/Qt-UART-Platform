#include "serialdata.h"

Serialdata::Serialdata()
{

}
//formatdata处理报文的数据部分
QStringList Serialdata::FormatData(QString data)
{
    //以“|”做分割
    QStringList strArray = data.split("|");
    for(int index=0;index<strArray.length();index++)
    {
        //如果以“+”开头
        if(strArray[index].mid(0,1)=="+")
        {
            strArray[index] = strArray[index].mid(1);

        }
        //如果中间包括“*”
        if(strArray[index].contains("*"))
        {
            int end = strArray[index].indexOf("*");
            strArray[index] = strArray[index].mid(0,end);
        }
    }
    return strArray;

}
