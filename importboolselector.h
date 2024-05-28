#ifndef IMPORTBOOLSELECTOR_H
#define IMPORTBOOLSELECTOR_H

#include <QWidget>
#include "importParametrSelector.h"

namespace Ui {
class ImportBoolSelector;
}

class ImportBoolSelector : public QWidget, public ImportParametrSelector
{
    Q_OBJECT

public:
    explicit ImportBoolSelector(QWidget *parent = nullptr);
    ~ImportBoolSelector();

    void setParametr(ImportParametr parametr) override;
    QString getResult() override;

private:
    Ui::ImportBoolSelector *ui;


};

#endif // IMPORTBOOLSELECTOR_H
