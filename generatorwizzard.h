#ifndef GENERATORWIZZARD_H
#define GENERATORWIZZARD_H

#include <QWidget>
#include "parametrslistselector.h"

enum GeneratorType
{
    COLUMN,
    ROW,
    LAYER
};

namespace Ui {
class GeneratorWizzard;
}

class GeneratorWizzard : public QWidget
{
    Q_OBJECT

public:
    explicit GeneratorWizzard(QWidget *parent = nullptr);
    ~GeneratorWizzard();

    CellGenerator *getGenerator() const;

    ViewLayer *getLayer() const;

    QString getName() const;

    GeneratorType getType() const;
    void setType(GeneratorType newType);

    void setStrategyList(const QList<CalculationStrategy *> &newStrategyList);

    void setLayerList(const QList<ViewLayer *> &newLayerList);

    SubjectModel *getModel() const;
    void setModel(SubjectModel *newModel);

signals:
    void ok();
    void cancel();
    void selectionStart();
    void selectionEnd();

private slots:
    void strategyChanged(int index);

private:
    Ui::GeneratorWizzard *ui;
    GeneratorType type;
    QList<CalculationStrategy*> strategyList;
    QList<ViewLayer*> layerList;
};

#endif // GENERATORWIZZARD_H
