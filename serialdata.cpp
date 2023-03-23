/**
 * @file serialdata.cpp
 * @author wujunzhe/302538094@qq.com
 * @version 1.0.1
 * @date 2023.3.19
 * @brief 概述：
 * @details 无详细说明
 * @note 描述需要注意的问题
 */

#include "serialdata.h"

/**
* @brief Serialdata构造函数
* @details 基础构造函数
* @param QWidget* 无特定传输参数
* @return none
* @warning none
* @note none
*/
Serialdata::Serialdata() {

}

/**
* @brief Serialdata成员函数FormatData
* @details 该函数用于处理报文里的数据部分，通过分割“+”及“*”获取数据
* @param QString data
* @return QStringList strArray
* @warning none
* @note none
*/
QStringList Serialdata::FormatData(QString data) {
    QStringList strArray = data.split("|");
    for (int index = 0; index < strArray.length(); index++) {
        if (strArray[index].mid(0, 1) == "+") {
            strArray[index] = strArray[index].mid(1);

        }
        if (strArray[index].contains("*")) {
            int end = strArray[index].indexOf("*");
            strArray[index] = strArray[index].mid(0, end);
        }
    }
    return strArray;

}
