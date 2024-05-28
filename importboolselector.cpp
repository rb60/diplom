#include "importboolselector.h"
#include "ui_importboolselector.h"

ImportBoolSelector::ImportBoolSelector(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ImportBoolSelector)
{
    ui->setupUi(this);
}

ImportBoolSelector::~ImportBoolSelector()
{
    delete ui;
}

void ImportBoolSelector::setParametr(ImportParametr parametr)
{
    ui->checkBox->setText(parametr.name);
}

QString ImportBoolSelector::getResult()
{
    return (ui->checkBox->isChecked()) ? "true" : "false";
}
