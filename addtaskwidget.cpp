#include "addtaskwidget.h"
#include "ui_addtaskwidget.h"

AddTaskWidget::AddTaskWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AddTaskWidget)
{
    ui->setupUi(this);

    connect(ui->okPushButton, &QPushButton::clicked, this, &AddTaskWidget::ok);
    connect(ui->cancelPushButton, &QPushButton::clicked, this, &AddTaskWidget::cancel);
}

AddTaskWidget::~AddTaskWidget()
{
    delete ui;
}

QString AddTaskWidget::getTaskName()
{
    return ui->lineEdit->text();
}

void AddTaskWidget::showEvent(QShowEvent *e)
{
    ui->lineEdit->setText("");
}
