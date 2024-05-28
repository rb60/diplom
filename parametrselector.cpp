#include "parametrselector.h"
#include "ui_parametrselector.h"

ParametrSelector::ParametrSelector(CalculationParametrInfo info, int order, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ParametrSelector)
    , order(order)
{
    ui->setupUi(this);
    ui->label->setText(info.name);
    connect(ui->selectButton, &QPushButton::clicked, this, &ParametrSelector::selectSlot);
}

ParametrSelector::~ParametrSelector()
{
    delete ui;
}

void ParametrSelector::selectSlot()
{
    if(!ui->selectButton->isFlat())
    {
        ui->selectButton->setFlat(true);
        emit selectionStart(order);
    }
    else
    {
        ui->selectButton->setFlat(false);
        emit selectionEnd(order);
    }
}
