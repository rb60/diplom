#ifndef SIMPLETRANSFERDATA_H
#define SIMPLETRANSFERDATA_H

#include "TransferData.h"
#include <QList>
#include <QString>

class SimpleTransferData : public TransferData
{
private:
    QList<char*> chars;
public:
    SimpleTransferData();
    int getColumnCount() override;
    int getRowCount() override;
    char *getColumnName(int index) override;
    char *getRowName(int index) override;
    float getValue(int row, int column) override;
    bool hasValue(int row, int column) override;
    void destroy() override;
    QList<QString> columns;
    QList<QString> rows;
    QList<QList<float>> values;
    QList<QList<bool>> valueExist;

};

#endif // SIMPLETRANSFERDATA_H
