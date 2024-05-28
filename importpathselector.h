#ifndef IMPORTPATHSELECTOR_H
#define IMPORTPATHSELECTOR_H

#include <QWidget>
#include "importParametrSelector.h"

namespace Ui {
class ImportPathSelector;
}

class ImportPathSelector : public QWidget, public ImportParametrSelector
{
    Q_OBJECT

public:
    explicit ImportPathSelector(QWidget *parent = nullptr);
    ~ImportPathSelector();

    void setParametr(ImportParametr parametr) override;
    QString getResult() override;

private slots:
    void selectFile();

private:
    QString result;
    bool open;
    Ui::ImportPathSelector *ui;


};

#endif // IMPORTPATHSELECTOR_H
