#ifndef PARAMETRSLISTSELECTOR_H
#define PARAMETRSLISTSELECTOR_H

#include <QWidget>
#include "parametrselector.h"
#include "subjectmodel.h"

namespace Ui {
class ParametrsListSelector;
}

class ParametrsListSelector : public QWidget
{
    Q_OBJECT

public:
    explicit ParametrsListSelector(QWidget *parent = nullptr, SubjectModel *model = nullptr);
    ~ParametrsListSelector();

    QList<CalculationParametrInfo> getInfos() const;
    void setInfos(const QList<CalculationParametrInfo> &newInfos);

    QList<QList<ViewCell *> > getSelections() const;

    SubjectModel *getModel() const;
    void setModel(SubjectModel *newModel);

signals:
    void selectionStart();
    void selectionEnd();


private slots:
    void select(int order);

private:
    Ui::ParametrsListSelector *ui;
    QList<CalculationParametrInfo> infos;
    QList<ParametrSelector*> selectors;
    SubjectModel* model;
    QList<QList<ViewCell*>> selections;
};

#endif // PARAMETRSLISTSELECTOR_H
