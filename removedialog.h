#ifndef REMOVEDIALOG_H
#define REMOVEDIALOG_H

#include <QDialog>
#include <DBTypes.h>

namespace Ui {
class RemoveDialog;
}

class RemoveDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RemoveDialog(QWidget *parent = nullptr);
    ~RemoveDialog();

    void setList(const QList<ViewCell *> &newList);

private:
    QList<ViewCell*> list;
    Ui::RemoveDialog *ui;
};

#endif // REMOVEDIALOG_H
