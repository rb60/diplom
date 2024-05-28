#ifndef IMPORTMARKSWIDGET_H
#define IMPORTMARKSWIDGET_H

#include <QDialog>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include "importstudentselector.h"
#include "TransferData.h"
#include "markmatrix.h"

namespace Ui {
class ImportMarksWidget;
}

class ImportMarksWidget : public QDialog
{
    Q_OBJECT

public:
    explicit ImportMarksWidget(QWidget *parent = nullptr);
    ~ImportMarksWidget();

    void setMarks(TransferData* data);
    MarkMatrix getMarks();

private:
    Ui::ImportMarksWidget *ui;
    QList<ImportStudentSelector*> students;
    QList<QLineEdit*> tasks;
    QList<QList<QDoubleSpinBox*>> marks;
};

#endif // IMPORTMARKSWIDGET_H
