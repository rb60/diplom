#include "addstudentwidget.h"
#include "ui_addstudentwidget.h"

AddStudentWidget::AddStudentWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AddStudentWidget)
{
    ui->setupUi(this);
    connect(ui->okPushButton, &QPushButton::clicked, this, &AddStudentWidget::ok);
    connect(ui->cancelPushButton, &QPushButton::clicked, this, &AddStudentWidget::cancel);
}

AddStudentWidget::~AddStudentWidget()
{
    delete ui;
}

void AddStudentWidget::setList(const QList<Student> &newList)
{
    list = newList;
    ui->comboBox->clear();
    for(int i = 0; i < list.size(); i++)
    {
        ui->comboBox->addItem(list[i].name);
    }
}

Student AddStudentWidget::getStudend() const
{
    if(ui->comboBox->currentIndex() >= 0 && ui->comboBox->currentIndex() < list.size())
    {
        return list[ui->comboBox->currentIndex()];
    }
    else
    {
        return Student();
    }
}
