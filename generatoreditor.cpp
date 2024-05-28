#include "generatoreditor.h"
#include "ui_generatoreditor.h"

GeneratorEditor::GeneratorEditor(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GeneratorEditor)
{
    ui->setupUi(this);
    connect(ui->okPushButton, &QPushButton::clicked, this, &GeneratorEditor::edit);
    connect(ui->parametrSelectorwidget, &ParametrsListSelector::selectionStart, this, &GeneratorEditor::selectionStart);
    connect(ui->parametrSelectorwidget, &ParametrsListSelector::selectionEnd, this, &GeneratorEditor::selectionEnd);
}

GeneratorEditor::~GeneratorEditor()
{
    delete ui;
}

CellGenerator *GeneratorEditor::getGenerator() const
{
    return generator;
}

void GeneratorEditor::setGenerator(CellGenerator *newGenerator)
{
    if(newGenerator != nullptr)
    {
        generator = newGenerator;
        ui->parametrSelectorwidget->setInfos(generator->strategy->parametrs());
    }
}

QList<QList<ViewCell *> > GeneratorEditor::getParametrs() const
{
    return ui->parametrSelectorwidget->getSelections();
}

void GeneratorEditor::setModel(SubjectModel *newModel)
{
    ui->parametrSelectorwidget->setModel(newModel);
}
