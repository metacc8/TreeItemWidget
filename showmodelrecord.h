#ifndef SHOWMODELRECORD_H
#define SHOWMODELRECORD_H

#include <vector>
#include "modelrecord.h"
#include "showrecorddata.h"


class ShowModelRecord : public ModelRecord{

    public:

        std::vector<ShowRecordData> vecRecord;


        int recordSize(){
            return vecRecord.size();
        }
        RecordData* recordAt(int pos){
            return &vecRecord[pos];
        }
};


#endif // SHOWMODELRECORD_H
