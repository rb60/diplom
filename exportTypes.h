#ifndef EXPORTTYPES_H
#define EXPORTTYPES_H
#include "TransferData.h"

class ExportData
{
public:
    virtual char* getName() = 0;
    virtual int getParametrsCount() = 0;
    virtual char* getParametrName(int index) = 0;
    virtual int getParametrType(int index) = 0;
    virtual void exportData(char** parametrs, TransferData* data) = 0;
    virtual void destroy() = 0;
};

#endif // EXPORTTYPES_H
