#ifndef IMPORTNUMBERSELECTOR_H
#define IMPORTNUMBERSELECTOR_H

#include <QWidget>
#include "importParametrSelector.h"

namespace Ui {
class ImportNumberSelector;
}

class ImportNumberSelector : public QWidget, public ImportParametrSelector
{
    Q_OBJECT

public:
    explicit ImportNumberSelector(QWidget *parent = nullptr);
    ~ImportNumberSelector();

    void setParametr(ImportParametr parametr) override;
    QString getResult() override;

private:
    Ui::ImportNumberSelector *ui;

};

#endif // IMPORTNUMBERSELECTOR_H
