#include "importstudentselector.h"
#include "ui_importstudentselector.h"

ImportStudentSelector::ImportStudentSelector(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ImportStudentSelector)
{
    ui->setupUi(this);

    connect(ui->comboBox, &QComboBox::currentIndexChanged, this, &ImportStudentSelector::changeStudent);
}

ImportStudentSelector::~ImportStudentSelector()
{
    delete ui;
}

void ImportStudentSelector::setStudents(const QList<Student> &newStudents)
{
    students = newStudents;
    ui->comboBox->clear();
    ui->comboBox->addItem("Новий студент");
    for(int i = 0; i < students.size(); i++)
    {
        ui->comboBox->addItem(students[i].name);
    }
}

void ImportStudentSelector::setStudent(QString name)
{
    for(int i = 0; i < students.size(); i++)
    {
        if(students[i].name == name)
        {
            ui->comboBox->setCurrentIndex(i + 1);
            return;
        }
    }
    ui->lineEdit->setText(name);
    ui->comboBox->setCurrentIndex(0);
}

Student ImportStudentSelector::getStudent()
{
    Student result;
    if(ui->lineEdit->isEnabled())
    {
        result.name = ui->lineEdit->text();
        result.id = -1;
    }
    else
    {
        result = students[ui->comboBox->currentIndex() - 1];
    }
    return result;
}

void ImportStudentSelector::changeStudent(int index)
{
    if(index == 0)
    {
        ui->lineEdit->setEnabled(true);
    }
    else
    {
        ui->lineEdit->setEnabled(false);
    }
}
