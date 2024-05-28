#ifndef SUBJECTMODEL_H
#define SUBJECTMODEL_H

#include <QAbstractTableModel>
#include <QTableView>
#include <QItemSelectionModel>
#include "dbmanger.h"
#include "simpletransferdata.h"

class SubjectModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit SubjectModel(QObject *parent = nullptr);
    Subject *getSubject() const;
    void setSubject(Subject *newSubject);
    void updateSubject();
    void setView(QTableView* view);
    QList<ViewCell*> getSelection();
    ViewColumn *getSelectedColumn();
    ViewRow *getSelectedRow();
    void clearSelection();
    SimpleTransferData *getData();

    QVariant data(const QModelIndex& index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    int getSelectedLayer() const;
    void setSelectedLayer(int newSelectedLayer);

signals:
    void selectionChange();

private:
    Subject* subject = nullptr;
    QItemSelectionModel selection;
    QList<QList<ViewCell*>> snapshot;
    void regenerateSnapshot();
    void fillSnapshot(ViewLayer* layer, bool fillHeaders = false);
    int selectedLayer = -1;

};

#endif // SUBJECTMODEL_H
