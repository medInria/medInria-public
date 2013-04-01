/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QtGui>
#include <QtOpenGL>
#include <QtDebug>
#include <qdebug.h>
#include "medMainWindow.h"
#include "medApplication.h"
#include "medSplashScreen.h"

#include <dtkCore>

#include <medPluginManager.h>
#include <medDataIndex.h>
#include <medDatabaseController.h>
#include <medSettingsManager.h>
#include <medStorage.h>

void forceShow(medMainWindow& mainwindow )
{
    //Idea and code taken from the OpenCOR project, Thanks Allan for the code!


    // Note: to show ourselves, one would normally use activateWindow(), but
    //       depending on the operating system it may or not bring OpenCOR to
    //       the foreground, so... instead we do what follows, depending on the
    //       operating system...

#if defined(Q_WS_WIN)
    // Retrieve window Id

    WId mainWinId = mainwindow.winId();

    // Bring main window to the foreground
    SetForegroundWindow(mainWinId);

    // Show/restore the window, depending on its current state

    if (IsIconic(mainWinId))
        ShowWindow(mainWinId, SW_RESTORE);
    else
        ShowWindow(mainWinId, SW_SHOW);

    // Note: under Windows, to use activateWindow() will only highlight the
    //       application in the taskbar, since under Windows no application
    //       should be allowed to bring itself to the foreground when another
    //       application is already in the foreground. Fair enough, but it
    //       happens that, here, the user wants OpenCOR to be brought to the
    //       foreground, hence the above code to get the effect we are after...
#else
    mainwindow.activateWindow();
    mainwindow.raise();
#endif
}

int main(int argc,char* argv[]) {

    qRegisterMetaType<medDataIndex>("medDataIndex");
    medApplication application(argc,argv);

    if (dtkApplicationArgumentsContain(&application, "-h") || dtkApplicationArgumentsContain(&application, "--help")) {
        qDebug() << "Usage: medInria [--no-fullscreen] [--stereo] [--view] [files]]";
        return 1;
    }

    // Do not show the splash screen in debug builds because it hogs the
    // foreground, hiding all other windows. This makes debugging the startup
    // operations difficult.

    #if !defined(_DEBUG)
    bool show_splash = true;
    #else
    bool show_splash = false;
    #endif

    medSettingsManager* mnger = medSettingsManager::instance();

    QStringList posargs;
    for (int i=1;i<application.argc();++i) {
        const QString arg = application.argv()[i];
        if (arg.startsWith("--")) {
            bool valid_option = false;
            const QStringList options = (QStringList() << "--no-fullscreen" << "--stereo" << "--view");
            for (QStringList::const_iterator i=options.constBegin();i!=options.constEnd();++i)
                if (arg==*i)
                    valid_option = true;
            if (!valid_option) { qDebug() << "Ignoring unknown option " << arg; }
            continue;
        }
        posargs.append(arg);
    }

    const bool DirectView = dtkApplicationArgumentsContain(&application,"--view") || posargs.size()!=0;
    int runningMedInria = 0;
    if (DirectView) {
        show_splash = false;
        for (QStringList::const_iterator i=posargs.constBegin();i!=posargs.constEnd();++i) {
            const QString& message = QString("/open ")+*i;
            runningMedInria = application.sendMessage(message);
        }
    } else {
        runningMedInria = application.sendMessage("");    
    }

    if (runningMedInria)
        return 0;

    medSplashScreen splash(QPixmap(":/pixmaps/medInria-splash.png"));
    if (show_splash) {

        QObject::connect(medDatabaseController::instance().data(),
                         SIGNAL(copyMessage(QString,int,QColor)),
                         &splash,SLOT(showMessage(QString,int, QColor)));

        application.setMsgColor(Qt::white);
        application.setMsgAlignment(Qt::AlignLeft|Qt::AlignBottom);

        QObject::connect(medPluginManager::instance(),SIGNAL(loadError(const QString&)),
                         &application,SLOT(redirectMessageToSplash(const QString&)) );
        QObject::connect(medPluginManager::instance(),SIGNAL(loaded(QString)),
                         &application,SLOT(redirectMessageToSplash(QString)) );
        QObject::connect(&application,SIGNAL(showMessage(const QString&, int, const QColor&)),
                         &splash,SLOT(showMessage(const QString&, int, const QColor&)) );
        splash.show();
        splash.showMessage("Loading plugins...",Qt::AlignLeft|Qt::AlignBottom,Qt::white);
    }

    //  DATABASE INITIALISATION.
    //  First compare the current with the new data location

    QString currentLocation = medStorage::dataLocation();

    //  If the user configured a new location for the database in the settings editor, we'll need to move it

    QString newLocation = mnger->value("medDatabaseSettingsWidget", "new_database_location").toString();
    if (!newLocation.isEmpty()) {

        //  If the locations are different we need to move the db to the new location

        if (currentLocation.compare(newLocation)!=0) {
            if (!medDatabaseController::instance()->moveDatabase(newLocation)) {
                qDebug() << "Failed to move the database from " << currentLocation << " to " << newLocation;
                //  The new location is invalid so set it to zero
                newLocation = "";
            }
            mnger->setValue("medDatabaseSettingsWidget", "actual_database_location",newLocation);

            //  We need to reset the new Location to prevent doing it all the time

            mnger->setValue("medDatabaseSettingsWidget", "new_database_location","");
        }
    }
    // END OF DATABASE INITIALISATION

    medPluginManager::instance()->initialize();

    medMainWindow mainwindow;
    if (DirectView)
        mainwindow.setStartup(medMainWindow::WorkSpace,posargs);

    forceShow(mainwindow);

    bool fullScreen = medSettingsManager::instance()->value("startup", "fullscreen", false).toBool();

    #ifdef ACTIVATE_WALL_OPTION
    if (application.arguments().contains("--wall"))
    {
        mainwindow.setWallScreen(true);
        fullScreen = false;
    }
    #endif

    const bool hasFullScreenArg = application.arguments().contains("--fullscreen");
    const bool hasNoFullScreenArg = application.arguments().contains("--no-fullscreen");

    if (hasFullScreenArg && hasNoFullScreenArg)
        dtkWarn() << "Conflicting command line parameters --fullscreen and --no-fullscren. Ignoring.";

    else if (hasNoFullScreenArg)
        fullScreen = false;
    else if (hasFullScreenArg)
        fullScreen = true;

    mainwindow.setFullScreen(fullScreen);

    if(application.arguments().contains("--stereo")) {
       QGLFormat format;
       format.setAlpha(true);
       format.setDoubleBuffer(true);
       format.setStereo(true);
       format.setDirectRendering(true);
       QGLFormat::setDefaultFormat(format);
    }

    if (show_splash)
        splash.finish(&mainwindow);

    if (medPluginManager::instance()->plugins().isEmpty()) {
        QMessageBox::warning(&mainwindow,
                             QObject::tr("No plugin loaded"),
                             QObject::tr("Warning : no plugin loaded successfully."));
    }

    //  Handle signals for multiple medInria instances.

    QObject::connect(&application,SIGNAL(messageReceived(const QString&)),
		             &mainwindow,SLOT(onNewInstance(const QString&)));

    application.setActivationWindow(&mainwindow);
    application.setMainWindow(&mainwindow);

    //  Start main loop.

    const int status = application.exec();

    medPluginManager::instance()->uninitialize();

    return status;
}
