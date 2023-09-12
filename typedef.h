#ifndef TYPEDEF_H
#define TYPEDEF_H

#include <QVector>

// person信息
typedef struct _ShowData{
    QString name;   // 姓名
    QString sex;    // 性别
    int age;     // 年龄
    QString phone;  // 电话号码
    _ShowData()
    {
        age = 0;
    }
} ShowData;


// 树列号
enum COLUMN
{
    COLUMN_NAME = 0,
    COLUMN_SEX,
    COLUMN_AGE,
    COLUMN_PHONE
};

#endif // TYPEDEF_H
