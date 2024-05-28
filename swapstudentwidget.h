#ifndef SWAPSTUDENTWIDGET_H
#define SWAPSTUDENTWIDGET_H

#include <QWidget>
#include "DBTypes.h"

namespace Ui {
class SwapStudentWidget;
}

class SwapStudentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SwapStudentWidget(QWidget *parent = nullptr);
    ~SwapStudentWidget();

    Student getOldStudent() const;
    void setOldStudent(const Student &newOldStudent);

    void setStudentList(const QList<Student> &newStudentList);

    Student getNewStudent() const;

    void setNewStudent(const Student &newNewStudent);

signals:
    void edit();

private:
    Student oldStudent;
    QList<Student> studentList;
    Student newStudent;
    Ui::SwapStudentWidget *ui;
};

#endif // SWAPSTUDENTWIDGET_H
