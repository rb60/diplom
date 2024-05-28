#ifndef IMPORTTYPES_H
#define IMPORTTYPES_H
#include "TransferData.h"

class ImportData
{
public:
    virtual char* getName() = 0;
    virtual int getParametrsCount() = 0;
    virtual char* getParametrName(int index) = 0;
    virtual int getParametrType(int index) = 0;
    virtual TransferData* getData(char** parametrs) = 0;
    virtual void destroy() = 0;
};

#endif // IMPORTTYPES_H
