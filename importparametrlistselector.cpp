#include "importparametrlistselector.h"
#include "ui_importparametrlistselector.h"
#include "importpathselector.h"
#include "importstringselector.h"
#include "importnumberselector.h"
#include "importboolselector.h"


ImportParametrListSelector::ImportParametrListSelector(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ImportParametrListSelector)
{
    ui->setupUi(this);
}

ImportParametrListSelector::~ImportParametrListSelector()
{
    delete ui;
}

void ImportParametrListSelector::setParametrs(const QList<ImportParametr> &newParametrs)
{
    parametrs = newParametrs;
    for(int i = 0; i < widgets.size(); i++)
    {
        ui->verticalLayout->removeWidget(widgets[i]);
        widgets[i]->deleteLater();
    }

    widgets.clear();
    selectors.clear();
    for(int i = 0; i < parametrs.size(); i++)
    {
        ImportParametrSelector* selector = nullptr;
        QWidget* widget;
        switch (parametrs[i].type) {
        case OPEN_FILE_PATH:
        case SAVE_FILE_PATH:
        {
            ImportPathSelector* pathselector = new ImportPathSelector(this);
            selector = ((ImportParametrSelector*)pathselector);
            widget = ((QWidget*)pathselector);
            break;
        }
        case STRING:
        {
            ImportStringSelector* stringSelector = new ImportStringSelector(this);
            selector = ((ImportParametrSelector*)stringSelector);
            widget = ((QWidget*)stringSelector);
            break;
        }
        case NUMBRER:
        {
            ImportNumberSelector* numberSelector = new ImportNumberSelector(this);
            selector = ((ImportParametrSelector*)numberSelector);
            widget = ((QWidget*)numberSelector);
            break;
        }
        case BOOL:
        {
            ImportBoolSelector* boolSelector = new ImportBoolSelector(this);
            selector = ((ImportParametrSelector*)boolSelector);
            widget = ((QWidget*)boolSelector);
            break;
        }
        default:
            break;
        }

        selector->setParametr(parametrs[i]);
        ui->verticalLayout->addWidget(widget);
        selectors.append(selector);
        widgets.append(widget);
    }



}

QList<QString> ImportParametrListSelector::getResult()
{
    QList<QString> result;
    for(int i = 0; i < selectors.size(); i++)
    {
        result.append(selectors[i]->getResult());
    }
    return result;
}
