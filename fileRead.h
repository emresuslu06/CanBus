#ifndef FILEREAD_H
#define FILEREAD_H

#include <QString>
#include <QList>
#include <QMap>

class fileRead
{
public:
    fileRead();

    QList<QString> readFile();

    QMap<QString, QPair<QList<int>, QList<QPair<int, int>>>> parseLines(const QList<QString>& lines);

};

#endif
