#ifndef AVERAGESTRATEGY_H
#define AVERAGESTRATEGY_H

#include "calculationstrategy.h"

class AverageStrategy : public CalculationStrategy
{
public:
    AverageStrategy();

public:
    float calculate(QList<QList<float> > parameters) override;
    QList<CalculationParametrInfo> parametrs() override;
    QString getName() override;
    ~AverageStrategy(){}
};

#endif // AVERAGESTRATEGY_H
