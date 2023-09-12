#pragma once

#include "recorddata.h"
#include "typedef.h"

class ShowRecordData : public RecordData{

private:
    ShowData record;
public:

    QVariant data(int column, int role = Qt::DisplayRole) {
        switch(column){
        case 0:
            return QString(record.name);
        case 1:
            return QString(record.sex);
        case 2:
            return QVariant(record.age);
        case 3:
            return QString(record.phone);
        default:
            return QVariant();
        }
    }

    bool setData(int column, const QVariant& value, int role = Qt::EditRole) {
        switch(column){
        case 0:
             record.name=value.toString();
             return true;
        case 1:
            record.sex=value.toString();
            return true;
        case 2:
            record.age=value.toInt();
            return true;
        case 3:
            record.phone=value.toString();
            return true;
        }

    }

};

