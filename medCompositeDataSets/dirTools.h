#ifndef DIRTOOLS_H
#define DIRTOOLS_H

#include <QDir>
#include <QString>

bool RemoveDirectory(const QDir& dir);
QString zip_dirname(const QString& name);

#endif  //  ! DIRTOOLS_H
