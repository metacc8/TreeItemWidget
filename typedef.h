#ifndef TYPEDEF_H
#define TYPEDEF_H

#include <QVector>

// person��Ϣ
typedef struct _ShowData{
    QString name;   // ����
    QString sex;    // �Ա�
    int age;     // ����
    QString phone;  // �绰����
    _ShowData()
    {
        age = 0;
    }
} ShowData;


// ���к�
enum COLUMN
{
    COLUMN_NAME = 0,
    COLUMN_SEX,
    COLUMN_AGE,
    COLUMN_PHONE
};

#endif // TYPEDEF_H
