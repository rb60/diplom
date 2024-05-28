#ifndef ADDSTUDENTWIDGET_H
#define ADDSTUDENTWIDGET_H

#include <QWidget>
#include "DBTypes.h"

namespace Ui {
class AddStudentWidget;
}

class AddStudentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AddStudentWidget(QWidget *parent = nullptr);
    ~AddStudentWidget();

    void setList(const QList<Student> &newList);
    Student getStudend() const;
signals:
    void ok();
    void cancel();

private:
    QList<Student> list;
    Student studend;
    Ui::AddStudentWidget *ui;
};

#endif // ADDSTUDENTWIDGET_H
