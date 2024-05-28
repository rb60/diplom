#ifndef PARAMETRSELECTOR_H
#define PARAMETRSELECTOR_H

#include <QWidget>
#include "calculationparametrinfo.h"

namespace Ui {
class ParametrSelector;
}

class ParametrSelector : public QWidget
{
    Q_OBJECT

public:
    explicit ParametrSelector(CalculationParametrInfo info, int order,  QWidget *parent = nullptr);
    ~ParametrSelector();

signals:
    void selectionStart(int order);
    void selectionEnd(int order);

private slots:
    void selectSlot();

private:
    Ui::ParametrSelector *ui;
    int order;
};

#endif // PARAMETRSELECTOR_H
