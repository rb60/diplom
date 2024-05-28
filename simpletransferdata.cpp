#include "simpletransferdata.h"

SimpleTransferData::SimpleTransferData() {}

int SimpleTransferData::getColumnCount()
{
    return columns.size();
}

int SimpleTransferData::getRowCount()
{
    return rows.size();
}

char *SimpleTransferData::getColumnName(int index)
{
    if(index < 0 || index >= columns.size())
    {
        return nullptr;
    }
    QByteArray ba = columns[index].toUtf8();
    char* result = new char[ba.size()];
    strcpy(result, ba.constData());
    chars.append(result);
    return result;
}

char *SimpleTransferData::getRowName(int index)
{
    if(index < 0 || index >= rows.size())
    {
        return nullptr;
    }

    QByteArray ba = rows[index].toUtf8();
    char* result = new char[ba.size()];
    strcpy(result, ba.constData());
    chars.append(result);
    return result;
}

float SimpleTransferData::getValue(int row, int column)
{
    if(row < 0 || row >= values.size() || column < 0 || column >= values[row].size())
    {
        return 0;
    }
    return values[row][column];
}

bool SimpleTransferData::hasValue(int row, int column)
{
    if(row < 0 || row >= valueExist.size() || column < 0 || column >= valueExist[row].size())
    {
        return false;
    }
    return valueExist[row][column];
}

void SimpleTransferData::destroy()
{
    for(int i = 0; i < chars.size(); i++)
    {
        delete[] chars[i];
    }
    delete this;
}
