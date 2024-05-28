#ifndef IMPORTPARAMETRSELECTOR_H
#define IMPORTPARAMETRSELECTOR_H
#include "importparametr.h"

class ImportParametrSelector
{
public:
    virtual void setParametr(ImportParametr parametr) = 0;
    virtual QString getResult() = 0;
};

#endif // IMPORTPARAMETRSELECTOR_H
