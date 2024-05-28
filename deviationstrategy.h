#ifndef DEVIATIONSTRATEGY_H
#define DEVIATIONSTRATEGY_H

#include "calculationstrategy.h"

class DeviationStrategy : public CalculationStrategy
{
public:
    DeviationStrategy();

public:
    float calculate(QList<QList<float> > parameters) override;
    QList<CalculationParametrInfo> parametrs() override;
    QString getName() override;
};

#endif // DEVIATIONSTRATEGY_H
