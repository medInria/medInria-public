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
