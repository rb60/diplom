#ifndef ADDTASKWIDGET_H
#define ADDTASKWIDGET_H

#include <QWidget>
#include <QString>
namespace Ui {
class AddTaskWidget;
}

class AddTaskWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AddTaskWidget(QWidget *parent = nullptr);
    ~AddTaskWidget();

    QString getTaskName();
    void showEvent(QShowEvent* e) override;

signals:
    void ok();
    void cancel();

private:
    Ui::AddTaskWidget *ui;
};

#endif // ADDTASKWIDGET_H
