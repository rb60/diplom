#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dbmanger.h"
#include "subjectmodel.h"
#include "studentlistform.h"
#include "generatorwizzard.h"
#include "removedialog.h"
#include "importparametr.h"
#include "importparametrlistselector.h"
#include "importmarkswidget.h"
#include "importTypes.h"
#include "exportTypes.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void showEvent(QShowEvent* e) override;
    bool eventFilter(QObject*obj, QEvent*e) override;
    ~MainWindow();

public slots:
    void openStudentList();

private slots:
    void addSubject();
    void subjectChanged();

    void showAddStudent();
    void hideAddStudent();
    void addStudent();

    void showAddTask();
    void hideAddTask();
    void addTask();

    void showAddCalc(int type);
    void hideAddCalc();
    void addCalc();

    void selectLayer(int index);

    void selectionChanged();

    void updateCalc();

    void selectionStart();
    void selectionEnd();

    void swapStudent();

    void import(int index);
    void _export(int index);

private:
    bool addingInProgres();
    void updateSubjectList();
    void updateLayerList();
    void hideEditing();
    QList<Subject> subjectList;
    QList<ImportData*> imports;
    QList<ExportData*> exports;
    StudentListForm* studentList;
    RemoveDialog* removeDialog;
    ImportParametrListSelector* importList;
    ImportMarksWidget* importMark;
    SubjectModel* subject;
    Ui::MainWindow *ui;
    bool parametrSelection = false;
};
#endif // MAINWINDOW_H
