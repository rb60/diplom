#ifndef DBTYPES_H
#define DBTYPES_H
#include <QString>
#include <QList>
#include "calculationstrategy.h"

class Student
{
public:
    Student(QString name = "",int id = -1);
    int id = -1;
    QString name;
};



class Task
{
public:
    Task(QString name = "",int id = -1);
    int id = -1;
    QString name;
};

class ViewTable;

class ViewColumn
{
public:
    ViewTable* table;
    int id;
    QString name;
    int order;
    Task* task = nullptr;
    ~ViewColumn();
};

class ViewRow
{
public:
    ViewTable* table;
    int id;
    QString name;
    int order;
    Student* student = nullptr;
    ~ViewRow();
};

class ViewLayer;

class ViewCell
{
public:
    int id;
    ViewTable* table = nullptr;
    ViewColumn* column = nullptr;
    ViewRow* row = nullptr;
    ViewLayer* layer = nullptr;
    int valueId  = -1;
    bool canIntersect(ViewCell* other);
};

class ViewLayer
{
public:
    ViewTable* table;
    QList<ViewCell*> cells;
    int id;
    QString name;
    int order;
    ViewCell *findCell(ViewColumn* column, ViewRow* row);
    ViewCell *findCell(int id);
    ~ViewLayer();

};

class CellGenerator
{
public:
    int id;
    QList<QList<ViewCell*>> parametrs;
    CalculationStrategy* strategy;
    ViewCell* destination;
    int order = -1;
    bool isItDepence(ViewCell *cell, QList<CellGenerator*> generators = QList<CellGenerator*>());
};

QList<QList<ViewCell*>> removeColumnFromParametrs(QList<QList<ViewCell*>> parametrs, ViewColumn *column);
QList<QList<ViewCell*>> removeRowFromParametrs(QList<QList<ViewCell*>> parametrs, ViewRow *row);
QList<QList<ViewCell*>> removeLayerFromParametrs(QList<QList<ViewCell*>> parametrs, ViewLayer *layer);


class ViewTable
{
public:
    int id;
    QList<ViewColumn*> columns;
    QList<ViewRow*> rows;
    QList<ViewLayer*> layers;
    QList<CellGenerator*> generators;
    void addColumn(ViewColumn* column);
    void addRow(ViewRow* row);
    void addLayer(ViewLayer* layer);
    void addGenerator(CellGenerator* generator);
    ViewColumn* findColumnById(int id);
    ViewRow* findRowById(int id);
    ViewLayer* findLayerById(int id);
    ViewColumn *findColumnByOrder(int order);
    ViewRow* findRowByOrder(int order);
    ViewLayer* findLayerByOrder(int order);
    QList<ViewCell *> getPotentialyRemoved(ViewColumn* column);
    QList<ViewCell *> getPotentialyRemoved(ViewRow* row);
    QList<ViewCell *> getPotentialyRemoved(ViewLayer* layer);
    ~ViewTable();
};

class Subject
{
public:
    int id;
    QString name;
    ViewTable* table = nullptr;
    ~Subject();
};






#endif // DBTYPES_H
