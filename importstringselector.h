#ifndef IMPORTSTRINGSELECTOR_H
#define IMPORTSTRINGSELECTOR_H

#include <QWidget>
#include "importParametrSelector.h"

namespace Ui {
class ImportStringSelector;
}

class ImportStringSelector : public QWidget, public ImportParametrSelector
{
    Q_OBJECT

public:
    explicit ImportStringSelector(QWidget *parent = nullptr);
    ~ImportStringSelector();

    void setParametr(ImportParametr parametr) override;
    QString getResult() override;

private:
    Ui::ImportStringSelector *ui;



};

#endif // IMPORTSTRINGSELECTOR_H
