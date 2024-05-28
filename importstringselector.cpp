#include "importstringselector.h"
#include "ui_importstringselector.h"

ImportStringSelector::ImportStringSelector(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ImportStringSelector)
{
    ui->setupUi(this);
}

ImportStringSelector::~ImportStringSelector()
{
    delete ui;
}

void ImportStringSelector::setParametr(ImportParametr parametr)
{
    ui->label->setText(parametr.name);
}

QString ImportStringSelector::getResult()
{
    return ui->lineEdit->text();
}
