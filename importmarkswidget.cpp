#include "importmarkswidget.h"
#include "ui_importmarkswidget.h"
#include "dbmanger.h"

ImportMarksWidget::ImportMarksWidget(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ImportMarksWidget)
{
    ui->setupUi(this);
}

ImportMarksWidget::~ImportMarksWidget()
{
    delete ui;
}

void ImportMarksWidget::setMarks(TransferData *data)
{
    for(int i = 0; i < students.size(); i++)
    {
        ui->gridLayout->removeWidget(students[i]);
        students[i]->deleteLater();
    }

    for(int i = 0; i < tasks.size(); i++)
    {
        ui->gridLayout->removeWidget(tasks[i]);
        tasks[i]->deleteLater();
    }

    for(int i = 0; i < marks.size(); i++)
    {
        for(int j = 0; j < marks[i].size(); j++)
        {
            ui->gridLayout->removeWidget(marks[i][j]);
            marks[i][j]->deleteLater();
        }
    }

    students.clear();
    tasks.clear();
    marks.clear();

    for(int i = 0; i < data->getRowCount(); i++)
    {
        ImportStudentSelector* newSelector = new ImportStudentSelector(this);
        ui->gridLayout->addWidget(newSelector, i + 1, 0);
        newSelector->setStudents(DBmanger::getInstance()->getStudentList());
        newSelector->setStudent(data->getRowName(i));
        students.append(newSelector);
    }

    for(int i = 0; i < data->getColumnCount(); i++)
    {
        QLineEdit* newEdit = new QLineEdit(this);
        ui->gridLayout->addWidget(newEdit, 0, i + 1);
        newEdit->setText(data->getColumnName(i));
        tasks.append(newEdit);
    }


    marks.resize(data->getRowCount());
    for(int i = 0; i < data->getRowCount(); i++)
    {
        for(int j = 0; j < data->getColumnCount(); j++)
        {
            QDoubleSpinBox* newSpin =  new QDoubleSpinBox(this);
            ui->gridLayout->addWidget(newSpin, i + 1, j + 1);
            newSpin->setValue(data->getValue(i,j));
            marks[i].append(newSpin);
        }
    }
}

MarkMatrix ImportMarksWidget::getMarks()
{
    MarkMatrix result;


    for(int i = 0; i < students.size(); i++)
    {
        result.students.append(students[i]->getStudent());
    }

    for(int i = 0; i < tasks.size(); i++)
    {
        result.tasks.append(Task(tasks[i]->text()));
    }

    result.marks.resize(marks.size());
    for(int i = 0; i < marks.size(); i++)
    {
        for(int j = 0; j < marks[i].size(); j++)
        {
            result.marks[i].append(marks[i][j]->value());
        }
    }

    return result;

}
