#ifndef MEDSTORAGE_H
#define MEDSTORAGE_H

#include <QString>

class medStorage
{
public:
    medStorage();
    ~medStorage();
    static bool mkpath(const QString& dirPath);
    static bool rmpath(const QString& dirPath);
    static QString dataLocation();
    static QString configLocation();
};

#endif // MEDSTORAGE_H
