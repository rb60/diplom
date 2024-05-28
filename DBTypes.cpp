#include "DBTypes.h"


QList<QList<ViewCell *> > removeColumnFromParametrs(QList<QList<ViewCell *> > parametrs, ViewColumn *column)
{
    QList<QList<ViewCell*>> result = QList<QList<ViewCell*>>(parametrs.size());
    for(int i = 0; i < parametrs.size(); i++)
    {
        for(int j = 0; j < parametrs[i].size(); j++)
        {
            if(parametrs[i][j]->column != column)
            {
                result[i].append(parametrs[i][j]);
            }
        }
    }
    return result;
}

QList<QList<ViewCell *> > removeRowFromParametrs(QList<QList<ViewCell *> > parametrs, ViewRow *row)
{
    QList<QList<ViewCell*>> result = QList<QList<ViewCell*>>(parametrs.size());
    for(int i = 0; i < parametrs.size(); i++)
    {
        for(int j = 0; j < parametrs[i].size(); j++)
        {
            if(parametrs[i][j]->row != row)
            {
                result[i].append(parametrs[i][j]);
            }
        }

    }
    return result;
}

QList<QList<ViewCell *> > removeLayerFromParametrs(QList<QList<ViewCell *> > parametrs, ViewLayer *layer)
{
    QList<QList<ViewCell*>> result = QList<QList<ViewCell*>>(parametrs.size());
    for(int i = 0; i < parametrs.size(); i++)
    {
        for(int j = 0; j < parametrs[i].size(); j++)
        {
            if(parametrs[i][j]->layer != layer)
            {
                result[i].append(parametrs[i][j]);
            }
        }
    }
    return result;
}


void ViewTable::addColumn(ViewColumn *column)
{
    columns.append(column);
    bool sorted;
    do
    {
        sorted = true;
        for(int i = 0; i < columns.size() - 1; i++)
        {
            if(columns[i]->order > columns[i + 1]->order)
            {
                sorted = false;
                columns.swapItemsAt(i, i + 1);
            }
        }
    }while(!sorted);
}

void ViewTable::addRow(ViewRow *row)
{
    rows.append(row);
    bool sorted;
    do
    {
        sorted = true;
        for(int i = 0; i < rows.size() - 1; i++)
        {
            if(rows[i]->order > rows[i + 1]->order)
            {
                sorted = false;
                rows.swapItemsAt(i, i + 1);
            }
        }
    }while(!sorted);
}

void ViewTable::addLayer(ViewLayer *layer)
{
    layers.append(layer);
    bool sorted;
    do
    {
        sorted = true;
        for(int i = 0; i < layers.size() - 1; i++)
        {
            if(layers[i]->order > layers[i + 1]->order)
            {
                sorted = false;
                layers.swapItemsAt(i, i + 1);
            }
        }
    }while(!sorted);
}

void ViewTable::addGenerator(CellGenerator *generator)
{
    generators.append(generator);
    bool sorted;
    do
    {
        sorted = true;
        for(int i = 0; i < generators.size() - 1; i++)
        {
            if(generators[i]->order > generators[i + 1]->order)
            {
                sorted = false;
                generators.swapItemsAt(i, i + 1);
            }
        }
    }while(!sorted);
}

ViewColumn *ViewTable::findColumnById(int id)
{
    for(int i = 0; i < columns.size(); i++)
    {
        if(columns[i]->id == id)
        {
            return columns[i];
        }
    }
    return nullptr;
}

ViewRow *ViewTable::findRowById(int id)
{
    for(int i = 0; i < rows.size(); i++)
    {
        if(rows[i]->id == id)
        {
            return rows[i];
        }
    }
    return nullptr;
}

ViewLayer *ViewTable::findLayerById(int id)
{
    for(int i = 0; i < layers.size(); i++)
    {
        if(layers[i]->id == id)
        {
            return layers[i];
        }
    }
    return nullptr;
}

ViewColumn *ViewTable::findColumnByOrder(int order)
{
    for(int i = 0; i < columns.size(); i++)
    {
        if(columns[i]->order == order)
        {
            return columns[i];
        }
    }
    return nullptr;
}

ViewRow *ViewTable::findRowByOrder(int order)
{
    for(int i = 0; i < rows.size(); i++)
    {
        if(rows[i]->order == order)
        {
            return rows[i];
        }
    }
    return nullptr;
}

ViewLayer *ViewTable::findLayerByOrder(int order)
{
    for(int i = 0; i < layers.size(); i++)
    {
        if(layers[i]->order == order)
        {
            return layers[i];
        }
    }
    return nullptr;
}

QList<ViewCell*> ViewTable::getPotentialyRemoved(ViewColumn *column)
{
    QList<ViewCell*> result;
    for(int i = 0; i < generators.size(); i++)
    {
        QList<QList<ViewCell*>> newParametrs = removeColumnFromParametrs(generators[i]->parametrs, column);
        if(newParametrs.contains(QList<ViewCell*>()))
        {
            result.append(generators[i]->destination);
            if(generators[i]->destination->row != nullptr && generators[i]->destination->column == nullptr)
            {
                result.append(getPotentialyRemoved(generators[i]->destination->row));
            }

            if(generators[i]->destination->column != nullptr && generators[i]->destination->row == nullptr)
            {
               result.append(getPotentialyRemoved(generators[i]->destination->column));
            }

            if(generators[i]->destination->column == nullptr && generators[i]->destination->row == nullptr)
            {
                result.append(getPotentialyRemoved(generators[i]->destination->layer));
            }
        }
    }
    return result;
}

QList<ViewCell *> ViewTable::getPotentialyRemoved(ViewRow *row)
{
    QList<ViewCell*> result;
    for(int i = 0; i < generators.size(); i++)
    {
        QList<QList<ViewCell*>> newParametrs = removeRowFromParametrs(generators[i]->parametrs, row);
        if(newParametrs.contains(QList<ViewCell*>()))
        {

            result.append(generators[i]->destination);
            if(generators[i]->destination->row != nullptr && generators[i]->destination->column == nullptr)
            {
                result.append(getPotentialyRemoved(generators[i]->destination->row));
            }

            if(generators[i]->destination->column != nullptr && generators[i]->destination->row == nullptr)
            {
                result.append(getPotentialyRemoved(generators[i]->destination->column));
            }

            if(generators[i]->destination->column == nullptr && generators[i]->destination->row == nullptr)
            {
                result.append(getPotentialyRemoved(generators[i]->destination->layer));
            }


        }
    }
    return result;
}

QList<ViewCell *> ViewTable::getPotentialyRemoved(ViewLayer *layer)
{
    QList<ViewCell*> result;
    for(int i = 0; i < generators.size(); i++)
    {
        QList<QList<ViewCell*>> newParametrs = removeLayerFromParametrs(generators[i]->parametrs, layer);
        if(newParametrs.contains(QList<ViewCell*>()))
        {

            result.append(generators[i]->destination);

            if(generators[i]->destination->row != nullptr && generators[i]->destination->column == nullptr)
            {
                result.append(getPotentialyRemoved(generators[i]->destination->row));
            }

            if(generators[i]->destination->column != nullptr && generators[i]->destination->row == nullptr)
            {
                result.append(getPotentialyRemoved(generators[i]->destination->column));
            }

            if(generators[i]->destination->column == nullptr && generators[i]->destination->row == nullptr)
            {
                result.append(getPotentialyRemoved(generators[i]->destination->layer));
            }


        }
    }
    return result;
}

ViewTable::~ViewTable()
{
    for(int i = 0; i < columns.size(); i++)
    {
        delete columns[i];
    }

    for(int i = 0; i < rows.size(); i++)
    {
        delete rows[i];
    }

    for(int i = 0; i < layers.size(); i++)
    {
        delete layers[i];
    }
}

ViewColumn::~ViewColumn()
{
    if(task != nullptr)
    {
        delete task;
    }
}

ViewRow::~ViewRow()
{
    if(student != nullptr)
    {
        delete student;
    }
}

Subject::~Subject()
{

    if(table != nullptr)
    {
        delete table;
    }
}

ViewCell *ViewLayer::findCell(ViewColumn *column, ViewRow *row)
{
    for(int i = 0; i < cells.size(); i++)
    {
        if(cells[i]->column == column && cells[i]->row == row)
        {
            return cells[i];
        }
    }
    return nullptr;
}

ViewCell *ViewLayer::findCell(int id)
{

    for(int i = 0; i < cells.size(); i++)
    {
        if(cells[i]->id == id)
        {
            return cells[i];
        }
    }
    return nullptr;
}

ViewLayer::~ViewLayer()
{
    for(int i = 0; i < cells.size(); i++)
    {
        delete cells[i];
    }
}

bool CellGenerator::isItDepence(ViewCell* cell, QList<CellGenerator*> generators)
{
    for(int i = 0; i < parametrs.size(); i++)
    {
        for(int j = 0; j < parametrs[i].size(); j++)
        {
            if(cell->canIntersect(parametrs[i][j]))
            {
                return true;
            }

            int recursiveResult = false;
            for(int k = 0; k < generators.size(); k++)
            {
                if(generators[k] != this && cell->canIntersect(generators[k]->destination))
                {
                    recursiveResult = generators[k]->isItDepence(cell, generators);
                    break;
                }
            }
            if(recursiveResult == true)
            {
                return true;
            }

        }
    }
    return false;
}

bool ViewCell::canIntersect(ViewCell *other)
{
    if(table != other->table)
    {
        return false;
    }
    if(layer != other->layer)
    {
        return false;
    }

    if((other->column == nullptr && other->row == nullptr) || (column == nullptr && row == nullptr))
    {
        return true;
    }

    if((other->column == nullptr && row == nullptr) || (other->row == nullptr && column == nullptr))
    {
        return true;
    }

    if(other->column == column && other->row == row)
    {
        return true;
    }

    return false;





}

Task::Task(QString name, int id)
    : name(name),
    id(id)
{}

Student::Student(QString name, int id)
    : name(name),
    id(id)
{}
