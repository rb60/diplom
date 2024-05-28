#ifndef IMPORTSTUDENTSELECTOR_H
#define IMPORTSTUDENTSELECTOR_H

#include <QWidget>
#include "DBTypes.h"

namespace Ui {
class ImportStudentSelector;
}

class ImportStudentSelector : public QWidget
{
    Q_OBJECT

public:
    explicit ImportStudentSelector(QWidget *parent = nullptr);
    ~ImportStudentSelector();

    void setStudents(const QList<Student> &newStudents);
    void setStudent(QString name);
    Student getStudent();

private slots:
    void changeStudent(int index);

private:
    QList<Student> students;
    Ui::ImportStudentSelector *ui;
};

#endif // IMPORTSTUDENTSELECTOR_H
