#include "averagestrategy.h"

AverageStrategy::AverageStrategy() {}

float AverageStrategy::calculate(QList<QList<float> > parameters)
{
    float result = 0;
    if(parameters.size() < 1)
    {
        return 0;
    }
    for(int i = 0; i < parameters[0].size(); i++)
    {
        result += parameters[0][i];
    }
    result /= parameters[0].size();
    return result;
}

QList<CalculationParametrInfo> AverageStrategy::parametrs()
{
    QList<CalculationParametrInfo> result;
    result.append(CalculationParametrInfo("values", -1));
    return result;
}

QString AverageStrategy::getName()
{
    return "Average";
}
