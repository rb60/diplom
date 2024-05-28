#ifndef GENERATOREDITOR_H
#define GENERATOREDITOR_H

#include <QWidget>
#include "DBTypes.h"
#include "subjectmodel.h"

namespace Ui {
class GeneratorEditor;
}

class GeneratorEditor : public QWidget
{
    Q_OBJECT

public:
    explicit GeneratorEditor(QWidget *parent = nullptr);
    ~GeneratorEditor();

    CellGenerator *getGenerator() const;
    void setGenerator(CellGenerator *newGenerator);

    QList<QList<ViewCell *> > getParametrs() const;

    void setModel(SubjectModel *newModel);

signals:
    void edit();
    void selectionStart();
    void selectionEnd();

private:
    Ui::GeneratorEditor *ui;
    CellGenerator* generator;
};

#endif // GENERATOREDITOR_H
