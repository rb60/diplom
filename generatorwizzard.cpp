#include "generatorwizzard.h"
#include "ui_generatorwizzard.h"

GeneratorWizzard::GeneratorWizzard(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GeneratorWizzard)
{
    ui->setupUi(this);

    connect(ui->strategyComboBox, &QComboBox::currentIndexChanged, this, &GeneratorWizzard::strategyChanged);
    connect(ui->okPushButton, &QPushButton::clicked, this, &GeneratorWizzard::ok);
    connect(ui->cancelPushButton, &QPushButton::clicked, this, &GeneratorWizzard::cancel);
    connect(ui->paramertsSelector, &ParametrsListSelector::selectionStart, this, &GeneratorWizzard::selectionStart);
    connect(ui->paramertsSelector, &ParametrsListSelector::selectionEnd, this, &GeneratorWizzard::selectionEnd);
}

GeneratorWizzard::~GeneratorWizzard()
{
    delete ui;
}

CellGenerator* GeneratorWizzard::getGenerator() const
{
    CellGenerator* result = new CellGenerator;
    result->parametrs = ui->paramertsSelector->getSelections();
    if(ui->strategyComboBox->currentIndex() < 0 || ui->strategyComboBox->currentIndex() >= strategyList.size())
    {
        result->strategy = nullptr;
    }
    else
    {
        result->strategy = strategyList[ui->strategyComboBox->currentIndex()];
    }
    return result;
}

ViewLayer *GeneratorWizzard::getLayer() const
{
    if(ui->layerComboBox->currentIndex() < 1 || ui->layerComboBox->currentIndex() >= layerList.size() + 1 || type == GeneratorType::LAYER)
    {
        return nullptr;
    }

    return layerList[ui->layerComboBox->currentIndex() - 1];


}

QString GeneratorWizzard::getName() const
{
    return ui->nameLineEdit->text();
}

GeneratorType GeneratorWizzard::getType() const
{
    return type;
}

void GeneratorWizzard::setType(GeneratorType newType)
{
    type = newType;
    if(type == GeneratorType::LAYER)
    {
        ui->layerComboBox->setEnabled(false);
    }
    else
    {
        ui->layerComboBox->setEnabled(true);
    }
}

void GeneratorWizzard::setStrategyList(const QList<CalculationStrategy *> &newStrategyList)
{
    strategyList = newStrategyList;

    ui->strategyComboBox->clear();

    for(int i = 0; i < strategyList.size(); i++)
    {
        ui->strategyComboBox->addItem(strategyList[i]->getName());
    }
}

void GeneratorWizzard::setLayerList(const QList<ViewLayer *> &newLayerList)
{
    layerList = newLayerList;

    for(int i = 0; i < layerList.size(); i++)
    {
        if(layerList[i]->name == "__marks")
        {
            layerList.remove(i);
            break;
        }
    }

    ui->layerComboBox->clear();
    ui->layerComboBox->addItem("Новий шар");

    for(int i = 0; i < layerList.size(); i++)
    {
        ui->layerComboBox->addItem(layerList[i]->name);
    }
}

void GeneratorWizzard::strategyChanged(int index)
{
    if(index < 0 || index >= strategyList.size())
    {
        return;
    }

    ui->paramertsSelector->setInfos(strategyList[index]->parametrs());

}

SubjectModel *GeneratorWizzard::getModel() const
{
    return ui->paramertsSelector->getModel();
}

void GeneratorWizzard::setModel(SubjectModel *newModel)
{
    ui->paramertsSelector->setModel(newModel);
}
