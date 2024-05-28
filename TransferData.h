#ifndef TRANSFERDATA_H
#define TRANSFERDATA_H

enum ParametrType
{
    OPEN_FILE_PATH,
    SAVE_FILE_PATH,
    STRING,
    NUMBRER,
    BOOL
};

class TransferData
{
public:
    virtual int getColumnCount() = 0;
    virtual int getRowCount() = 0;
    virtual char* getColumnName(int index) = 0;
    virtual char* getRowName(int index) = 0;
    virtual float getValue(int row, int column) = 0;
    virtual bool hasValue(int row, int column) = 0;
    virtual void destroy() = 0;
};

#endif // TRANSFERDATA_H
