#ifndef DBMANGER_H
#define DBMANGER_H
#include <QtSql>
#include "DBTypes.h"
#include "calculationstrategy.h"
#include "markmatrix.h"
#include <QList>

struct GeneratorDifference
{
    QList<ViewCell*> added;
    QList<ViewCell*> removed;
};

struct CalculatorDifference
{
    QList<int> added;
    QList<int> removed;
};


class DBmanger
{
public:
    static DBmanger *getInstance();
    bool open();
    void init();

    void addCalculatedColumn(CellGenerator *generator, QString name, ViewLayer* layer, Subject *subject);
    void addCalculatedRow(CellGenerator *generator, QString name, ViewLayer* layer, Subject *subject);
    void addCalculatedLayer(CellGenerator *generator, QString name, Subject *subject);
    void updateGeneratorParametrs(CellGenerator* generator, QList<QList<ViewCell*>> newParametrs, Subject* subject, bool transaction = true);

    bool removeColumn(ViewColumn* column, Subject* subject, bool transaction = true);
    bool removeRow(ViewRow* row, Subject* subject, bool transaction = true);
    bool removeLayer(ViewLayer* layer, Subject* subject, bool transaction = true);

    void changeOrder(ViewColumn* column, int newOrder);
    void changeOrder(ViewRow* row, int newOrder);
    void changeOrder(ViewLayer* layer, int newOrder);

    void addStrategy(CalculationStrategy* strategy);
    CalculationStrategy* getStrategy(int id);
    QList<CalculationStrategy*> getStrategyList();

    float getMarkValueById(int id);
    void setMarkValueById(int id, float value);
    void setMark(Student student, Task task, float value);

    void addStudent(Student *student, bool transaction = true);
    void updateStudent(Student student);
    QList<Student> getStudentList();
    void addStudentToSubject(Student student, Subject* subject);
    void swapStudents(Student oldStudent, Student newStudent, Subject* subject);

    void addMarks(MarkMatrix matrix, Subject* subject);

    void addSubject(QString name);
    void removeSubject(Subject* subject);
    Subject* getSubjectById(int id);
    QList<Subject> getSubjectList();

    void addTask(Task* task, Subject *subject, bool transaction = true);
    void removeTask(int taskId);
    void renameTask(Task task);

    void renameColumn(ViewColumn* column);
    void renameRow(ViewRow* row);



    ~DBmanger();

private:
    DBmanger();
    static DBmanger* instance;
    int getLastId();
    int toPositiveInt(const QVariant& value);
    bool removeValue(int valueId);
    bool removeCalculator(int calculatorId);
    bool removeMark(int markId);
    bool removeGenerator(CellGenerator* generator, Subject* subject);
    int addMark(int StudentId, int TaskId);
    int getCalculatorOrder(int id);
    int addCalculator(QList<QList<int>> parameters, int strategyId);
    bool updateCalculatorParametrs(QList<CalculatorDifference> diff, int id);
    int getCalculatorId(int markValueId);
    void updateCalculator(int id);
    bool addCell(ViewCell *cell);
    bool removeCell(int cellId);
    bool addCells(QList<ViewCell*> cells);
    bool addColumn(ViewColumn* column);
    bool addLayer(ViewLayer* layer);
    bool addRow(ViewRow* row);
    bool addGenerator(CellGenerator* generator, Subject* subject);
    bool testGenerator(CellGenerator* generator, Subject* subject);
    int setGeneratorOrder(CellGenerator* generator, Subject* subject);
    void applyGenerators(Subject* subject);
    void applyGenerator(CellGenerator* generator, Subject* subject);
    QList<QList<ViewCell*>> getGeneratorParamentres(int generatorId, Subject* subject);
    QList<QList<int>> getCalculatorParameters(QList<QList<ViewCell*>> cells, ViewCell* destination);
    QList<CalculatorDifference> getCalculatorDifference(QList<GeneratorDifference> diff, ViewCell* destination);
    int getMaxColumnOrder(int viewTableId);
    bool exec(QSqlQuery& query, bool rollback = true);
    bool beginTransaction(QSqlQuery& query);
    void rollbackTransaction(QSqlQuery& query);
    void commitTransaction(QSqlQuery& query);
    QList<CalculationStrategy*> strategys;
    QSqlDatabase db;
};

#endif // DBMANGER_H
