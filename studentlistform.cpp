#include "studentlistform.h"
#include "ui_studentlistform.h"


StudentListForm::StudentListForm(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::StudentListForm)
{
    ui->setupUi(this);
    connect(ui->listWidget, &QListWidget::currentRowChanged, this, &StudentListForm::updateInputs);
    connect(ui->addPushButton, &QPushButton::clicked, this, &StudentListForm::addStudent);
    connect(ui->editPushButton, &QPushButton::clicked, this, &StudentListForm::editStudent);
}

void StudentListForm::showEvent(QShowEvent *e)
{
    updateList();
    updateInputs();
}

StudentListForm::~StudentListForm()
{
    delete ui;
}

void StudentListForm::updateInputs()
{
    if(ui->listWidget->currentRow() >= 0 && ui->listWidget->currentRow() < list.size())
    {
        ui->nameLineEdit->setText(list[ui->listWidget->currentRow()].name);
    }
    else
    {
        ui->nameLineEdit->setText("");
    }

}

void StudentListForm::addStudent()
{
    Student s(ui->nameLineEdit->text());
    DBmanger::getInstance()->addStudent(&s);
    updateList();
    updateInputs();
}

void StudentListForm::editStudent()
{
    if(ui->listWidget->currentRow() >= 0 && ui->listWidget->currentRow() < list.size())
    {
        Student student = list[ui->listWidget->currentRow()];
        student.name = ui->nameLineEdit->text();
        DBmanger::getInstance()->updateStudent(student);
        updateList();
        updateInputs();
    }
}

void StudentListForm::updateList()
{
    list = DBmanger::getInstance()->getStudentList();
    ui->listWidget->clear();
    for(int i = 0; i < list.size(); i++)
    {
        ui->listWidget->addItem(list[i].name);
    }
}
