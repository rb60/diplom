#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMenuBar>
#include <QLayout>
#include "averagestrategy.h"
#include "deviationstrategy.h"
#include <QKeyEvent>
#include <QDir>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QMenuBar* menu = new QMenuBar(this);
    menu->addAction("Students", this, SLOT(openStudentList()));
    QMenu* importMenu = new QMenu("Import");
    menu->addMenu(importMenu);
    QMenu* exportMenu = new QMenu("Export");
    menu->addMenu(exportMenu);
    this->layout()->setMenuBar(menu);


    QSignalMapper* importMapper = new QSignalMapper(this);
    connect(importMapper, &QSignalMapper::mappedInt, this, &MainWindow::import);
    QDir dir(QCoreApplication::applicationDirPath());
    if(dir.cd("imports"))
    {
        QFileInfoList importFiles = dir.entryInfoList(QDir::Files);
        typedef ImportData* (*getInstance)();
        for(int i = 0; i < importFiles.size(); i++)
        {
            QString path = importFiles[i].absoluteFilePath();
            path.chop(importFiles[i].completeSuffix().size() + 1);
            QLibrary lib(path);
            if(lib.load())
            {
                getInstance instance = (getInstance) lib.resolve("getInstance");
                if(instance)
                {
                    ImportData* data = instance();
                    if(data != nullptr)
                    {
                        imports.append(data);
                        QAction* importAction = importMenu->addAction(data->getName(), importMapper, SLOT(map()));
                        importMapper->setMapping(importAction, imports.size() - 1);
                    }

                }
            }
            else
            {
                qDebug() << lib.errorString();
            }


        }
        dir.cdUp();
    }


    QSignalMapper* exportMapper = new QSignalMapper(this);
    connect(exportMapper, &QSignalMapper::mappedInt, this, &MainWindow::_export);
    if(dir.cd("exports"))
    {
        QFileInfoList exportFiles = dir.entryInfoList(QDir::Files);
        typedef ExportData* (*getInstance)();
        for(int i = 0; i < exportFiles.size(); i++)
        {
            QString path = exportFiles[i].absoluteFilePath();
            path.chop(exportFiles[i].completeSuffix().size() + 1);
            QLibrary lib(path);
            if(lib.load())
            {
                getInstance instance = (getInstance) lib.resolve("getInstance");
                if(instance)
                {
                    ExportData* data = instance();
                    if(data != nullptr)
                    {
                        exports.append(data);
                        QAction* exportAction = exportMenu->addAction(data->getName(), exportMapper, SLOT(map()));
                        exportMapper->setMapping(exportAction, exports.size() - 1);
                    }
                }
            }
            else
            {
                qDebug() << lib.errorString();
            }

        }
        dir.cdUp();
    }







    DBmanger::getInstance()->init();
    DBmanger::getInstance()->addStrategy(new AverageStrategy);
    DBmanger::getInstance()->addStrategy(new DeviationStrategy);

    subject = new SubjectModel(this);
    subject->setView(ui->tableView);
    updateSubjectList();

    studentList = new StudentListForm(this);
    removeDialog = new RemoveDialog(this);
    importList = new ImportParametrListSelector(this);
    importMark = new ImportMarksWidget(this);

    connect(ui->addSubjectPushButton,  &QPushButton::clicked, this, &MainWindow::addSubject);
    connect(ui->subjectListWidget, &QListWidget::currentRowChanged, this, &MainWindow::subjectChanged);
    connect(ui->addStudentPushButton, &QPushButton::clicked, this, &MainWindow::showAddStudent);
    connect(ui->addTaskPushButton, &QPushButton::clicked, this, &MainWindow::showAddTask);
    connect(ui->addStudentWidget, &AddStudentWidget::cancel, this, &MainWindow::hideAddStudent);
    connect(ui->addTaskWidget, &AddTaskWidget::cancel, this, &MainWindow::hideAddTask);
    connect(ui->addStudentWidget, &AddStudentWidget::ok, this, &MainWindow::addStudent);
    connect(ui->addTaskWidget, &AddTaskWidget::ok, this, &MainWindow::addTask);

    QSignalMapper* addCalcMapper = new QSignalMapper(this);
    connect(addCalcMapper, &QSignalMapper::mappedInt, this, &MainWindow::showAddCalc);

    connect(ui->addCalcColumnPushButton, &QPushButton::clicked, addCalcMapper, qOverload<>(&QSignalMapper::map));
    addCalcMapper->setMapping(ui->addCalcColumnPushButton, GeneratorType::COLUMN);

    connect(ui->addCalcRowPushButton, &QPushButton::clicked, addCalcMapper, qOverload<>(&QSignalMapper::map));
    addCalcMapper->setMapping(ui->addCalcRowPushButton, GeneratorType::ROW);

    connect(ui->addCalcLayerPushButton, &QPushButton::clicked, addCalcMapper, qOverload<>(&QSignalMapper::map));
    addCalcMapper->setMapping(ui->addCalcLayerPushButton, GeneratorType::LAYER);

    connect(ui->addGeneratorWidget, &GeneratorWizzard::cancel, this, &MainWindow::hideAddCalc);
    connect(ui->addGeneratorWidget, &GeneratorWizzard::ok, this, &MainWindow::addCalc);

    connect(subject, &SubjectModel::selectionChange, this, &MainWindow::selectionChanged);

    connect(ui->layerListWidget, &QListWidget::currentRowChanged, this, &MainWindow::selectLayer);

    connect(ui->generatorEditorWidget, &GeneratorEditor::selectionStart, this, &MainWindow::selectionStart);
    connect(ui->generatorEditorWidget, &GeneratorEditor::selectionEnd, this, &MainWindow::selectionEnd);
    connect(ui->addGeneratorWidget, &GeneratorWizzard::selectionStart, this, &MainWindow::selectionStart);
    connect(ui->addGeneratorWidget, &GeneratorWizzard::selectionEnd, this, &MainWindow::selectionEnd);

    connect(ui->generatorEditorWidget, &GeneratorEditor::edit, this, &MainWindow::updateCalc);

    connect(ui->swapStudentWidget, &SwapStudentWidget::edit, this, &MainWindow::swapStudent);

    ui->tableView->installEventFilter(this);
    ui->layerListWidget->installEventFilter(this);
    ui->subjectListWidget->installEventFilter(this);
}

void MainWindow::showEvent(QShowEvent *e)
{
    ui->addStudentWidget->hide();
    ui->addTaskWidget->hide();
    ui->addGeneratorWidget->hide();
    ui->generatorEditorWidget->hide();
    ui->swapStudentWidget->hide();
}

bool MainWindow::eventFilter(QObject* obj, QEvent* e)
{
    if(e->type() == QEvent::KeyPress)
    {
        if(obj == ui->tableView)
        {
            if(static_cast<QKeyEvent*>(e)->key() == Qt::Key_Delete)
            {
                if(subject->getSelectedColumn() != nullptr)
                {
                    removeDialog->setList(subject->getSubject()->table->getPotentialyRemoved(subject->getSelectedColumn()));
                    if(removeDialog->exec() == QDialog::Accepted)
                    {
                        int taskId = -1;
                        if(subject->getSelectedColumn()->task != nullptr)
                        {
                            taskId = subject->getSelectedColumn()->task->id;
                        }
                        DBmanger::getInstance()->removeColumn(subject->getSelectedColumn(), subject->getSubject());
                        DBmanger::getInstance()->removeTask(taskId);
                        subject->updateSubject();
                        updateLayerList();
                    }
                }

                if(subject->getSelectedRow() != nullptr)
                {
                    removeDialog->setList(subject->getSubject()->table->getPotentialyRemoved(subject->getSelectedColumn()));
                    if(removeDialog->exec() == QDialog::Accepted)
                    {
                        DBmanger::getInstance()->removeRow(subject->getSelectedRow(), subject->getSubject());
                        subject->updateSubject();
                        updateLayerList();
                    }
                }
            }

            if(static_cast<QKeyEvent*>(e)->key() == Qt::Key_Right &&
                subject->getSelectedColumn() != nullptr &&
                subject->getSelectedColumn()->order < subject->getSubject()->table->columns.size() - 1)
            {
                DBmanger::getInstance()->changeOrder(subject->getSelectedColumn(), subject->getSelectedColumn()->order + 1);
                subject->updateSubject();
                updateLayerList();
            }

            if(static_cast<QKeyEvent*>(e)->key() == Qt::Key_Left &&
                subject->getSelectedColumn() != nullptr &&
                subject->getSelectedColumn()->order > 0)
            {
                DBmanger::getInstance()->changeOrder(subject->getSelectedColumn(), subject->getSelectedColumn()->order - 1);
                subject->updateSubject();
                updateLayerList();
            }

            if(static_cast<QKeyEvent*>(e)->key() == Qt::Key_Down &&
                subject->getSelectedRow() != nullptr &&
                subject->getSelectedRow()->order < subject->getSubject()->table->rows.size() - 1)
            {
                DBmanger::getInstance()->changeOrder(subject->getSelectedRow(), subject->getSelectedRow()->order + 1);
                subject->updateSubject();
                updateLayerList();
            }

            if(static_cast<QKeyEvent*>(e)->key() == Qt::Key_Up &&
                subject->getSelectedRow() != nullptr &&
                subject->getSelectedRow()->order > 0)
            {
                DBmanger::getInstance()->changeOrder(subject->getSelectedRow(), subject->getSelectedRow()->order - 1);
                subject->updateSubject();
                updateLayerList();
            }

        }

        if(obj == ui->layerListWidget && ui->layerListWidget->currentRow() != -1)
        {
            ViewLayer* layer = subject->getSubject()->table->layers[ui->layerListWidget->currentRow()];
            if(static_cast<QKeyEvent*>(e)->key() == Qt::Key_Delete)
            {

                removeDialog->setList(subject->getSubject()->table->getPotentialyRemoved(layer));
                if(removeDialog->exec() == QDialog::Accepted)
                {
                    DBmanger::getInstance()->removeLayer(layer, subject->getSubject());
                    subject->updateSubject();
                    updateLayerList();
                }

            }

            if(static_cast<QKeyEvent*>(e)->key() == Qt::Key_Down &&
                layer->order < subject->getSubject()->table->layers.size() - 1)
            {
                DBmanger::getInstance()->changeOrder(layer, layer->order + 1);
                subject->updateSubject();
                updateLayerList();
            }

            if(static_cast<QKeyEvent*>(e)->key() == Qt::Key_Up &&
                layer->order > 0)
            {
                DBmanger::getInstance()->changeOrder(layer, layer->order - 1);
                subject->updateSubject();
                updateLayerList();
            }

        }

        if(obj == ui->subjectListWidget && ui->subjectListWidget->currentRow() != -1)
        {
            if(static_cast<QKeyEvent*>(e)->key() == Qt::Key_Delete)
            {
                if(QMessageBox::information(this, "", "Поточний курс буде видалено", QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
                {
                    DBmanger::getInstance()->removeSubject(subject->getSubject());
                    delete subject->getSubject();
                    subject->setSubject(nullptr);
                    updateSubjectList();
                    updateLayerList();
                }
            }
        }
    }


    return false;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete DBmanger::getInstance();
}

void MainWindow::openStudentList()
{
    studentList->exec();
    subject->updateSubject();
}

void MainWindow::addSubject()
{
    DBmanger::getInstance()->addSubject(ui->subjectLineEdit->text());
    updateSubjectList();
}

void MainWindow::subjectChanged()
{
    if(ui->subjectListWidget->currentRow() >= 0 && ui->subjectListWidget->currentRow() < subjectList.size())
    {
        subject->setSubject(DBmanger::getInstance()->getSubjectById(subjectList[ui->subjectListWidget->currentRow()].id));
        updateLayerList();
    }
}

void MainWindow::showAddStudent()
{
    if(!addingInProgres())
    {
        hideEditing();
        ui->addStudentWidget->setList(DBmanger::getInstance()->getStudentList());
        ui->addStudentWidget->show();
    }

}

void MainWindow::hideAddStudent()
{
    ui->addStudentWidget->hide();
}

void MainWindow::addStudent()
{
    DBmanger::getInstance()->addStudentToSubject(ui->addStudentWidget->getStudend(), subject->getSubject());
    ui->addStudentWidget->hide();
    subject->updateSubject();
    updateLayerList();
}

void MainWindow::showAddTask()
{
    if(!addingInProgres())
    {
        hideEditing();
        ui->addTaskWidget->show();
    }
}

void MainWindow::hideAddTask()
{
    ui->addTaskWidget->hide();
}

void MainWindow::addTask()
{
    Task t(ui->addTaskWidget->getTaskName());
    DBmanger::getInstance()->addTask(&t, subject->getSubject());
    ui->addTaskWidget->hide();
    subject->updateSubject();
    updateLayerList();
}

void MainWindow::showAddCalc(int type)
{
    if(!addingInProgres() && subject->getSubject() != nullptr)
    {
        hideEditing();
        ui->generatorEditorWidget->hide();
        ui->addGeneratorWidget->setType(((GeneratorType)type));
        ui->addGeneratorWidget->setModel(subject);
        ui->addGeneratorWidget->setLayerList(subject->getSubject()->table->layers);
        ui->addGeneratorWidget->setStrategyList(DBmanger::getInstance()->getStrategyList());
        ui->addGeneratorWidget->show();
    }
}

void MainWindow::hideAddCalc()
{
    ui->addGeneratorWidget->hide();
}

void MainWindow::addCalc()
{
    if(subject->getSubject() != nullptr)
    {
        switch (ui->addGeneratorWidget->getType()) {
        case GeneratorType::COLUMN:
            DBmanger::getInstance()->addCalculatedColumn(ui->addGeneratorWidget->getGenerator(), ui->addGeneratorWidget->getName(), ui->addGeneratorWidget->getLayer(), subject->getSubject());
            break;
        case GeneratorType::ROW:
            DBmanger::getInstance()->addCalculatedRow(ui->addGeneratorWidget->getGenerator(), ui->addGeneratorWidget->getName(), ui->addGeneratorWidget->getLayer(), subject->getSubject());
            break;
        case GeneratorType::LAYER:
            DBmanger::getInstance()->addCalculatedLayer(ui->addGeneratorWidget->getGenerator(), ui->addGeneratorWidget->getName(), subject->getSubject());
            break;
        default:
            break;
        }
    }
    hideAddCalc();
    subject->updateSubject();
    updateLayerList();

}

void MainWindow::selectLayer(int index)
{
    if(subject->getSelectedLayer() != -1)
    {
       subject->setSelectedLayer(index);
    }
}

void MainWindow::selectionChanged()
{
    if(!parametrSelection)
    {
        ui->layerListWidget->setCurrentRow(-1);
    }

    if(addingInProgres())
    {
        return;
    }
    if(parametrSelection)
    {
        return;
    }

    if(ui->generatorEditorWidget->isVisible())
    {
        ui->generatorEditorWidget->hide();
    }
    if(ui->swapStudentWidget->isVisible())
    {
        ui->swapStudentWidget->hide();
    }



    if(subject->getSelectedColumn() != nullptr)
    {
        for(int i = 0; i < subject->getSubject()->table->generators.size(); i++)
        {
            if(subject->getSubject()->table->generators[i]->destination->column == subject->getSelectedColumn())
            {
                ui->generatorEditorWidget->setGenerator(subject->getSubject()->table->generators[i]);
                ui->generatorEditorWidget->setModel(subject);
                ui->generatorEditorWidget->show();
                break;
            }
        }
    }

    if(subject->getSelectedRow() != nullptr)
    {
        for(int i = 0; i < subject->getSubject()->table->generators.size(); i++)
        {
            if(subject->getSubject()->table->generators[i]->destination->row == subject->getSelectedRow())
            {
                ui->generatorEditorWidget->setGenerator(subject->getSubject()->table->generators[i]);
                ui->generatorEditorWidget->setModel(subject);
                ui->generatorEditorWidget->show();
                break;
            }
        }
        if(subject->getSelectedRow()->student != nullptr)
        {
            ui->swapStudentWidget->setStudentList(DBmanger::getInstance()->getStudentList());
            ui->swapStudentWidget->setNewStudent(*(subject->getSelectedRow()->student));
            ui->swapStudentWidget->setOldStudent(*(subject->getSelectedRow()->student));
            ui->swapStudentWidget->show();
        }
    }
}

void MainWindow::updateCalc()
{
    if(subject->getSubject() != nullptr && ui->generatorEditorWidget->getGenerator() != nullptr)
    {
        DBmanger::getInstance()->updateGeneratorParametrs(ui->generatorEditorWidget->getGenerator(), ui->generatorEditorWidget->getParametrs(), subject->getSubject());
        subject->updateSubject();
        updateLayerList();
    }
}

void MainWindow::selectionStart()
{
    parametrSelection = true;
    subject->setSelectedLayer(0);
}

void MainWindow::selectionEnd()
{
    subject->clearSelection();
    subject->setSelectedLayer(-1);
    parametrSelection = false;
}

void MainWindow::swapStudent()
{
    if(subject != nullptr)
    {
        DBmanger::getInstance()->swapStudents(ui->swapStudentWidget->getOldStudent(), ui->swapStudentWidget->getNewStudent(), subject->getSubject());
        subject->updateSubject();
        updateLayerList();
    }
}

void MainWindow::import(int index)
{
    if(index < 0 || index >= imports.size() || imports[index] == nullptr)
    {
        return;
    }

    ImportData* importData = imports[index];

    QList<ImportParametr> parametrs;
    for(int i = 0; i < importData->getParametrsCount(); i++)
    {
        parametrs.append(ImportParametr(importData->getParametrName(i), (ParametrType)(importData->getParametrType(i))));
    }

    importList->setParametrs(parametrs);
    if(importList->exec() != QDialog::Accepted)
    {
        return;
    }

    char** cParametrs = new char*[importList->getResult().size()];
    for(int i = 0; i < importList->getResult().size(); i++)
    {
        QByteArray arr = importList->getResult()[i].toUtf8();
        cParametrs[i] = new char[arr.size()];
        strcpy(cParametrs[i], arr.constData());
    }

    TransferData* data = importData->getData(cParametrs);
    if(data == nullptr)
    {
        return;
    }

    importMark->setMarks(data);
    if(importMark->exec() != QDialog::Accepted)
    {
        return;
    }

    DBmanger::getInstance()->addMarks(importMark->getMarks(), subject->getSubject());
    subject->updateSubject();
    updateLayerList();
}

void MainWindow::_export(int index)
{
    if(index < 0 || index >= exports.size() || exports[index] == nullptr)
    {
        return;
    }

    ExportData* exportData = exports[index];

    QList<ImportParametr> parametrs;
    for(int i = 0; i < exportData->getParametrsCount(); i++)
    {
        parametrs.append(ImportParametr(exportData->getParametrName(i), (ParametrType)(exportData->getParametrType(i))));
    }

    importList->setParametrs(parametrs);
    if(importList->exec() != QDialog::Accepted)
    {
        return;
    }

    char** cParametrs = new char*[importList->getResult().size()];
    for(int i = 0; i < importList->getResult().size(); i++)
    {
        QByteArray arr = importList->getResult()[i].toUtf8();
        cParametrs[i] = new char[arr.size()];
        strcpy(cParametrs[i], arr.constData());
    }

    TransferData* data = subject->getData();
    if(data == nullptr)
    {
        return;
    }

    exportData->exportData(cParametrs, data);

}

bool MainWindow::addingInProgres()
{
    return  ui->addStudentWidget->isVisible() ||
            ui->addTaskWidget->isVisible() ||
            ui->addGeneratorWidget->isVisible();
}

void MainWindow::updateSubjectList()
{
    subjectList = DBmanger::getInstance()->getSubjectList();
    ui->subjectListWidget->clear();
    for(int i = 0; i < subjectList.size(); i++)
    {
        ui->subjectListWidget->addItem(subjectList[i].name);
    }
}

void MainWindow::updateLayerList()
{
    ui->layerListWidget->clear();
    if(subject->getSubject() != nullptr)
    {
        for(int i = 0 ; i < subject->getSubject()->table->layers.size(); i++)
        {
            if(subject->getSubject()->table->layers[i]->name == "__marks")
            {
                ui->layerListWidget->addItem("Оцінки");
            }
            else
            {
                ui->layerListWidget->addItem(subject->getSubject()->table->layers[i]->name);
            }
        }
    }
    subject->clearSelection();
}

void MainWindow::hideEditing()
{
    ui->generatorEditorWidget->hide();
    ui->swapStudentWidget->hide();
}

