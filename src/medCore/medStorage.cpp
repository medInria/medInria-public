#include "medStorage.h"
#include <QDesktopServices>
#include <QDir>

medStorage::medStorage()
{
}

medStorage::~medStorage()
{
}

bool medStorage::mkpath(const QString& dirPath)
{
    QDir dir; return(dir.mkpath(dirPath));
}

bool medStorage::rmpath(const QString& dirPath)
{
    QDir dir; return(dir.rmpath(dirPath));
}

QString medStorage::dataLocation(void)
{
#ifdef Q_WS_MAC
    return QString(QDesktopServices::storageLocation(QDesktopServices::DataLocation))
            .remove(QCoreApplication::applicationName())
            .append(QCoreApplication::applicationName());
#else
    return QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif
}

QString medStorage::configLocation(void)
{
#ifdef Q_WS_MAC
    return(QDir::homePath() + "/Library/Preferences/" + "com" + "." + QCoreApplication::organizationName() + "." + QCoreApplication::applicationName() + "." + "plist");
#else
    return(dataLocation());
#endif
}
