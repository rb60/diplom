#ifndef CALCULATIONSTRATEGY_H
#define CALCULATIONSTRATEGY_H
#include <QList>
#include "calculationparametrinfo.h"

class CalculationStrategy
{
public:
    int id = -1;
    virtual float calculate(QList<QList<float>> parameters) = 0;
    virtual QList<CalculationParametrInfo> parametrs() = 0;
    virtual QString getName() = 0;
    virtual ~CalculationStrategy() {}
};

#endif // CALCULATIONSTRATEGY_H
