#ifndef CELLGENERATOR_H
#define CELLGENERATOR_H
#include "DBTypes.h"
#include "calculationstrategy.h"

class CellGenerator
{
public:
    QList<QList<ViewCell*>> parametrs;
    CalculationStrategy* strategy;
    ViewCell* destination;
    int order;
    CellGenerator();
};

#endif // CELLGENERATOR_H
