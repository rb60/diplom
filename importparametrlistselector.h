#ifndef IMPORTPARAMETRLISTSELECTOR_H
#define IMPORTPARAMETRLISTSELECTOR_H

#include <QDialog>
#include "importParametrSelector.h"
#include <QList>

namespace Ui {
class ImportParametrListSelector;
}

class ImportParametrListSelector : public QDialog
{
    Q_OBJECT

public:
    explicit ImportParametrListSelector(QWidget *parent = nullptr);
    ~ImportParametrListSelector();

    void setParametrs(const QList<ImportParametr> &newParametrs);
    QList<QString> getResult();

private:
    Ui::ImportParametrListSelector *ui;
    QList<ImportParametr> parametrs;
    QList<ImportParametrSelector*> selectors;
    QList<QWidget*> widgets;

};

#endif // IMPORTPARAMETRLISTSELECTOR_H
