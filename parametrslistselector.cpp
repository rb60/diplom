#include "parametrslistselector.h"
#include "ui_parametrslistselector.h"

ParametrsListSelector::ParametrsListSelector(QWidget *parent, SubjectModel* model)
    : QWidget(parent)
    , ui(new Ui::ParametrsListSelector)
    , model(model)
{
    ui->setupUi(this);
}

ParametrsListSelector::~ParametrsListSelector()
{
    delete ui;
}

QList<CalculationParametrInfo> ParametrsListSelector::getInfos() const
{
    return infos;
}

void ParametrsListSelector::setInfos(const QList<CalculationParametrInfo> &newInfos)
{
    infos = newInfos;

    selections.clear();
    selections.resize(infos.size());

    for(int i = 0; i < selectors.size(); i++)
    {
        ui->verticalLayout->removeWidget(selectors[i]);
        selectors[i]->deleteLater();
    }

    selectors.clear();

    for(int i = 0; i < infos.size(); i++)
    {
        ParametrSelector* newSelector = new ParametrSelector(infos[i], i, this);
        connect(newSelector, &ParametrSelector::selectionStart, this, &ParametrsListSelector::selectionStart);
        connect(newSelector, &ParametrSelector::selectionEnd, this, &ParametrsListSelector::select);
        ui->verticalLayout->addWidget(newSelector);
        selectors.append(newSelector);
    }
}

QList<QList<ViewCell *> > ParametrsListSelector::getSelections() const
{
    return selections;
}

void ParametrsListSelector::select(int order)
{
    if(order >= 0 && order < selections.size() && model != nullptr)
    {
        selections[order] = model->getSelection();
        emit selectionEnd();
    }
}

SubjectModel *ParametrsListSelector::getModel() const
{
    return model;
}

void ParametrsListSelector::setModel(SubjectModel *newModel)
{
    model = newModel;
}

