#include "removedialog.h"
#include "ui_removedialog.h"

RemoveDialog::RemoveDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RemoveDialog)
{
    ui->setupUi(this);
}

RemoveDialog::~RemoveDialog()
{
    delete ui;
}

void RemoveDialog::setList(const QList<ViewCell *> &newList)
{
    list = newList;

    QString result = "Ці елементи також будуть видалені:\n";
    for(int i = 0; i < list.size(); i++)
    {
        if(list[i]->column != nullptr && list[i]->row == nullptr)
        {
            result += "Колонка \"" + list[i]->column->name + "\"\n";
        }

        if(list[i]->column == nullptr && list[i]->row != nullptr)
        {
            result += "Рядок \"" + list[i]->row->name + "\"\n";
        }

        if(list[i]->column == nullptr && list[i]->row == nullptr)
        {
            result += "Шар \"" + list[i]->layer->name + "\"\n";
        }
    }
    ui->label->setText(result);
}

