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
    QString vDbLoc;
    
    // first check if someone set the data location, then try to pull the actual database
    if (m_dataLocation != NULL)
    {
        return m_dataLocation;
    }
    else
    {
        vDbLoc = medSettingsManager::instance()->value("medDatabaseSettingsWidget", "actual_database_location").toString();

        // if the location is still not set we return the default paths
        if ( vDbLoc.isEmpty() )
        {
#ifdef Q_WS_MAC
            vDbLoc = QString(QDesktopServices::storageLocation(QDesktopServices::DataLocation))
                .remove(QCoreApplication::applicationName())
                .append(QCoreApplication::applicationName());
#else
            vDbLoc = QString(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
#endif   
        }
        setDataLocation(vDbLoc);
    }
    return vDbLoc;
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
    // return without writing if the location ist the same
    if(m_dataLocation != NULL)
        if(m_dataLocation.compare(newLocation) == 0)
            return;

    m_dataLocation = newLocation;

    medSettingsManager::instance()->setValue("database", "actual_database_location", newLocation);
}

QString medStorage::m_dataLocation = NULL;


