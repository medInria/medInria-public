/* main.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu Sep 17 08:29:18 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Oct 14 14:33:19 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 139
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include <QtGui>
#include <QtOpenGL>
#include <QtDebug>
#include <qdebug.h>
#include "medMainWindow.h"
#include "medApplication.h"
#include "medSplashScreen.h"

#include <dtkCore/dtkGlobal.h>

#include <dtkScript/dtkScriptManager.h>

#include <medPluginManager.h>
#include <medDataIndex.h>
#include <medDatabaseController.h>
#include <medSettingsManager.h>
#include <medStorage.h>



int main(int argc, char *argv[])
{
    qRegisterMetaType<medDataIndex>("medDataIndex");
    medApplication application(argc, argv);


    if (dtkApplicationArgumentsContain(&application, "-h") || dtkApplicationArgumentsContain(&application, "--help")) {
        qDebug() << "Usage: medinria [--no-fullscreen] [--stereo]";
        return 1;
    }
    //QSplashScreen
    medSplashScreen splash(QPixmap(":/pixmaps/medinria-splash.png"));
    splash.setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    //splash.setAttribute(Qt::WA_TranslucentBackground);
    // Do not show the splash screen in debug builds because it hogs the
    // foreground, hiding all other windows. This makes debugging the startup
    // operations difficult.
#if ! defined( _DEBUG ) // && defined( WINDOWS )
    QObject::connect(medDatabaseController::instance().data(), SIGNAL(copyMessage(QString, int, QColor)), &splash, SLOT(showMessage(QString, int, QColor)) );

    application.setMsgColor(Qt::white);
    application.setMsgAlignment(Qt::AlignLeft|Qt::AlignBottom);

    QObject::connect(medPluginManager::instance(), SIGNAL(loadError(const QString&)),
                     &application, SLOT(redirectMessageToSplash(const QString&)) );
    QObject::connect(medPluginManager::instance(), SIGNAL(loaded(QString)),
                     &application, SLOT(redirectMessageToSplash(QString)) );
    QObject::connect(&application, SIGNAL(showMessage(const QString&, int, const QColor&)),
                     &splash, SLOT(showMessage(const QString&, int, const QColor&)) );
    splash.show();
    splash.showMessage("Loading plugins...",Qt::AlignLeft|Qt::AlignBottom,Qt::white);
    application.processEvents();
#endif


    // DATABASE INITIAL ROUTINE
    medSettingsManager* mnger = medSettingsManager::instance();
    // first compare the current with the new data location
    QString currentLocation = medStorage::dataLocation();

    // if the user configured a new location for the database in the settings editor, we'll need to move it
    QString newLocation = mnger->value("medDatabaseSettingsWidget", "new_database_location").toString();
    if (!newLocation.isEmpty()) {

        // if the locations are different we need to move the db to the new location
        if (currentLocation.compare(newLocation) != 0) {
            if(!medDatabaseController::instance()->moveDatabase(newLocation)){
                qDebug() << "Failed to move the database from " << currentLocation << " to " << newLocation;
                // the new location is invalid so set it to zero
                newLocation = "";
            }
        mnger->setValue("medDatabaseSettingsWidget", "actual_database_location", newLocation);
        // we need to reset the new Location to prevent doing it all the time
        mnger->setValue("medDatabaseSettingsWidget", "new_database_location","");
        }
    }
    // END OF DATABASE INITIAL ROUTINE

    medPluginManager::instance()->initialize();
    dtkScriptManager::instance()->initialize();

    medMainWindow mainwindow;
    mainwindow.show();


    if(!dtkApplicationArgumentsContain(&application, "--no-fullscreen")
    && !dtkApplicationArgumentsContain(&application, "--wall")){
        bool fullScreen  = medSettingsManager::instance()->value("startup", "fullscreen", true).toBool();
        mainwindow.setFullScreen(fullScreen);
    }

    if(application.arguments().contains("--wall"))
        mainwindow.setWallScreen(true);

    if(application.arguments().contains("--stereo")) {
       QGLFormat format;
       format.setAlpha(true);
       format.setDoubleBuffer(true);
       format.setStereo(true);
       format.setDirectRendering(true);
       QGLFormat::setDefaultFormat(format);
    }

    splash.finish(&mainwindow);

    int status = application.exec();

    medPluginManager::instance()->uninitialize();
    dtkScriptManager::instance()->uninitialize();


    return status;
}

