#include "deviationstrategy.h"

DeviationStrategy::DeviationStrategy() {}

float DeviationStrategy::calculate(QList<QList<float> > parameters)
{
    if(parameters.size() != 2)
    {
        return 0;
    }

    if(parameters[0].size() != 1 || parameters[1].size() != 1)
    {
        return 0;
    }

    return (parameters[0][0] - parameters[1][0])*(parameters[0][0] - parameters[1][0]);

}

QList<CalculationParametrInfo> DeviationStrategy::parametrs()
{
    QList<CalculationParametrInfo> result;
    result.append(CalculationParametrInfo("value", 1));
    result.append(CalculationParametrInfo("target", 1));
    return result;
}

QString DeviationStrategy::getName()
{
    return "Diviation";
}
