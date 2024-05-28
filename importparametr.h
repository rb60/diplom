#ifndef IMPORTPARAMETR_H
#define IMPORTPARAMETR_H
#include <QString>
#include "TransferData.h"

class ImportParametr
{
public:
    ImportParametr(QString name, ParametrType type);
    QString name;
    ParametrType type;
};

#endif // IMPORTPARAMETR_H
