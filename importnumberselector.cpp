#include "importnumberselector.h"
#include "ui_importnumberselector.h"

ImportNumberSelector::ImportNumberSelector(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ImportNumberSelector)
{
    ui->setupUi(this);
}

ImportNumberSelector::~ImportNumberSelector()
{
    delete ui;
}

void ImportNumberSelector::setParametr(ImportParametr parametr)
{
    ui->label->setText(parametr.name);
}

QString ImportNumberSelector::getResult()
{
    return QString::number(ui->doubleSpinBox->value());
}
