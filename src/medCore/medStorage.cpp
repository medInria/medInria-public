/* medStorage.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 17 18:45:13 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 17 18:45:56 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 2
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medStorage.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>

#include <QtGui/QDesktopServices>
#include "medSettingsManager.h"

medStorage::medStorage(void)
{
    

}

medStorage::~medStorage(void)
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
    if (m_dataLocation != NULL)
    	return m_dataLocation;

    QVariant vDbLoc = medSettingsManager::instance()->value("database", "actual_database_location");
    if (!vDbLoc.isNull()) {
    	QString dbLoc = vDbLoc.toString();
    	return dbLoc;
    }
    else
    {
#ifdef Q_WS_MAC
    return QString(QDesktopServices::storageLocation(QDesktopServices::DataLocation))
        .remove(QCoreApplication::applicationName())
        .append(QCoreApplication::applicationName());
#else
    return QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif
    }
}

QString medStorage::configLocation(void)
{
#ifdef Q_WS_MAC
    return(QDir::homePath() + "/Library/Preferences/" + "com" + "." + QCoreApplication::organizationName() + "." + QCoreApplication::applicationName() + "." + "plist");
#else
    return(dataLocation());
#endif
}

void medStorage::setDataLocation( QString newLocation)
{
    m_dataLocation = newLocation;

    medSettingsManager::instance()->setValue("database", "actual_database_location", newLocation);
}

QString medStorage::m_dataLocation;


