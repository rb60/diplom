#ifndef STUDENTLISTFORM_H
#define STUDENTLISTFORM_H

#include <QDialog>
#include "dbmanger.h"

namespace Ui {
class StudentListForm;
}

class StudentListForm : public QDialog
{
    Q_OBJECT

public:
    explicit StudentListForm(QWidget *parent = nullptr);
    void showEvent(QShowEvent* e) override;
    ~StudentListForm();
private slots:
    void updateInputs();
    void addStudent();
    void editStudent();

private:
    Ui::StudentListForm *ui;
    QList<Student> list;
    void updateList();
};

#endif // STUDENTLISTFORM_H
