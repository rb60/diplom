#include "dbmanger.h"
#include <QtSql>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QtLogging>
#include <QtDebug>
#include <QStringList>

DBmanger::DBmanger()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("database.db");
}

DBmanger* DBmanger::instance = nullptr;

DBmanger *DBmanger::getInstance()
{
    if(instance == nullptr)
    {
        instance = new DBmanger();
    }
    return instance;
}

bool DBmanger::open()
{
    if(!db.isOpen())
    {
        db.open();
    }
    return db.isOpen();

}

void DBmanger::init()
{
    if(!open())
    {
        return;
    }

    QFile queryFile(":/sql/initdb.sql");
    if(!queryFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&queryFile);
    QStringList queryStrings = in.readAll().split(";");
    QSqlQuery query;
    for(int i = 0; i < queryStrings.size(); i++)
    {
        if(!query.exec(queryStrings[i]))
        {
            qDebug() << query.lastQuery();
            qDebug() << query.lastError().text();
        }
    }
}


void DBmanger::addCalculatedColumn(CellGenerator *generator, QString name, ViewLayer *layer, Subject *subject)
{
    if(!open())
    {
        return;
    }
    QSqlQuery query;
    if(!beginTransaction(query))
        return;

    if(layer == nullptr)
    {
        layer = new ViewLayer;
        layer->name = name;
        layer->order = subject->table->layers.size();
        layer->table = subject->table;
        if(!addLayer(layer))
        {
            rollbackTransaction(query);
            delete layer;
            return;
        }
    }


    ViewColumn* column = new ViewColumn;
    column->name = name;
    column->table = subject->table;
    column->order = subject->table->columns.size();
    if(!addColumn(column))
    {
        rollbackTransaction(query);
        delete column;
        return;
    }

    generator->destination = layer->findCell(column, nullptr);
    if(generator->destination == nullptr)
    {
        return;
    }

    if(!addGenerator(generator, subject))
    {
        rollbackTransaction(query);
        return;
    }
    applyGenerators(subject);
    commitTransaction(query);
}

void DBmanger::addCalculatedRow(CellGenerator *generator, QString name, ViewLayer *layer, Subject *subject)
{
    if(!open())
    {
        return;
    }
    QSqlQuery query;
    if(!beginTransaction(query))
        return;

    if(layer == nullptr)
    {
        layer = new ViewLayer;
        layer->name = name;
        layer->order = subject->table->layers.size();
        layer->table = subject->table;
        if(!addLayer(layer))
        {
            rollbackTransaction(query);
            delete layer;
            return;
        }
    }


    ViewRow* row = new ViewRow;
    row->name = name;
    row->table = subject->table;
    row->order = subject->table->rows.size();
    if(!addRow(row))
    {
        rollbackTransaction(query);
        delete row;
        return;
    }

    generator->destination = layer->findCell(nullptr, row);
    if(generator->destination == nullptr)
    {
        return;
    }

    addGenerator(generator, subject);
    applyGenerators(subject);
    commitTransaction(query);
}

void DBmanger::addCalculatedLayer(CellGenerator *generator, QString name, Subject *subject)
{
    if(!open())
    {
        return;
    }
    QSqlQuery query;
    if(!beginTransaction(query))
        return;


    ViewLayer*layer = new ViewLayer;
    layer->name = name;
    layer->order = subject->table->layers.size();
    layer->table = subject->table;
    if(!addLayer(layer))
    {
        rollbackTransaction(query);
        delete layer;
        return;
    }


    generator->destination = layer->findCell(nullptr, nullptr);
    if(generator->destination == nullptr)
    {
        return;
    }

    addGenerator(generator, subject);
    applyGenerators(subject);
    commitTransaction(query);
}

void DBmanger::updateGeneratorParametrs(CellGenerator *generator, QList<QList<ViewCell *> > newParametrs, Subject *subject, bool transaction)
{

    if(generator->parametrs.size() != newParametrs.size())
    {
        return;
    }

    QList<GeneratorDifference> difference(newParametrs.size());
    for(int i = 0; i < newParametrs.size(); i++)
    {
        for(int j = 0; j < newParametrs[i].size(); j++)
        {
            if(!generator->parametrs[i].contains(newParametrs[i][j]))
            {
                difference[i].added.append(newParametrs[i][j]);
            }
        }

        for(int j = 0; j < generator->parametrs[i].size(); j++)
        {
            if(!newParametrs[i].contains(generator->parametrs[i][j]))
            {
                difference[i].removed.append(generator->parametrs[i][j]);
            }
        }
    }

    if(!open())
    {
        return;
    }
    QSqlQuery query;
    if(transaction && !beginTransaction(query))
        return;


    QList<QList<ViewCell *> > oldParametrs = generator->parametrs;
    int oldOrder = generator->order;
    generator->parametrs = newParametrs;
    if(!testGenerator(generator, subject))
    {
        generator->parametrs = oldParametrs;
        rollbackTransaction(query);
        return;
    }

    setGeneratorOrder(generator, subject);


    for(int i = 0; i < difference.size(); i++)
    {
        query.prepare("SELECT _group FROM GroupToCellGenerator WHERE generator = :generator AND _order = :_order");
        query.bindValue(":generator", generator->id);
        query.bindValue(":_order", i);
        if(!exec(query))
        {
            generator->parametrs = oldParametrs;
            generator->order = oldOrder;
            return;
        };
        if(!query.first())
        {
            generator->parametrs = oldParametrs;
            generator->order = oldOrder;
            return;
        }
        int groupId = toPositiveInt(query.value("_group"));
        if(groupId == -1)
        {
            generator->parametrs = oldParametrs;
            generator->order = oldOrder;
            return;
        }

        for(int j = 0; j < difference[i].added.size(); j++)
        {
            query.prepare("INSERT INTO CellToCellGroup(_group, cell) VALUES(:_group, :cell)");
            query.bindValue(":_group", groupId);
            query.bindValue(":cell", difference[i].added[j]->id);
            if(!exec(query))
            {
                generator->parametrs = oldParametrs;
                generator->order = oldOrder;
                return;
            };
        }

        for(int j = 0; j < difference[i].removed.size(); j++)
        {
            query.prepare("DELETE FROM CellToCellGroup WHERE _group = :_group AND cell = :cell");
            query.bindValue(":_group", groupId);
            query.bindValue(":cell", difference[i].removed[j]->id);
            if(!exec(query))
            {
                generator->parametrs = oldParametrs;
                generator->order = oldOrder;
                return;
            };
        }
    }


    ViewCell iterator;
    ViewCell* destination;
    iterator.table = generator->destination->table;
    iterator.layer = generator->destination->layer;
    iterator.column = generator->destination->column;
    iterator.row = generator->destination->row;
    for(int i = 0; i < subject->table->columns.size(); i++)
    {
        if(generator->destination->column == nullptr)
        {
            iterator.column = subject->table->columns[i];
        }

        for(int j = 0; j < subject->table->rows.size(); j++)
        {
            if(generator->destination->row == nullptr)
            {
                iterator.row = subject->table->rows[j];
            }

            destination = iterator.layer->findCell(iterator.column, iterator.row);
            if(destination != nullptr)
            {
                int calculatorId = getCalculatorId(destination->valueId);
                if(calculatorId != -1)
                {
                    QList<CalculatorDifference> calcDifference = getCalculatorDifference(difference, destination);
                    if(!updateCalculatorParametrs(calcDifference, calculatorId))
                    {
                        generator->parametrs = oldParametrs;
                        generator->order = oldOrder;
                        rollbackTransaction(query);
                        return;
                    }
                    updateCalculator(calculatorId);

                }
            }

            if(generator->destination->row != nullptr)
            {
                break;
            }

        }

        if(generator->destination->column != nullptr)
        {
            break;
        }
    }

    if(transaction)
        commitTransaction(query);
}

bool DBmanger::removeColumn(ViewColumn *column, Subject *subject, bool transaction)
{
    if(!open() || column == nullptr || subject == nullptr)
    {
        return false;
    }
    QSqlQuery query;
    if(transaction && !beginTransaction(query))
        return false;

    query.prepare("SELECT _value, id FROM ViewCell WHERE viewColumn = :viewColumn");
    query.bindValue(":viewColumn", column->id);
    if(!exec(query, transaction))
    {
        return false;
    }
    while(query.next())
    {
        if(toPositiveInt(query.value("_value")) != -1)
        {
            if(!removeValue(toPositiveInt(query.value("_value"))))
            {
                if(transaction)
                    rollbackTransaction(query);
                return false;
            }
        }
        else
        {
            if(!removeCell(toPositiveInt(query.value("id"))))
            {
                if(transaction)
                    rollbackTransaction(query);
                return false;
            }
        }


    }

    query.prepare("DELETE FROM ViewColumn WHERE id = :id");
    query.bindValue(":id", column->id);
    if(!exec(query, transaction))
    {
        return false;
    }

    query.prepare("UPDATE ViewColumn SET _order = _order - 1 WHERE _order > :_order AND viewTable = :viewTable");
    query.bindValue(":_order", column->order);
    query.bindValue(":viewTable", subject->table->id);
    if(!exec(query, transaction))
    {
        return false;
    }



    for(int i = 0; i < subject->table->generators.size(); i++)
    {
        if(subject->table->generators[i]->destination->column == column)
        {
            if(!removeGenerator(subject->table->generators[i], subject))
            {
                if(transaction)
                    rollbackTransaction(query);
                return false;
            }
            i--;
            continue;
        }

        QList<QList<ViewCell*>> newParametrs = removeColumnFromParametrs(subject->table->generators[i]->parametrs, column);
        if(newParametrs.contains(QList<ViewCell*>()))
        {
            if(subject->table->generators[i]->destination->row != nullptr && subject->table->generators[i]->destination->column == nullptr)
            {
                if(!removeRow(subject->table->generators[i]->destination->row, subject, false))
                {
                    if(transaction)
                        rollbackTransaction(query);
                    return false;
                }
                i = -1;
                continue;
            }

            if(subject->table->generators[i]->destination->column != nullptr && subject->table->generators[i]->destination->row == nullptr)
            {
                if(!removeColumn(subject->table->generators[i]->destination->column, subject, false))
                {
                    if(transaction)
                        rollbackTransaction(query);
                    return false;
                }
                i = -1;
                continue;
            }

            if(subject->table->generators[i]->destination->column == nullptr && subject->table->generators[i]->destination->row == nullptr)
            {
                if(!removeLayer(subject->table->generators[i]->destination->layer, subject, false))
                {
                    if(transaction)
                        rollbackTransaction(query);
                    return false;
                }
                i = -1;
                continue;
            }
        }
        else
        {
            updateGeneratorParametrs(subject->table->generators[i], newParametrs, subject, false);
        }
    }

    for(int i = 0; i < subject->table->layers.size(); i++)
    {
        for(int j = 0; j < subject->table->layers[i]->cells.size(); j++)
        {
            if(subject->table->layers[i]->cells[j]->column == column)
            {
                delete subject->table->layers[i]->cells[j];
                subject->table->layers[i]->cells.remove(j);
                j--;
            }
        }
    }


    int columnIndex = subject->table->columns.indexOf(column);
    if(columnIndex >= 0)
    {
        delete subject->table->columns[columnIndex];
        subject->table->columns.removeAll(column);
    }

    if(transaction)
        commitTransaction(query);
    return true;
}

bool DBmanger::removeRow(ViewRow *row, Subject *subject, bool transaction)
{
    if(!open() || row == nullptr || subject == nullptr)
    {
        return false;
    }
    QSqlQuery query;
    if(transaction && !beginTransaction(query))
        return false;

    query.prepare("SELECT _value, id FROM ViewCell WHERE viewRow = :viewRow");
    query.bindValue(":viewRow", row->id);
    if(!exec(query, transaction))
    {
        return false;
    }
    while(query.next())
    {
        if(toPositiveInt(query.value("_value")) != -1)
        {
            if(!removeValue(toPositiveInt(query.value("_value"))))
            {
                if(transaction)
                    rollbackTransaction(query);
                return false;
            }
        }
        else
        {
            if(!removeCell(toPositiveInt(query.value("id"))))
            {
                if(transaction)
                    rollbackTransaction(query);
                return false;
            }
        }
    }

    query.prepare("DELETE FROM ViewRow WHERE id = :id");
    query.bindValue(":id", row->id);
    if(!exec(query, transaction))
    {
        return false;
    }

    query.prepare("UPDATE ViewRow SET _order = _order - 1 WHERE _order > :_order AND viewTable = :viewTable");
    query.bindValue(":_order", row->order);
    query.bindValue(":viewTable", subject->table->id);
    if(!exec(query, transaction))
    {
        return false;
    }





    for(int i = 0; i < subject->table->generators.size(); i++)
    {

        if(subject->table->generators[i]->destination->row == row)
        {
            if(!removeGenerator(subject->table->generators[i], subject))
            {
                if(transaction)
                    rollbackTransaction(query);
                return false;
            }
            i--;
            continue;
        }

        QList<QList<ViewCell*>> newParametrs = removeRowFromParametrs(subject->table->generators[i]->parametrs, row);
        if(newParametrs.contains(QList<ViewCell*>()))
        {
            if(subject->table->generators[i]->destination->row != nullptr && subject->table->generators[i]->destination->column == nullptr)
            {
                if(!removeRow(subject->table->generators[i]->destination->row, subject, false))
                {
                    if(transaction)
                        rollbackTransaction(query);
                    return false;
                }
                i = -1;
                continue;
            }

            if(subject->table->generators[i]->destination->column != nullptr && subject->table->generators[i]->destination->row == nullptr)
            {
                if(!removeColumn(subject->table->generators[i]->destination->column, subject, false))
                {
                    if(transaction)
                        rollbackTransaction(query);
                    return false;
                }
                i = -1;
                continue;
            }

            if(subject->table->generators[i]->destination->column == nullptr && subject->table->generators[i]->destination->row == nullptr)
            {
                if(!removeLayer(subject->table->generators[i]->destination->layer, subject, false))
                {
                    if(transaction)
                        rollbackTransaction(query);
                    return false;
                }
                i = -1;
                continue;
            }
        }
        else
        {
            updateGeneratorParametrs(subject->table->generators[i], newParametrs, subject, false);
        }
    }

    for(int i = 0; i < subject->table->layers.size(); i++)
    {
        for(int j = 0; j < subject->table->layers[i]->cells.size(); j++)
        {
            if(subject->table->layers[i]->cells[j]->row == row)
            {
                delete subject->table->layers[i]->cells[j];
                subject->table->layers[i]->cells.remove(j);
                j--;
            }
        }
    }

    int rowIndex = subject->table->rows.indexOf(row);
    if(rowIndex >= 0)
    {
        delete subject->table->rows[rowIndex];
        subject->table->rows.removeAll(row);
    }

    if(transaction)
        commitTransaction(query);
    return true;

}

bool DBmanger::removeLayer(ViewLayer *layer, Subject *subject, bool transaction)
{
    if(!open() || layer == nullptr || subject == nullptr)
    {
        return false;
    }
    QSqlQuery query;
    if(transaction && !beginTransaction(query))
        return false;

    query.prepare("SELECT _value, id FROM ViewCell WHERE viewLayer = :viewLayer");
    query.bindValue(":viewLayer", layer->id);
    if(!exec(query, transaction))
    {
        return false;
    }
    while(query.next())
    {
        if(toPositiveInt(query.value("_value")) != -1)
        {
            if(!removeValue(toPositiveInt(query.value("_value"))))
            {
                if(transaction)
                    rollbackTransaction(query);
                return false;
            }
        }
        else
        {
            if(!removeCell(toPositiveInt(query.value("id"))))
            {
                if(transaction)
                    rollbackTransaction(query);
                return false;
            }
        }
    }

    query.prepare("DELETE FROM ViewLayer WHERE id = :id");
    query.bindValue(":id", layer->id);
    if(!exec(query, transaction))
    {
        return false;
    }

    query.prepare("UPDATE ViewLayer SET _order = _order - 1 WHERE _order > :_order AND viewTable = :viewTable");
    query.bindValue(":_order", layer->order);
    query.bindValue(":viewTable", subject->table->id);
    if(!exec(query, transaction))
    {
        return false;
    }



    for(int i = 0; i < subject->table->generators.size(); i++)
    {
        if(subject->table->generators[i]->destination->layer == layer)
        {
            if(!removeGenerator(subject->table->generators[i], subject))
            {
                if(transaction)
                    rollbackTransaction(query);
                return false;
            }
            i--;
            continue;
        }


        QList<QList<ViewCell*>> newParametrs = removeLayerFromParametrs(subject->table->generators[i]->parametrs, layer);
        if(newParametrs.contains(QList<ViewCell*>()))
        {
            if(subject->table->generators[i]->destination->row != nullptr && subject->table->generators[i]->destination->column == nullptr)
            {
                if(!removeRow(subject->table->generators[i]->destination->row, subject, false))
                {
                    if(transaction)
                        rollbackTransaction(query);
                    return false;
                }
                i = -1;
                continue;
            }

            if(subject->table->generators[i]->destination->column != nullptr && subject->table->generators[i]->destination->row == nullptr)
            {
                if(!removeColumn(subject->table->generators[i]->destination->column, subject, false))
                {
                    if(transaction)
                        rollbackTransaction(query);
                    return false;
                }
                i = -1;
                continue;
            }

            if(subject->table->generators[i]->destination->column == nullptr && subject->table->generators[i]->destination->row == nullptr)
            {
                if(!removeLayer(subject->table->generators[i]->destination->layer, subject, false))
                {
                    if(transaction)
                        rollbackTransaction(query);
                    return false;
                }
                i = -1;
                continue;
            }
        }
        else
        {
            updateGeneratorParametrs(subject->table->generators[i], newParametrs, subject, false);
        }
    }

    int layerIndex = subject->table->layers.indexOf(layer);
    if(layerIndex >= 0)
    {
        delete subject->table->layers[layerIndex];
        subject->table->layers.removeAll(layer);
    }

    if(transaction)
        commitTransaction(query);
    return true;
}

void DBmanger::changeOrder(ViewColumn *column, int newOrder)
{
    if(!open() || column == nullptr || column->order == -1 || column->table == nullptr)
    {
        return;
    }
    QSqlQuery query;
    if(!beginTransaction(query))
        return;

    query.prepare("UPDATE ViewColumn SET _order = :newOrder WHERE _order = :oldOrder AND viewTable = :viewTable");
    query.bindValue(":newOrder", column->order);
    query.bindValue(":oldOrder", newOrder);
    query.bindValue(":viewTable", column->table->id);
    if(!exec(query))
    {
        return;
    }

    query.prepare("UPDATE ViewColumn SET _order = :newOrder WHERE id = :id");
    query.bindValue(":id", column->id);
    query.bindValue(":newOrder", newOrder);
    if(!exec(query))
    {
        return;
    }
    commitTransaction(query);


}

void DBmanger::changeOrder(ViewRow *row, int newOrder)
{
    if(!open() || row == nullptr || row->order == -1 || row->table == nullptr)
    {
        return;
    }
    QSqlQuery query;
    if(!beginTransaction(query))
        return;

    query.prepare("UPDATE ViewRow SET _order = :newOrder WHERE _order = :oldOrder AND viewTable = :viewTable");
    query.bindValue(":newOrder", row->order);
    query.bindValue(":oldOrder", newOrder);
    query.bindValue(":viewTable", row->table->id);
    if(!exec(query))
    {
        return;
    }

    query.prepare("UPDATE ViewRow SET _order = :newOrder WHERE id = :id");
    query.bindValue(":id", row->id);
    query.bindValue(":newOrder", newOrder);
    if(!exec(query))
    {
        return;
    }
    commitTransaction(query);
}

void DBmanger::changeOrder(ViewLayer *layer, int newOrder)
{
    if(!open() || layer == nullptr || layer->order == -1 || layer->table == nullptr)
    {
        return;
    }
    QSqlQuery query;
    if(!beginTransaction(query))
        return;

    query.prepare("UPDATE ViewLayer SET _order = :newOrder WHERE _order = :oldOrder AND viewTable = :viewTable");
    query.bindValue(":newOrder", layer->order);
    query.bindValue(":oldOrder", newOrder);
    query.bindValue(":viewTable", layer->table->id);
    if(!exec(query))
    {
        return;
    }

    query.prepare("UPDATE ViewLayer SET _order = :newOrder WHERE id = :id");
    query.bindValue(":id", layer->id);
    query.bindValue(":newOrder", newOrder);
    if(!exec(query))
    {
        return;
    }
    commitTransaction(query);
}

void DBmanger::addStrategy(CalculationStrategy *strategy)
{
    if(strategy == nullptr)
    {
        return;
    }
    if(!open())
    {
        return;
    }
    QSqlQuery query;
    if(!beginTransaction(query))
        return;

    query.prepare("SELECT * FROM Strategy WHERE name = :name");
    query.bindValue(":name", strategy->getName());
    if(!exec(query))
    {
        return;
    }
    if(query.first())
    {
        strategy->id = toPositiveInt(query.value("id"));
        if(strategy->id == -1)
        {
            rollbackTransaction(query);
            return;
        }
    }
    else
    {
        query.prepare("INSERT INTO Strategy(name) VALUES(:name)");
        query.bindValue(":name", strategy->getName());
        if(!exec(query))
        {
            return;
        }
        strategy->id = getLastId();
    }
    strategys.append(strategy);
    commitTransaction(query);
}

CalculationStrategy *DBmanger::getStrategy(int id)
{
    for(int i = 0; i < strategys.size(); i++)
    {
        if(strategys[i]->id == id)
        {
            return strategys[i];
        }
    }
    return nullptr;
}

QList<CalculationStrategy *> DBmanger::getStrategyList()
{
    return strategys;
}

float DBmanger::getMarkValueById(int id)
{
    if(id < 0)
    {
        return 0;
    }
    if(!open())
    {
        return 0;
    }
    QSqlQuery query;
    query.prepare("SELECT * FROM MarkValue WHERE id = :id");
    query.bindValue(":id", id);
    if(!exec(query, false))
    {
        return 0;
    }
    if(!query.first())
    {
        return 0;
    }
    if(!query.value("_value").canConvert(QMetaType::Float))
    {
        return 0;
    }
    return query.value("_value").toFloat();
}

void DBmanger::setMarkValueById(int id, float value)
{
    if(id < 0)
    {
        return;
    }
    if(!open())
    {
        return;
    }
    QSqlQuery query;
    query.prepare("UPDATE MarkValue SET _value = :_value WHERE id = :id");
    query.bindValue(":_value", value);
    query.bindValue(":id", id);
    if(!exec(query, false))
    {
        return;
    }

    query.prepare("SELECT CalculatedMark.id as calcId, CalculatedMark._order FROM MarkToMarkGroup "
                  "INNER JOIN GroupToMarkCalculator ON MarkToMarkGroup._group = GroupToMarkCalculator._group "
                  "INNER JOIN CalculatedMark ON GroupToMarkCalculator.calculator = CalculatedMark.id "
                  "WHERE MarkToMarkGroup.mark = :markId "
                  "ORDER BY CalculatedMark._order ASC");
    query.bindValue(":markId", id);


    if(!exec(query, false))
    {
        return;
    }
    while(query.next())
    {
        int calcId = toPositiveInt(query.value("calcId"));
        if(calcId == -1)
        {
            continue;
        }
        updateCalculator(calcId);
    }

}

void DBmanger::setMark(Student student, Task task, float value)
{
    if(!open() || student.id == -1 || task.id == -1)
    {
        return;
    }
    QSqlQuery query;
    query.prepare("SELECT _value FROM Mark WHERE task = :task AND student = :student");
    query.bindValue(":task", task.id);
    query.bindValue(":student", student.id);
    if(!exec(query, false))
    {
        return;
    }
    if(!query.first())
    {
        return;
    }
    setMarkValueById(toPositiveInt(query.value("_value")), value);
}

void DBmanger::addStudent(Student* student, bool transaction)
{
    if(!open() || student == nullptr)
    {
        return;
    }
    QSqlQuery query;
    if(transaction && !beginTransaction(query))
        return;

    query.prepare("INSERT INTO Student (name) VALUES(:name);");
    query.bindValue(":name", student->name);

    if(!exec(query))
    {
        return;
    }
    student->id = getLastId();
    if(transaction)
        commitTransaction(query);
}

void DBmanger::updateStudent(Student student)
{
    if(!open())
    {
        return;
    }
    QSqlQuery query;
    if(!beginTransaction(query))
        return;

    query.prepare("UPDATE Student SET name = :name WHERE id = :id");
    query.bindValue(":name", student.name);
    query.bindValue(":id", student.id);
    if(!exec(query))
    {
        return;
    };

    query.prepare("UPDATE ViewRow SET name = :name WHERE student = :student");
    query.bindValue(":name", student.name);
    query.bindValue(":student", student.id);
    if(!exec(query))
    {
        return;
    };
    commitTransaction(query);

}

QList<Student> DBmanger::getStudentList()
{
    QList<Student> result;
    if(!open())
    {
        return result;
    }
    QSqlQuery query("SELECT * FROM Student;");
    if(!exec(query, false))
    {
        return result;
    }

    Student buffer;
    while(query.next())
    {
        buffer.id = query.value("id").toInt();
        buffer.name = query.value("name").toString();
        result.append(buffer);
    }
    return result;
}

void DBmanger::addStudentToSubject(Student student, Subject* subject)
{
    if(!open() || student.id == -1 || subject == nullptr)
    {
        return;
    }
    QSqlQuery query;
    if(!beginTransaction(query))
        return;


    ViewRow* row = new ViewRow;
    row->name = student.name;
    row->order = subject->table->rows.size();
    row->table = subject->table;
    row->student = new Student;
    row->student->id = student.id;
    row->student->name = student.name;
    if(!addRow(row))
    {
        rollbackTransaction(query);
        return;
    }

    ViewLayer* layer = nullptr;
    for(int i = 0; i < subject->table->layers.size(); i++)
    {
        if(subject->table->layers[i]->name == "__marks")
        {
            layer = subject->table->layers[i];
            break;
        }
    }


    ViewCell* cell;
    for(int i = 0; i < subject->table->columns.size(); i++)
    {
        if(subject->table->columns[i]->task != nullptr)
        {
            cell = new ViewCell;
            cell->table = subject->table;
            cell->layer = layer;
            cell->row = row;
            cell->valueId = -1;
            cell->column = subject->table->columns[i];
            cell->valueId = addMark(student.id, subject->table->columns[i]->task->id);
            if(cell->valueId == -1)
            {
                delete cell;
                continue;
            }
            if(!addCell(cell))
            {
                delete cell;
            }

        }
    }


    commitTransaction(query);
}

void DBmanger::swapStudents(Student oldStudent, Student newStudent, Subject *subject)
{
    if(!open() || newStudent.id == -1 || subject == nullptr)
    {
        return;
    }

    QSqlQuery query;
    if(!beginTransaction(query))
        return;

    ViewRow* row = nullptr;
    for(int i = 0; i < subject->table->rows.size(); i++)
    {
        if(subject->table->rows[i]->student != nullptr && subject->table->rows[i]->student->id == oldStudent.id)
        {
            row = subject->table->rows[i];
            break;
        }
    }
    if(row == nullptr)
    {
        return;
    }

    query.prepare("UPDATE ViewRow SET name = :name, student = :student WHERE id = :id");
    query.bindValue(":name", newStudent.name);
    query.bindValue(":student", newStudent.id);
    query.bindValue(":id", row->id);
    if(!exec(query))
    {
        return;
    }
    query.prepare("UPDATE Mark SET student = :newStudent WHERE id IN (SELECT Mark.id FROM Mark "
                  "INNER JOIN Task ON Mark.task = Task.id "
                  "WHERE student = :oldStudent AND Task.subject = :subject)");
    query.bindValue(":newStudent", newStudent.id);
    query.bindValue(":oldStudent", oldStudent.id);
    query.bindValue(":subject", subject->id);
    if(!exec(query))
    {
        return;
    }

    commitTransaction(query);
}

void DBmanger::addMarks(MarkMatrix matrix, Subject *subject)
{

    if(!open())
    {
        return;
    }

    for(int i = 0; i < matrix.students.size(); i++)
    {
        if(matrix.students[i].id == -1)
        {
            addStudent(&(matrix.students[i]), false);
        }
    }

    for(int i = 0; i < matrix.tasks.size(); i++)
    {
        addTask(&(matrix.tasks[i]), subject, false);
    }



    for(int i = 0; i < matrix.students.size(); i++)
    {
        bool add = true;
        for(int j = 0; j < subject->table->rows.size(); j++)
        {
            if(subject->table->rows[j]->student != nullptr && subject->table->rows[j]->student->id == matrix.students[i].id)
            {
                add = false;
                break;
            }
        }

        if(add)
        {
            addStudentToSubject(matrix.students[i], subject);
        }
    }


    for(int i = 0; i < matrix.students.size(); i++)
    {
        for(int j = 0; j < matrix.tasks.size(); j++)
        {
            setMark(matrix.students[i], matrix.tasks[j], matrix.marks[i][j]);
        }
    }

}

void DBmanger::addSubject(QString name)
{
    if(!open())
    {
        return;
    }

    QSqlQuery query;
    if(!beginTransaction(query))
        return;

    query.prepare("INSERT INTO Subject (name) VALUES(:name);");
    query.bindValue(":name", name);
    if(!exec(query))
    {
        return;
    }

    int subjectId = getLastId();
    query.prepare("INSERT INTO ViewTable (subject) VALUES(:subject)");
    query.bindValue(":subject", subjectId);
    if(!exec(query))
    {
        return;
    }
    int viewTableId = getLastId();

    query.prepare("INSERT INTO ViewLayer (viewTable, name, _order) VALUES(:viewTable, :name, :_order)");
    query.bindValue(":viewTable", viewTableId);
    query.bindValue(":name", "__marks");
    query.bindValue(":_order", 0);
    if(!exec(query))
    {
        return;
    }
    int layerId = getLastId();
    ViewCell* cell = new ViewCell;
    cell->column = nullptr;
    cell->row = nullptr;
    cell->layer = new ViewLayer;
    cell->layer->id = layerId;
    cell->table = new ViewTable;
    cell->table->id = viewTableId;
    cell->valueId = -1;
    addCell(cell);
    delete cell->table;
    delete cell->layer;
    commitTransaction(query);
}

void DBmanger::removeSubject(Subject *subject)
{
    if(!open() || subject == nullptr)
    {
        return;
    }
    QSqlQuery query;
    if(!beginTransaction(query))
        return;

    for(int i = 0; i < subject->table->generators.size(); i++)
    {
        if(!removeGenerator(subject->table->generators[i], subject))
        {
            rollbackTransaction(query);
            return;
        }
    }


    query.prepare("SELECT id, _value FROM ViewCell WHERE viewTable = :viewTable");
    query.bindValue(":viewTable", subject->table->id);
    if(!exec(query))
    {
        return;
    }
    while(query.next())
    {
        if(toPositiveInt(query.value("_value")) != -1)
        {
            if(!removeValue(toPositiveInt(query.value("_value"))))
            {
                rollbackTransaction(query);
                return;
            }
        }
        else
        {
            if(!removeCell(toPositiveInt(query.value("id"))))
            {
                rollbackTransaction(query);
                return;
            }
        }
    }


    query.prepare("DELETE FROM ViewLayer WHERE viewTable = :viewTable");
    query.bindValue(":viewTable", subject->table->id);
    if(!exec(query))
    {
        return;
    }
    query.prepare("DELETE FROM ViewRow WHERE viewTable = :viewTable");
    query.bindValue(":viewTable", subject->table->id);
    if(!exec(query))
    {
        return;
    }
    query.prepare("DELETE FROM ViewColumn WHERE viewTable = :viewTable");
    query.bindValue(":viewTable", subject->table->id);
    if(!exec(query))
    {
        return;
    }
    query.prepare("DELETE FROM ViewTable WHERE id = :id");
    query.bindValue(":id", subject->table->id);
    if(!exec(query))
    {
        return;
    }

    query.prepare("DELETE FROM Task WHERE subject = :subject");
    query.bindValue(":subject", subject->id);
    if(!exec(query))
    {
        return;
    }

    query.prepare("DELETE FROM Subject WHERE id = :id");
    query.bindValue(":id", subject->id);
    if(!exec(query))
    {
        return;
    }

    commitTransaction(query);




}

Subject *DBmanger::getSubjectById(int id)
{
    Subject* result = new Subject;
    result->id = id;
    if(!open())
    {
        delete result;
        return nullptr;
    }
    QSqlQuery query;
    query.prepare("SELECT * FROM Subject WHERE id = :id");
    query.bindValue(":id", id);
    if(!exec(query, false))
    {
        delete result;
        return nullptr;
    }
    if(!query.first())
    {
        delete result;
        return nullptr;
    }
    result->name = query.value("name").toString();

    query.prepare("SELECT * FROM ViewTable WHERE subject = :subject");
    query.bindValue(":subject", id);
    if(!exec(query, false))
    {
        delete result;
        return nullptr;
    }
    if(!query.first())
    {
        delete result;
        return nullptr;
    }
    if(query.value("id").canConvert(QMetaType::Int))
    {
        result->table = new ViewTable;
        result->table->id = query.value("id").toInt();

        query.prepare("SELECT * FROM ViewColumn WHERE viewTable = :viewTable");
        query.bindValue(":viewTable", result->table->id);
        if(!exec(query, false))
        {
            delete result;
            return nullptr;
        }
        ViewColumn* column;
        while(query.next())
        {
            column = new ViewColumn;
            column->table = result->table;
            column->id = query.value("id").toInt();
            column->name = query.value("name").toString();
            column->order = query.value("_order").toInt();
            result->table->addColumn(column);
            if(!query.value("task").isNull())
            {
                QSqlQuery taskQuery;
                taskQuery.prepare("SELECT * FROM Task WHERE id = :id");
                taskQuery.bindValue(":id", query.value("task").toInt());
                if(!exec(taskQuery, false))
                {
                    delete result;
                    return nullptr;
                }
                if(!taskQuery.first())
                {
                    delete result;
                    return nullptr;
                }
                column->task = new Task;
                column->task->id = taskQuery.value("id").toInt();
                column->task->name = taskQuery.value("name").toString();
            }
        }

        query.prepare("SELECT * FROM ViewRow WHERE viewTable = :viewTable");
        query.bindValue(":viewTable", result->table->id);
        if(!exec(query, false))
        {
            delete result;
            return nullptr;
        }
        ViewRow* row;
        while(query.next())
        {
            row = new ViewRow;
            row->table = result->table;
            row->id = query.value("id").toInt();
            row->name = query.value("name").toString();
            row->order = query.value("_order").toInt();
            result->table->addRow(row);
            if(!query.value("student").isNull())
            {
                QSqlQuery studentQuery;
                studentQuery.prepare("SELECT * FROM student WHERE id = :id");
                studentQuery.bindValue(":id", query.value("student").toInt());
                if(!exec(studentQuery, false))
                {
                    delete result;
                    return nullptr;
                }
                if(!studentQuery.first())
                {
                    delete result;
                    return nullptr;
                }
                row->student = new Student;
                row->student->id = studentQuery.value("id").toInt();
                row->student->name = studentQuery.value("name").toString();
            }
        }

        query.prepare("SELECT * FROM ViewLayer WHERE viewTable = :viewTable");
        query.bindValue(":viewTable", result->table->id);
        if(!exec(query, false))
        {
            delete result;
            return nullptr;
        }
        ViewLayer* layer;
        while (query.next())
        {
            layer = new ViewLayer;
            layer->table = result->table;
            layer->id = query.value("id").toInt();
            layer->name = query.value("name").toString();
            layer->order = query.value("_order").toInt();
            result->table->addLayer(layer);
        }

        query.prepare("SELECT * FROM ViewCell WHERE viewTable = :viewTable");
        query.bindValue(":viewTable", result->table->id);
        if(!exec(query, false))
        {
            delete result;
            return nullptr;
        }
        ViewCell* cell;
        while (query.next())
        {
            cell = new ViewCell;

            cell->table = result->table;

            cell->id = query.value("id").toInt();
            if(!query.value("viewColumn").isNull())
            {
                cell->column = result->table->findColumnById(query.value("viewColumn").toInt());
            }
            if(!query.value("viewRow").isNull())
            {
                cell->row = result->table->findRowById(query.value("viewRow").toInt());
            }
            if(!query.value("viewLayer").isNull())
            {
                cell->layer = result->table->findLayerById(query.value("viewLayer").toInt());
                cell->layer->cells.append(cell);
            }
            if(!query.value("_value").isNull())
            {
                cell->valueId = query.value("_value").toInt();
            }
        }

        query.prepare("SELECT CellGenerator.id AS genId, CellGenerator.destination AS genDest, CellGenerator._order AS genOrder, CellGenerator.strategy AS genStrat, ViewCell.viewLayer AS destLayer FROM CellGenerator "
                      "INNER JOIN ViewCell ON CellGenerator.destination = ViewCell.id "
                      "WHERE ViewCell.viewTable = :viewTable "
                      "ORDER BY CellGenerator._order ASC");
        query.bindValue(":viewTable", result->table->id);
        if(!exec(query, false))
        {
            delete result;
            return nullptr;
        }
        CellGenerator* generator;
        while(query.next())
        {
            generator = nullptr;
            int genId = toPositiveInt(query.value("genId"));
            if(genId == -1)
            {
                continue;
            }
            generator = new CellGenerator;
            generator->id = genId;

            int genlayerId = toPositiveInt(query.value("destLayer"));
            if(genlayerId == -1)
            {
                delete generator;
                continue;
            }
            ViewLayer* destinationlayer = result->table->findLayerById(genlayerId);
            if(destinationlayer == nullptr)
            {
                delete generator;
                continue;
            }

            int cellId = toPositiveInt(query.value("genDest"));
            if(cellId == -1)
            {
                delete generator;
                continue;
            }
            ViewCell* destinationCell = destinationlayer->findCell(cellId);
            if(destinationCell == nullptr)
            {
                delete generator;
                continue;
            }

            generator->destination = destinationCell;

            int strategyId = toPositiveInt(query.value("genStrat"));
            if(strategyId == -1)
            {
                delete generator;
                continue;
            }
            generator->strategy = getStrategy(strategyId);
            if(generator->strategy == nullptr)
            {
                delete generator;
                continue;
            }

            generator->order = toPositiveInt(query.value("genOrder"));
            if(generator->order == -1)
            {
                delete generator;
                continue;
            }

            generator->parametrs = getGeneratorParamentres(generator->id, result);
            if(generator->parametrs.size() == 0)
            {
                delete generator;
                continue;
            }
            result->table->addGenerator(generator);
        }

    }
    return result;
}

QList<Subject> DBmanger::getSubjectList()
{
    QList<Subject> result;
    if(!open())
    {
        return result;
    }
    QSqlQuery query("SELECT * FROM Subject;");
    if(!exec(query, false))
    {
        return result;
    }

    Subject buffer;
    while(query.next())
    {
        buffer.id = query.value("id").toInt();
        buffer.name = query.value("name").toString();
        result.append(buffer);
    }
    return result;
}

void DBmanger::addTask(Task *task, Subject* subject, bool transaction)
{
    if(!open() || subject == nullptr || task == nullptr)
    {
        return;
    }
    QSqlQuery query;
    if(transaction && !beginTransaction(query))
        return;

    query.prepare("INSERT INTO Task (subject, name) VALUES(:subject, :name)");
    query.bindValue(":subject", subject->id);
    query.bindValue(":name", task->name);
    if(!exec(query))
    {
        return;
    }
    task->id = getLastId();

    ViewColumn* column = new ViewColumn;
    column->table = subject->table;
    column->order = subject->table->columns.size();
    column->name = task->name;
    column->task = new Task;
    column->task->id = task->id;
    if(!addColumn(column))
    {
        rollbackTransaction(query);
        return;
    }

    ViewLayer* layer = nullptr;
    for(int i = 0; i < subject->table->layers.size(); i++)
    {
        if(subject->table->layers[i]->name == "__marks")
        {
            layer = subject->table->layers[i];
            break;
        }
    }


    ViewCell* cell;
    for(int i = 0; i < subject->table->rows.size(); i++)
    {
        if(subject->table->rows[i]->student != nullptr)
        {
            cell = new ViewCell;
            cell->table = subject->table;
            cell->layer = layer;
            cell->column = column;
            cell->row = subject->table->rows[i];
            cell->valueId = addMark(subject->table->rows[i]->student->id, task->id);
            if(cell->valueId == -1)
            {
                delete cell;
                continue;
            }
            if(!addCell(cell))
            {
                delete cell;
            }
        }
    }

    if(transaction)
        commitTransaction(query);
}

void DBmanger::removeTask(int taskId)
{
    if(!open() || taskId == -1)
    {
        return;
    }
    QSqlQuery query;
    if(!beginTransaction(query))
        return;

    query.prepare("DELETE FROM Task WHERE id = :id");
    query.bindValue(":id", taskId);
    exec(query);
    commitTransaction(query);
}

void DBmanger::renameTask(Task task)
{
    if(!open() || task.id == -1)
    {
        return;
    }
    QSqlQuery query;
    if(!beginTransaction(query))
        return;

    query.prepare("UPDATE Task SET name = :name WHERE id = :id");
    query.bindValue(":name", task.name);
    query.bindValue(":id", task.id);
    if(!exec(query))
    {
        return;
    };
    query.prepare("UPDATE ViewColumn SET name = :name WHERE task = :task");
    query.bindValue(":name", task.name);
    query.bindValue(":task", task.id);
    if(!exec(query))
    {
        return;
    };
    commitTransaction(query);


}

void DBmanger::renameColumn(ViewColumn *column)
{
    if(!open() || column == nullptr)
    {
        return;
    }
    QSqlQuery query;
    if(!beginTransaction(query))
        return;

    query.prepare("UPDATE ViewColumn SET name = :name WHERE id = :id");
    query.bindValue(":name", column->name);
    query.bindValue(":id", column->id);
    if(!exec(query))
    {
        return;
    };
    commitTransaction(query);
}

void DBmanger::renameRow(ViewRow *row)
{
    if(!open() || row == nullptr)
    {
        return;
    }
    QSqlQuery query;
    if(!beginTransaction(query))
        return;

    query.prepare("UPDATE ViewRow SET name = :name WHERE id = :id");
    query.bindValue(":name", row->name);
    query.bindValue(":id", row->id);
    if(!exec(query))
    {
        return;
    };
    commitTransaction(query);
}

DBmanger::~DBmanger()
{
    db.close();
    for(int i = 0; i < strategys.size(); i++)
    {
        delete strategys[i];
    }
}

int DBmanger::getLastId()
{
    if(!open())
    {
        return -1;
    }
    QSqlQuery query;
    query.prepare("SELECT LAST_INSERT_ROWID();");
    if(!query.exec())
    {
        qDebug() << query.lastError().text();
        return -1;
    }
    query.first();
    bool ok;
    int lastId = query.value(0).toInt(&ok);
    if(ok)
    {
        return lastId;
    }
    else
    {
        return -1;
    }

}

int DBmanger::toPositiveInt(const QVariant &value)
{
    bool OK;
    int result = value.toInt(&OK);
    if(!OK)
    {
        return -1;
    }
    return result;
}

bool DBmanger::removeValue(int valueId)
{
    if(!open() || valueId == -1)
    {
        return false;
    }
    QSqlQuery query;
    query.prepare("SELECT CalculatedMark._value AS markId FROM GroupToMarkCalculator INNER JOIN CalculatedMark ON GroupToMarkCalculator.calculator = CalculatedMark.ID WHERE GroupToMarkCalculator._group IN "
                  "(SELECT _group FROM MarkToMarkGroup WHERE _group IN (SELECT _group FROM MarkToMarkGroup WHERE mark = :mark) GROUP BY _group HAVING COUNT(mark) = 1) "
                  "GROUP BY CalculatedMark._value");
    query.bindValue(":mark", valueId);
    if(!exec(query, false))
    {
        return false;
    };
    while(query.next())
    {
        removeValue(toPositiveInt(query.value("markId")));
    }

    query.prepare("DELETE FROM MarkToMarkGroup WHERE mark = :mark");
    query.bindValue(":mark", valueId);
    if(!exec(query, false))
    {
        return false;
    };

    query.prepare("SELECT id FROM CalculatedMark WHERE _value = :_value");
    query.bindValue(":_value", valueId);
    if(!exec(query, false))
    {
        return false;
    };
    if(query.first())
    {
        if(!removeCalculator(toPositiveInt(query.value("id"))))
        {
            return false;
        }
    }

    query.prepare("SELECT id FROM Mark WHERE _value = :_value");
    query.bindValue(":_value", valueId);
    if(!exec(query, false))
    {
        return false;
    };
    if(query.first())
    {
        if(!removeMark(toPositiveInt(query.value("id"))))
        {
            return false;
        }
    }

    query.prepare("DELETE FROM ViewCell WHERE _value = :_value");
    query.bindValue(":_value", valueId);
    if(!exec(query, false))
    {
        return false;
    };


    query.prepare("DELETE FROM MarkValue WHERE id = :id");
    query.bindValue(":id", valueId);
    if(!exec(query, false))
    {
        return false;
    };

    return true;

}

bool DBmanger::removeCalculator(int calculatorId)
{
    if(!open() || calculatorId == -1)
    {
        return false;
    }
    QSqlQuery query;
    query.prepare("DELETE FROM MarkToMarkGroup WHERE _group IN (SELECT _group FROM GroupToMarkCalculator WHERE calculator = :calculator)");
    query.bindValue(":calculator", calculatorId);
    if(!exec(query, false))
    {
        return false;
    };
    query.prepare("DELETE FROM MarkGroup WHERE id IN (SELECT _group FROM GroupToMarkCalculator WHERE calculator = :calculator)");
    query.bindValue(":calculator", calculatorId);
    if(!exec(query, false))
    {
        return false;
    };
    query.prepare("DELETE FROM GroupToMarkCalculator WHERE calculator = :calculator");
    query.bindValue(":calculator", calculatorId);
    if(!exec(query, false))
    {
        return false;
    };
    query.prepare("DELETE FROM CalculatedMark WHERE id = :id");
    query.bindValue(":id", calculatorId);
    if(!exec(query, false))
    {
        return false;
    };
    return true;
}

bool DBmanger::removeMark(int markId)
{
    if(!open() || markId == -1)
    {
        return false;
    }
    QSqlQuery query;
    query.prepare("DELETE FROM Mark WHERE id = :id");
    query.bindValue(":id", markId);
    if(!exec(query, false))
    {
        return false;
    };
    return true;

}

bool DBmanger::removeGenerator(CellGenerator *generator, Subject *subject)
{
    if(!open() || generator == nullptr)
    {
        return false;
    }
    QSqlQuery query;
    query.prepare("DELETE FROM CellToCellGroup WHERE _group IN (SELECT _group FROM GroupToCellGenerator WHERE generator = :generator)");
    query.bindValue(":generator", generator->id);
    if(!exec(query, false))
    {
        return false;
    };
    query.prepare("DELETE FROM CellGroup WHERE id IN (SELECT _group FROM GroupToCellGenerator WHERE generator = :generator)");
    query.bindValue(":calculator", generator->id);
    if(!exec(query, false))
    {
        return false;
    };
    query.prepare("DELETE FROM GroupToCellGenerator WHERE generator = :generator");
    query.bindValue(":calculator", generator->id);
    if(!exec(query, false))
    {
        return false;
    };
    query.prepare("DELETE FROM CellGenerator WHERE id = :id");
    query.bindValue(":id", generator->id);
    if(!exec(query, false))
    {
        return false;
    };

    delete generator;
    subject->table->generators.removeAll(generator);
    return true;
}



int DBmanger::addMark(int StudentId, int TaskId)
{
    if(!open())
    {
        return -1;
    }
    QSqlQuery query;
    query.prepare("INSERT INTO MarkValue (_value) VALUES(:_value)");
    query.bindValue(":_value", 0);
    if(!exec(query))
    {
        return -1;
    };
    int markValueId = getLastId();
    query.prepare("INSERT INTO Mark (task, student, _value) VALUES(:task, :student ,:_value)");
    query.bindValue(":task", TaskId);
    query.bindValue(":student", StudentId);
    query.bindValue(":_value", markValueId);
    if(!exec(query))
    {
        return -1;
    };
    return markValueId;
}

int DBmanger::getCalculatorOrder(int id)
{
    if(!open())
    {
        return - 1;
    }
    QSqlQuery query;
    query.prepare("SELECT MarkToMarkGroup.mark as markId FROM GroupToMarkCalculator "
                  "INNER JOIN MarkToMarkGroup ON GroupToMarkCalculator._group = MarkToMarkGroup._group "
                  "WHERE GroupToMarkCalculator.calculator = :calculator");
    query.bindValue(":calcelator", id);
    if(!exec(query))
    {
        return -1;
    };
    QString maxOrderQuery = "SELECT COALESCE(MAX(_order), -1) FROM CalculatedMark WHERE ";
    while(query.next())
    {
        int markId = toPositiveInt(query.value("markId"));
        if(markId == -1)
        {
            return -1;
        }
        maxOrderQuery += QString("_value = %1 OR ").arg(markId);
    }
    maxOrderQuery.chop(3);
    query.prepare(maxOrderQuery);
    if(!exec(query))
    {
        return -1;
    };
    if(!query.first())
    {
        return -1;
    }

    int order = toPositiveInt(query.value("_order"));
    order++;
    return order;
}

int DBmanger::addCalculator(QList<QList<int> > parameters, int strategyId)
{
    if(!open())
    {
        return - 1;
    }
    QSqlQuery query;
    query.prepare("INSERT INTO MarkValue (_value) VALUES(:_value)");
    query.bindValue(":_value", 0);
    if(!exec(query, false))
    {
        return -1;
    };
    int markValueId = getLastId();


    query.prepare("INSERT INTO CalculatedMark (_value, _order, strategy) VALUES(:_value, :_order, :strategy)");
    query.bindValue(":_value", markValueId);
    query.bindValue(":_order", 0);
    query.bindValue(":strategy", strategyId);
    if(!exec(query, false))
    {
        return -1;
    };
    int calculatorId = getLastId();


    for(int i = 0; i < parameters.size(); i++)
    {
        query.prepare("INSERT INTO MarkGroup DEFAULT VALUES");
        if(!exec(query, false))
        {
            return -1;
        };
        int groupId = getLastId();
        for(int j = 0; j < parameters[i].size(); j++)
        {
            query.prepare("INSERT INTO MarkToMarkGroup (_group, mark) VALUES(:_group , :mark)");
            query.bindValue(":_group", groupId);
            query.bindValue(":mark", parameters[i][j]);
            if(!exec(query, false))
            {
                return -1;
            };
        }
        query.prepare("INSERT INTO GroupToMarkCalculator (calculator, _group, _order) VALUES(:calculator, :_group, :_order)");
        query.bindValue(":calculator", calculatorId);
        query.bindValue(":_group", groupId);
        query.bindValue(":_order", i);
        if(!exec(query, false))
        {
            return -1;
        };
    }

    int order = getCalculatorOrder(calculatorId);
    if(order == -1)
    {
        return -1;
    }

    query.prepare("UPDATE CalculatedMark SET _order = :_order WHERE id = :id");
    query.bindValue(":_order", order);
    query.bindValue(":id", calculatorId);
    if(!exec(query, false))
    {
        return -1;
    };
    updateCalculator(calculatorId);
    return markValueId;
}

bool DBmanger::updateCalculatorParametrs(QList<CalculatorDifference> diff, int id)
{
    if(!open())
    {
        return false;
    }
    QSqlQuery query;


    for(int i = 0; i < diff.size(); i++)
    {
        query.prepare("SELECT _group FROM GroupToMarkCalculator WHERE calculator = :calculator AND _order = :_order");
        query.bindValue(":calculator", id);
        query.bindValue(":_order", i);
        if(!exec(query, false))
        {
            return false;
        };
        if(!query.first())
        {
            return false;
        }
        int groupId = toPositiveInt(query.value("_group"));
        if(groupId == -1)
        {
            return false;
        }

        for(int j = 0; j < diff[i].added.size(); j++)
        {
            query.prepare("INSERT INTO MarkToMarkGroup(_group, mark) VALUES(:_group, :mark)");
            query.bindValue(":_group", groupId);
            query.bindValue(":mark", diff[i].added[j]);
            if(!exec(query, false))
            {
                return false;
            };
        }

        for(int j = 0; j < diff[i].removed.size(); j++)
        {
            query.prepare("DELETE FROM MarkToMarkGroup WHERE _group = :_group AND mark = :mark");
            query.bindValue(":_group", groupId);
            query.bindValue(":mark", diff[i].removed[j]);
            if(!exec(query, false))
            {
                return false;
            };
        }
    }

    int order = getCalculatorOrder(id);
    if(order == -1)
    {
        return -1;
    }
    query.prepare("UPDATE CalculatedMark SET _order = :_order WHERE id = :id");
    query.bindValue(":_order", order);
    query.bindValue(":id", id);
    if(!exec(query, false))
    {
        return false;
    };


    return true;
}

int DBmanger::getCalculatorId(int markValueId)
{
    if(!open())
    {
        return - 1;
    }
    QSqlQuery query;
    query.prepare("SELECT id FROM CalculatedMark WHERE _value = :_value");
    query.bindValue(":_value", markValueId);
    if(!exec(query, false))
    {
        return -1;
    };
    if(!query.first())
    {
        return -1;
    }
    return toPositiveInt(query.value("id"));
}

void DBmanger::updateCalculator(int id)
{
    QList<QList<float>> parametrs;
    if(!open())
    {
        return;
    }
    QSqlQuery groupQuery;
    groupQuery.prepare("SELECT _group FROM GroupToMarkCalculator WHERE calculator = :calculator ORDER BY _order ASC");
    groupQuery.bindValue(":calculator", id);
    if(!exec(groupQuery, false))
    {
        return;
    };

    QSqlQuery markQuery;
    while (groupQuery.next())
    {
        QList<float> group;
        int groupId = toPositiveInt(groupQuery.value("_group"));
        if(groupId == -1)
        {
            return;
        }
        markQuery.prepare("SELECT MarkValue._value as val FROM MarkToMarkGroup INNER JOIN MarkValue ON MarkToMarkGroup.mark = MarkValue.id WHERE MarkToMarkGroup._group = :_group");
        markQuery.bindValue(":_group", groupId);
        if(!exec(markQuery, false))
        {
            return;
        };
        while (markQuery.next())
        {
            bool OK;
            float mark = markQuery.value("val").toFloat(&OK);
            if(!OK)
            {
                return;
            }
            group.append(mark);
        }
        parametrs.append(group);
    }

    groupQuery.prepare("SELECT _value, strategy FROM CalculatedMark WHERE id = :id");
    groupQuery.bindValue(":id", id);
    if(!exec(groupQuery, false))
    {
        return;
    };
    if(!groupQuery.first())
    {
        return;
    }
    int valueId = toPositiveInt(groupQuery.value("_value"));
    if(valueId == -1)
    {
        return;
    }
    int strategyId = toPositiveInt(groupQuery.value("strategy"));
    if(strategyId == -1)
    {
        return;
    }

    CalculationStrategy* strategy = getStrategy(strategyId);
    if(strategy == nullptr)
    {
        return;
    }

    float newValue = strategy->calculate(parametrs);
    setMarkValueById(valueId, newValue);
}

bool DBmanger::addCell(ViewCell* cell)
{
    if(!open())
    {
        return false;
    }
    QSqlQuery query;
    query.prepare("INSERT INTO ViewCell(viewColumn, viewRow, viewLayer, viewTable, _value) VALUES(:viewColumn, :viewRow, :viewLayer, :viewTable, :_value)");

    if(cell->column != nullptr)
        query.bindValue(":viewColumn", cell->column->id);
    else
        query.bindValue(":viewColumn", QVariant::fromValue(nullptr));

    if(cell->row != nullptr)
        query.bindValue(":viewRow", cell->row->id);
    else
        query.bindValue(":viewRow", QVariant::fromValue(nullptr));

    if(cell->layer != nullptr)
        query.bindValue(":viewLayer", cell->layer->id);
    else
        query.bindValue(":viewLayer", QVariant::fromValue(nullptr));

    if(cell->table != nullptr)
        query.bindValue(":viewTable", cell->table->id);
    else
        query.bindValue(":viewTable", QVariant::fromValue(nullptr));

    if(cell->valueId != -1)
        query.bindValue(":_value", cell->valueId);
    else
        query.bindValue(":_value", QVariant::fromValue(nullptr));
    if(!exec(query, false))
    {
        return false;
    };
    cell->id = getLastId();
    cell->layer->cells.append(cell);
    return true;
}

bool DBmanger::removeCell(int cellId)
{

    if(!open() || cellId == -1)
    {
        return false;
    }
    QSqlQuery query;
    query.prepare("DELETE FROM ViewCell WHERE id = :id");
    query.bindValue(":id", cellId);
    if(!exec(query, false))
    {
        return false;
    };
    return true;
}

bool DBmanger::addCells(QList<ViewCell *> cells)
{
    if(!open())
    {
        return false;
    }
    QSqlQuery query;
    for(int i = 0; i < cells.size(); i++)
    {
        query.prepare("INSERT INTO ViewCell(viewColumn, viewRow, viewLayer, viewTable, _value) VALUES(:viewColumn, :viewRow, :viewLayer, :viewTable, :_value)");

        if(cells[i]->column != nullptr)
            query.bindValue(":viewColumn", cells[i]->column->id);
        else
            query.bindValue(":viewColumn", QVariant::fromValue(nullptr));

        if(cells[i]->row != nullptr)
            query.bindValue(":viewRow", cells[i]->row->id);
        else
            query.bindValue(":viewRow", QVariant::fromValue(nullptr));

        if(cells[i]->layer != nullptr)
            query.bindValue(":viewLayer", cells[i]->layer->id);
        else
            return false;

        if(cells[i]->table != nullptr)
            query.bindValue(":viewTable", cells[i]->table->id);
        else
            return false;

        if(cells[i]->valueId != -1)
            query.bindValue(":_value", cells[i]->valueId);
        else
            query.bindValue(":_value", QVariant::fromValue(nullptr));
        if(!exec(query, false))
        {
            return false;
        };
        cells[i]->id = getLastId();
    }

    for(int i = 0; i < cells.size(); i++)
    {
        cells[i]->layer->cells.append(cells[i]);
    }
    return true;
}

bool DBmanger::addColumn(ViewColumn *column)
{
    if(!open())
    {
        return false;
    }
    QSqlQuery query;
    query.prepare("INSERT INTO ViewColumn(viewTable, name, _order, task) VALUES(:viewTable, :name, :_order, :task)");
    query.bindValue(":viewTable", column->table->id);
    query.bindValue(":name", column->name);
    query.bindValue(":_order", column->order);
    if(column->task == nullptr)
    {
        query.bindValue(":task", QVariant::fromValue(nullptr));
    }
    else
    {
        query.bindValue(":task", column->task->id);
    }

    if(!exec(query, false))
    {
        return false;
    }
    column->id = getLastId();

    ViewCell* cell;
    QList<ViewCell*> cells;
    for(int i = 0; i < column->table->layers.size(); i++)
    {
        cell = new ViewCell;
        cell->table = column->table;
        cell->layer = column->table->layers[i];
        cell->column = column;
        cell->row = nullptr;
        cell->valueId = -1;
        cells.append(cell);
    }

    if(!addCells(cells))
    {
        for(int i = 0; i < cells.size(); i++)
        {
            delete cells[i];
        }
        return false;
    }


    column->table->addColumn(column);
    return true;
}

bool DBmanger::addLayer(ViewLayer *layer)
{
    if(!open())
    {
        return false;
    }
    QSqlQuery query;
    query.prepare("INSERT INTO ViewLayer (viewTable, name, _order) VALUES(:viewTable, :name, :_order)");
    query.bindValue(":viewTable", layer->table->id);
    query.bindValue(":name", layer->name);
    query.bindValue(":_order", layer->order);
    if(!exec(query, false))
    {
        return false;
    }
    layer->id = getLastId();


    ViewCell* cell = new ViewCell;
    cell->table = layer->table;
    cell->layer = layer;
    cell->column = nullptr;
    cell->row = nullptr;
    cell->valueId = -1;
    QList<ViewCell*> cells;
    cells.append(cell);

    for(int i = 0; i < layer->table->columns.size(); i++)
    {
        cell = new ViewCell;
        cell->table = layer->table;
        cell->layer = layer;
        cell->column = layer->table->columns[i];
        cell->row = nullptr;
        cell->valueId = -1;
        cells.append(cell);
    }

    for(int i = 0; i < layer->table->rows.size(); i++)
    {
        cell = new ViewCell;
        cell->table = layer->table;
        cell->layer = layer;
        cell->column = nullptr;
        cell->row = layer->table->rows[i];
        cell->valueId = -1;
        cells.append(cell);
    }

    if(!addCells(cells))
    {
        for(int i = 0; i < cells.size(); i++)
        {
            delete cells[i];
        }
        return false;
    }

    layer->table->addLayer(layer);
    return true;


}

bool DBmanger::addRow(ViewRow *row)
{
    if(!open())
    {
        return false;
    }
    QSqlQuery query;
    query.prepare("INSERT INTO ViewRow(viewTable, name, _order, student) VALUES(:viewTable, :name, :_order, :student)");
    query.bindValue(":viewTable", row->table->id);
    query.bindValue(":name", row->name);
    query.bindValue(":_order", row->order);
    if(row->student == nullptr)
    {
        query.bindValue(":student", QVariant::fromValue(nullptr));
    }
    else
    {
        query.bindValue(":student", row->student->id);
    }

    if(!exec(query, false))
    {
        return false;
    }
    row->id = getLastId();

    ViewCell* cell;
    QList<ViewCell*> cells;
    for(int i = 0; i < row->table->layers.size(); i++)
    {
        cell = new ViewCell;
        cell->table = row->table;
        cell->layer = row->table->layers[i];
        cell->column = nullptr;
        cell->row = row;
        cell->valueId = -1;
        cells.append(cell);
    }

    if(!addCells(cells))
    {
        for(int i = 0; i < cells.size(); i++)
        {
            delete cells[i];
        }
        return false;
    }

    row->table->addRow(row);
    return true;
}

bool DBmanger::addGenerator(CellGenerator *generator, Subject *subject)
{
    if(!open())
    {
        return false;
    }


    if(!testGenerator(generator, subject))
    {
        return false;
    }

    setGeneratorOrder(generator, subject);

    QSqlQuery query;
    query.prepare("INSERT INTO CellGenerator(destination, _order, strategy) VALUES(:destination, :_order, :strategy)");
    query.bindValue(":destination", generator->destination->id);
    query.bindValue(":_order", generator->order);
    query.bindValue(":strategy", generator->strategy->id);
    if(!exec(query, false))
    {
        return false;
    }

    generator->id = getLastId();
    for(int i = 0; i < generator->parametrs.size(); i++)
    {
        query.prepare("INSERT INTO CellGroup DEFAULT VALUES");
        if(!exec(query, false))
        {
            return false;
        }
        int groupId = getLastId();
        for(int j = 0; j < generator->parametrs[i].size(); j++)
        {
            query.prepare("INSERT INTO CellToCellGroup(_group, cell) VALUES(:_group, :cell)");
            query.bindValue(":_group", groupId);
            query.bindValue(":cell", generator->parametrs[i][j]->id);
            if(!exec(query, false))
            {
                return false;
            }
        }
        query.prepare("INSERT INTO GroupToCellGenerator(generator, _group, _order) VALUES(:generator, :_group, :_order)");
        query.bindValue(":generator", generator->id);
        query.bindValue(":_group", groupId);
        query.bindValue(":_order", i);
        if(!exec(query, false))
        {
            return false;
        }
    }

    subject->table->addGenerator(generator);
    return true;
}

bool DBmanger::testGenerator(CellGenerator *generator, Subject *subject)
{
    if(generator->isItDepence(generator->destination, subject->table->generators))
    {
        return false;
    }

    if(generator->parametrs.contains(QList<ViewCell*>()))
    {
        return false;
    }

    return true;
}

int DBmanger::setGeneratorOrder(CellGenerator *generator, Subject *subject)
{
    for(int i = 0; i < subject->table->generators.size(); i++)
    {
        if(generator->isItDepence(subject->table->generators[i]->destination) && subject->table->generators[i]->order > generator->order)
        {
            generator->order = subject->table->generators[i]->order;
        }
    }
    generator->order++;
}

void DBmanger::applyGenerators(Subject *subject)
{
    for(int i = 0; i < subject->table->generators.size(); i++)
    {
        applyGenerator(subject->table->generators[i], subject);
    }
}

void DBmanger::applyGenerator(CellGenerator *generator, Subject *subject)
{
    ViewCell iterator;
    ViewCell* destination;
    iterator.table = generator->destination->table;
    iterator.layer = generator->destination->layer;
    iterator.column = generator->destination->column;
    iterator.row = generator->destination->row;
    for(int i = 0; i < subject->table->columns.size(); i++)
    {
        if(generator->destination->column == nullptr)
        {
            iterator.column = subject->table->columns[i];
        }

        for(int j = 0; j < subject->table->rows.size(); j++)
        {
            if(generator->destination->row == nullptr)
            {
                iterator.row = subject->table->rows[j];
            }

            destination = iterator.layer->findCell(iterator.column, iterator.row);
            if(destination == nullptr)
            {
                destination = new ViewCell;
                destination->table = iterator.table;
                destination->layer = iterator.layer;
                destination->column = iterator.column;
                destination->row = iterator.row;

                QList<QList<int>> calcParameters = getCalculatorParameters(generator->parametrs, destination);
                if(!calcParameters.contains(QList<int>()))
                {
                    destination->valueId = addCalculator(calcParameters, generator->strategy->id);
                    if(destination->valueId != -1)
                    {
                        addCell(destination);
                    }
                    else
                        delete destination;
                }
                else
                    delete destination;
            }

            if(generator->destination->row != nullptr)
            {
                break;
            }

        }

        if(generator->destination->column != nullptr)
        {
            break;
        }
    }
}

QList<QList<ViewCell *>> DBmanger::getGeneratorParamentres(int generatorId, Subject *subject)
{
    QList<QList<ViewCell *>> result;
    if(!open())
    {
        return result;
    }
    QSqlQuery groupQuery;
    groupQuery.prepare("SELECT _group FROM GroupToCellGenerator WHERE generator = :generator ORDER BY _order ASC");
    groupQuery.bindValue(":generator", generatorId);
    if(!exec(groupQuery, false))
    {
        return result;
    }

    QSqlQuery cellQuery;
    while (groupQuery.next())
    {
        cellQuery.prepare("SELECT ViewCell.id AS cellId, ViewCell.viewLayer AS cellLayer FROM CellToCellGroup "
                          "INNER JOIN ViewCell ON CellToCellGroup.cell = ViewCell.id "
                          "WHERE CellToCellGroup._group = :_group");
        cellQuery.bindValue(":_group", groupQuery.value("_group"));
        if(!exec(cellQuery, false))
        {
            continue;
        }
        QList<ViewCell*> group;
        while(cellQuery.next())
        {
            bool layerIdOK;
            int layerId = cellQuery.value("cellLayer").toInt(&layerIdOK);
            if(!layerIdOK)
            {
                continue;
            }
            ViewLayer* layer = subject->table->findLayerById(layerId);
            if(layer == nullptr)
            {
                continue;
            }
            bool cellIdOK;
            int cellId = cellQuery.value("cellId").toInt(&cellIdOK);
            if(!cellIdOK)
            {
                continue;
            }
            ViewCell* cell = layer->findCell(cellId);
            if(cell == nullptr)
            {
                continue;
            }
            group.append(cell);
        }
        result.append(group);
    }
    return result;

}

QList<QList<int> > DBmanger::getCalculatorParameters(QList<QList<ViewCell *> > cells, ViewCell *destination)
{
    QList<QList<int>> result;
    ViewLayer* layer;
    ViewCell* cell;
    for(int i = 0; i < cells.size(); i++)
    {
        QList<int> group;
        for(int j = 0 ; j < cells[i].size(); j++)
        {
            layer = cells[i][j]->layer;
            cell = layer->findCell(cells[i][j]->column == nullptr ? destination->column : cells[i][j]->column, cells[i][j]->row == nullptr ? destination->row : cells[i][j]->row);
            if(cell != nullptr)
            {
                group.append(cell->valueId);
            }
        }
        result.append(group);
    }
    return result;
}

QList<CalculatorDifference> DBmanger::getCalculatorDifference(QList<GeneratorDifference> diff, ViewCell *destination)
{
    QList<CalculatorDifference> result(diff.size());
    ViewLayer* layer;
    ViewCell* cell;
    for(int i = 0; i < diff.size(); i++)
    {
        for(int j = 0 ; j < diff[i].added.size(); j++)
        {
            layer = diff[i].added[j]->layer;
            cell = layer->findCell(diff[i].added[j]->column == nullptr ? destination->column : diff[i].added[j]->column, diff[i].added[j]->row == nullptr ? destination->row : diff[i].added[j]->row);
            if(cell != nullptr)
            {
                result[i].added.append(cell->valueId);
            }
        }

        for(int j = 0 ; j < diff[i].removed.size(); j++)
        {
            layer = diff[i].removed[j]->layer;
            cell = layer->findCell(diff[i].removed[j]->column == nullptr ? destination->column : diff[i].removed[j]->column, diff[i].removed[j]->row == nullptr ? destination->row : diff[i].removed[j]->row);
            if(cell != nullptr)
            {
                result[i].removed.append(cell->valueId);
            }
        }
    }
    return result;
}

int DBmanger::getMaxColumnOrder(int viewTableId)
{
    if(!open())
    {
        return -1;
    }
    QSqlQuery query;
    query.prepare("SELECT COALESCE(MAX(_order), 0) FROM ViewColumn WHERE viewTable=:viewTable");
    query.bindValue(":viewTable", viewTableId);
    if(!query.exec())
    {
        qDebug() << query.lastError().text();
        return -1;
    };
    if(!query.first())
    {
        return -1;
    }

    bool ok;
    int result;
    result = query.value(0).toInt(&ok);
    if(ok)
    {
        return result;
    }
    else
    {
        return -1;
    }

}

bool DBmanger::exec(QSqlQuery &query, bool rollback)
{
    if(!query.exec())
    {
        qDebug() << query.lastQuery();
        qDebug() << query.lastError().text();
        if (rollback)
            query.exec("ROLLBACK;");
        return false;
    };
    return true;
}

bool DBmanger::beginTransaction(QSqlQuery &query)
{
    query.prepare("BEGIN TRANSACTION;");
    if(!query.exec())
    {
        qDebug() << query.lastError().text();
        return false;
    }
    return true;
}

void DBmanger::rollbackTransaction(QSqlQuery &query)
{
    if(!query.exec("ROLLBACK;"))
    {
        qDebug() << query.lastError().text();
    }
}

void DBmanger::commitTransaction(QSqlQuery &query)
{
    query.prepare("COMMIT;");
    if(!query.exec())
    {
        qDebug() << query.lastError().text();
    }
}


