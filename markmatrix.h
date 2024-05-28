#ifndef MARKMATRIX_H
#define MARKMATRIX_H
#include "DBTypes.h"

class MarkMatrix
{
public:
    MarkMatrix();
    QList<Student> students;
    QList<Task> tasks;
    QList<QList<float>> marks;
};

#endif // MARKMATRIX_H
