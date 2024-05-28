#include "swapstudentwidget.h"
#include "ui_swapstudentwidget.h"

SwapStudentWidget::SwapStudentWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SwapStudentWidget)
{
    ui->setupUi(this);
    connect(ui->editPushButton, &QPushButton::clicked, this, &SwapStudentWidget::edit);
}

SwapStudentWidget::~SwapStudentWidget()
{
    delete ui;
}

Student SwapStudentWidget::getOldStudent() const
{
    return oldStudent;
}

void SwapStudentWidget::setOldStudent(const Student &newOldStudent)
{
    oldStudent = newOldStudent;
}

void SwapStudentWidget::setStudentList(const QList<Student> &newStudentList)
{
    studentList = newStudentList;
    ui->comboBox->clear();
    for(int i = 0; i < studentList.size(); i++)
    {
        ui->comboBox->addItem(studentList[i].name);
    }
    setNewStudent(newStudent);
}

Student SwapStudentWidget::getNewStudent() const
{
    if(ui->comboBox->currentIndex() >= 0 && ui->comboBox->currentIndex() < studentList.size())
    {
        return studentList[ui->comboBox->currentIndex()];
    }
    return oldStudent;
}

void SwapStudentWidget::setNewStudent(const Student &newNewStudent)
{
    newStudent = newNewStudent;
    for(int i = 0; i < studentList.size(); i++)
    {
        if(studentList[i].id == newStudent.id && i < ui->comboBox->count())
        {
            ui->comboBox->setCurrentIndex(i);
            return;
        }
    }
    ui->comboBox->setCurrentIndex(-1);
}
