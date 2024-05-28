#include "subjectmodel.h"

SubjectModel::SubjectModel(QObject *parent)
    : QAbstractTableModel{parent}
{

    connect(&selection, &QItemSelectionModel::selectionChanged, this, &SubjectModel::selectionChange);

}

Subject *SubjectModel::getSubject() const
{
    return subject;
}

void SubjectModel::setSubject(Subject *newSubject)
{
    subject = newSubject;
    regenerateSnapshot();
}

void SubjectModel::updateSubject()
{
    if(subject != nullptr)
    {
        selection.clearSelection();
        int id = subject->id;
        delete subject;
        setSubject(DBmanger::getInstance()->getSubjectById(id));
    }
}

void SubjectModel::setView(QTableView *view)
{
    view->setModel(this);
    selection.setModel(this);
    view->setSelectionModel(&selection);
}

QList<ViewCell *> SubjectModel::getSelection()
{
    QList<ViewCell*> result;
    for(int i = 0; i < selection.selectedIndexes().size(); i++)
    {
        result.append(snapshot[selection.selectedIndexes()[i].row()][selection.selectedIndexes()[i].column()]);
    }
    return result;
}

ViewColumn *SubjectModel::getSelectedColumn()
{
    if(selection.selectedIndexes().size() != 1)
    {
        return nullptr;
    }
    if(selection.selectedIndexes().first().row() != 0)
    {
        return nullptr;
    }
    if(selection.selectedIndexes().first().column() == 0)
    {
        return nullptr;
    }
    return subject->table->columns[selection.selectedIndexes().first().column() - 1];
}

ViewRow *SubjectModel::getSelectedRow()
{
    if(selection.selectedIndexes().size() != 1)
    {
        return nullptr;
    }
    if(selection.selectedIndexes().first().column() != 0)
    {
        return nullptr;
    }
    if(selection.selectedIndexes().first().row() == 0)
    {
        return nullptr;
    }
    return subject->table->rows[selection.selectedIndexes().first().row() - 1];
}

void SubjectModel::clearSelection()
{
    selection.clearSelection();
}

SimpleTransferData *SubjectModel::getData()
{
    SimpleTransferData* data = new SimpleTransferData;
    for(int i = 0; i < subject->table->columns.size(); i++)
    {
        data->columns.append(subject->table->columns[i]->name);
    }

    for(int i = 0; i < subject->table->rows.size(); i++)
    {
        data->rows.append(subject->table->rows[i]->name);
    }

    data->valueExist.resize(snapshot.size() - 1);
    data->values.resize(snapshot.size() - 1);
    for(int i = 1; i < snapshot.size(); i++)
    {
        data->valueExist[i - 1].resize(snapshot[i].size() - 1);
        data->values[i - 1].resize(snapshot[i].size() - 1);
        for(int j = 1; j < snapshot[i].size(); j++)
        {
            if(snapshot[i][j] != nullptr)
            {
                data->valueExist[i - 1][j - 1] = true;
                data->values[i - 1][j - 1] = DBmanger::getInstance()->getMarkValueById(snapshot[i][j]->valueId);
            }
            else
            {
                data->valueExist[i - 1][j - 1] = false;
                data->values[i - 1][j - 1] = 0;
            }

        }
    }

    return data;

}

QVariant SubjectModel::data(const QModelIndex &index, int role) const
{
    if(subject == nullptr || subject->table == nullptr)
    {
        return QVariant();
    }

    if(!index.isValid())
    {
        return QVariant();
    }

    if(index.row() < 0 || index.row() >= snapshot.size() || index.column() < 0 ||index.column() >= snapshot.first().size())
    {
        return QVariant();
    }

    if(role != Qt::DisplayRole && role != Qt::EditRole)
    {
        return QVariant();
    }

    if(index.column() == 0 && index.row() == 0)
    {
        return QVariant();
    }

    if(index.column() == 0)
    {
        return QVariant(subject->table->findRowByOrder(index.row() - 1)->name);
    }

    if(index.row() == 0)
    {
        return QVariant(subject->table->findColumnByOrder(index.column() - 1)->name);
    }

    if(snapshot[index.row()][index.column()] == nullptr)
    {
        return QVariant();
    }

    return QVariant(DBmanger::getInstance()->getMarkValueById(snapshot[index.row()][index.column()]->valueId));

}

bool SubjectModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(subject == nullptr || subject->table == nullptr)
    {
        return false;
    }

    if(!index.isValid())
    {
        return false;
    }

    if(index.row() < 0 || index.row() >= snapshot.size() || index.column() < 0 ||index.column() >= snapshot.first().size())
    {
        return false;
    }

    if(role != Qt::EditRole)
    {
        return false;
    }

    if(!value.canConvert(QMetaType::Float))
    {
        return false;
    }

    if(index.column() == 0 && subject->table->rows[index.row() -1]->student == nullptr)
    {
        subject->table->rows[index.row() -1]->name = value.toString();
        DBmanger::getInstance()->renameRow(subject->table->rows[index.row() -1]);
        emit dataChanged(index, index);
        return true;
    }

    if(index.row() == 0)
    {
        if(subject->table->columns[index.column() - 1]->task == nullptr)
        {
            Task t = *(subject->table->columns[index.column() - 1]->task);
            t.name = value.toString();
            DBmanger::getInstance()->renameTask(t);
        }
        else
        {
            subject->table->columns[index.column() - 1]->name = value.toString();
            DBmanger::getInstance()->renameColumn(subject->table->columns[index.column() - 1]);
        }
        emit dataChanged(index, index);
        return true;
    }

    if(snapshot[index.row()][index.column()] == nullptr)
    {
        return false;
    }

    DBmanger::getInstance()->setMarkValueById(snapshot[index.row()][index.column()]->valueId, value.toFloat());
    emit dataChanged(index, index);
    return true;
}

int SubjectModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
    {
        return 0;
    }
    return snapshot.size();
}

int SubjectModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
    {
        return 0;
    }
    if(snapshot.size() == 0)
    {
        return 0;
    }
    return snapshot.first().size();
}

QVariant SubjectModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

Qt::ItemFlags SubjectModel::flags(const QModelIndex &index) const
{
    if(subject == nullptr || subject->table == nullptr)
    {
        return Qt::NoItemFlags;
    }

    if(index.row() < 0 || index.row() >= snapshot.size() || index.column() < 0 || index.column() >= snapshot.first().size())
    {
        return Qt::NoItemFlags;
    }

    if(!index.isValid())
    {
        return QAbstractTableModel::flags(index);
    }

    if(index.column() == 0 && index.row() > 0 && subject->table->rows[index.row() - 1]->student != nullptr)
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }

    if(index.row() == 0 || index.column() == 0)
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
    }

    if(snapshot[index.row()][index.column()] == nullptr)
    {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

void SubjectModel::regenerateSnapshot()
{
    if(subject == nullptr || subject->table == nullptr)
    {
        beginResetModel();
        snapshot = QList<QList<ViewCell*>>();
        endResetModel();
        return;
    }
    int layerCount = subject->table->layers.size();
    int rowsCount = subject->table->rows.size() + 1;
    int columnCount = subject->table->columns.size() + 1;
    beginResetModel();
    snapshot = QList<QList<ViewCell*>>(rowsCount, QList<ViewCell*>(columnCount, nullptr));

    if(selectedLayer != -1)
    {
        ViewLayer* layer = subject->table->layers[selectedLayer];
        ViewRow* row = nullptr;
        ViewColumn* column = nullptr;
        snapshot[0][0] = layer->findCell(column, row);
        for(int i = 0; i < rowsCount - 1; i++)
        {
            row = subject->table->rows[i];
            snapshot[i + 1][0] = layer->findCell(column, row);
        }
        row = nullptr;
        for(int i = 0; i < columnCount - 1; i++)
        {
            column = subject->table->columns[i];
            snapshot[0][i + 1] = layer->findCell(column, row);
        }
        fillSnapshot(layer);
    }
    else
    {
        for(int i = 0; i < layerCount; i++)
        {
            fillSnapshot(subject->table->layers[i]);
        }
    }
    endResetModel();
}

void SubjectModel::fillSnapshot(ViewLayer *layer, bool fillHeaders)
{
    if(layer == nullptr)
    {
        return;
    }

    int rowIndex, columnIndex;
    for(int i = 0; i < layer->cells.size(); i++)
    {


        if(layer->cells[i]->row == nullptr)
        {
            if(!fillHeaders)
            {
                continue;
            }
            rowIndex = 0;
        }
        else
        {
            rowIndex = layer->cells[i]->row->order + 1;
        }

        if(layer->cells[i]->column == nullptr)
        {
            if(!fillHeaders)
            {
                continue;
            }
            columnIndex = 0;
        }
        else
        {
            columnIndex = layer->cells[i]->column->order + 1;
        }

        if(snapshot[rowIndex][columnIndex] == nullptr)
        {
            snapshot[rowIndex][columnIndex] = layer->cells[i];
        }
    }
}

int SubjectModel::getSelectedLayer() const
{
    return selectedLayer;
}

void SubjectModel::setSelectedLayer(int newSelectedLayer)
{
    selectedLayer = newSelectedLayer;
    regenerateSnapshot();
}
