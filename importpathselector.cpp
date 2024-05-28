#include "importpathselector.h"
#include "ui_importpathselector.h"
#include <QFileDialog>

ImportPathSelector::ImportPathSelector(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ImportPathSelector)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked, this, &ImportPathSelector::selectFile);
}

ImportPathSelector::~ImportPathSelector()
{
    delete ui;
}

void ImportPathSelector::setParametr(ImportParametr parametr)
{
    ui->label->setText(parametr.name);
    open = parametr.type == ParametrType::OPEN_FILE_PATH;
}

QString ImportPathSelector::getResult()
{
    return result;
}

void ImportPathSelector::selectFile()
{
    result = open ? QFileDialog::getOpenFileName() : QFileDialog::getSaveFileName();
}
