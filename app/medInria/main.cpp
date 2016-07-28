/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <boost/iostreams/tee.hpp>
#include <boost/iostreams/stream.hpp>

#include <QtGui>
#include <QtOpenGL>
#include <QtDebug>
#include <qdebug.h>
#include <medMainWindow.h>
#include <medApplication.h>
#include <medSplashScreen.h>

#include <dtkCore>
#include <dtkLog/dtkLog.h>

#include <medPluginManager.h>
#include <medDataIndex.h>
#include <medDatabaseController.h>
#include <medSettingsManager.h>
#include <medStorage.h>

void forceShow(medMainWindow& mainwindow )
{
    //Idea and code taken from the OpenCOR project, Thanks Allan for the code!


    // Note: to show ourselves, one would normally use activateWindow(), but
    //       depending on the operating system it may or not bring medInria to
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
    //       happens that, here, the user wants medInria to be brought to the
    //       foreground, hence the above code to get the effect we are after...
#else
    mainwindow.activateWindow();
    mainwindow.raise();
#endif
}


void myMessageOutput(QtMsgType type, const char *msg)
{
    switch (type)
    {
    case QtDebugMsg:
        dtkDebug()<<msg;
        break;
    case QtWarningMsg:
        dtkWarn()<<msg;
        break;
    case QtCriticalMsg:
        dtkError()<<msg;
        break;
    case QtFatalMsg:
        dtkFatal()<<msg;
        abort();
    }
}

int main(int argc,char* argv[]) {

    qRegisterMetaType<medDataIndex>("medDataIndex");

    // this needs to be done before creating the QApplication object, as per the
    // Qt doc, otherwise there are some edge cases where the style is not fully applied
    QApplication::setStyle("plastique");
    medApplication application(argc,argv);

    // Copy std::cout and std::cerr in log file
    typedef boost::iostreams::tee_device<std::ostream, std::ofstream> TeeDevice;
    typedef boost::iostreams::stream<TeeDevice> TeeStream;

    std::ofstream logFile(dtkLogPath(&application).toLocal8Bit().data(), std::ios::app);

    std::ostream tmp(std::cout.rdbuf());
    TeeDevice outputDevice(tmp, logFile);
    TeeStream logger(outputDevice);
    std::cout.rdbuf(logger.rdbuf());

    std::ostream tmpErr(std::cerr.rdbuf());
    TeeDevice outputDeviceErr(tmpErr, logFile);
    TeeStream loggerErr(outputDeviceErr);
    std::cerr.rdbuf(loggerErr.rdbuf());

    // Redirect Qt messages into dtkMsg
    qInstallMsgHandler(myMessageOutput);

    // Copy dtkMsg into log file
    // TRACE >> DEBUG >> INFO >> WARN >> ERROR >> FATAL
    // Note that these levels form a hierarchy.
    // If dtk’s logging level is set to TRACE, all logs will be visible,
    // unless set to WARN, only WARN, ERROR and FATAL log messages will be visible.
    dtkLogger::instance().setLevel(dtkLog::Debug);
    dtkLogger::instance().attachFile(dtkLogPath(&application));
    dtkLogger::instance().attachConsole();

    qDebug() << "####################################";
    qDebug() << "Version: "    << MEDINRIA_VERSION;
    qDebug() << "Build Date: " << MEDINRIA_BUILD_DATE;

    medSplashScreen splash(QPixmap(":music_logo.png"));
    setlocale(LC_NUMERIC, "C");
    QLocale::setDefault(QLocale("C"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    if (dtkApplicationArgumentsContain(&application, "-h") || dtkApplicationArgumentsContain(&application, "--help")) {
        qDebug() << "Usage: MUSIC [--fullscreen|--no-fullscreen] [--stereo] "
        #ifdef ACTIVATE_WALL_OPTION
        "[--wall] [--tracker=URL] "
        #endif
        "[--view] [files]]";
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
            const QStringList options = (QStringList()
                    << "--fullscreen"
                    << "--no-fullscreen"
                    << "--wall"
                    << "--tracker"
                    << "--stereo"
                    << "--view");
            for (QStringList::const_iterator opt=options.constBegin();opt!=options.constEnd();++opt)
                if (arg.startsWith(*opt))
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

    if (show_splash) {
        QObject::connect(medPluginManager::instance(),SIGNAL(loadError(const QString&)),
                         &application,SLOT(redirectMessageToSplash(const QString&)) );
        QObject::connect(medPluginManager::instance(),SIGNAL(loaded(QString)),
                         &application,SLOT(redirectMessageToSplash(QString)) );
        QObject::connect(&application,SIGNAL(showMessage(const QString&)),
                         &splash,SLOT(showMessage(const QString&)) );
        splash.show();
        splash.showMessage("Loading plugins...");
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

    //Use Qt::WA_DeleteOnClose attribute to be sure to always have only one closeEvent.
    medMainWindow *mainwindow = new medMainWindow;
    mainwindow->setAttribute(Qt::WA_DeleteOnClose, true);

    if (DirectView)
        mainwindow->setStartup(medMainWindow::WorkSpace,posargs);

    bool fullScreen = medSettingsManager::instance()->value("startup", "fullscreen", false).toBool();

    const bool hasFullScreenArg   = application.arguments().contains("--fullscreen");
    const bool hasNoFullScreenArg = application.arguments().contains("--no-fullscreen");
    const bool hasWallArg         = application.arguments().contains("--wall");

    const int conflict = static_cast<int>(hasFullScreenArg)+static_cast<int>(hasNoFullScreenArg)+ static_cast<int>(hasWallArg);

    if (conflict>1)
        dtkWarn() << "Conflicting command line parameters between --fullscreen, --no-fullscreen and -wall. Ignoring.";
    else {
        if (hasWallArg) {
            mainwindow->setWallScreen(true);
            fullScreen = false;
        }

        if (hasFullScreenArg)
            fullScreen = true;

        if (hasNoFullScreenArg)
            fullScreen = false;
    }

    mainwindow->setFullScreen(fullScreen);


    if(application.arguments().contains("--stereo")) {
       QGLFormat format;
       format.setAlpha(true);
       format.setDoubleBuffer(true);
       format.setStereo(true);
       format.setDirectRendering(true);
       QGLFormat::setDefaultFormat(format);
    }

    if (show_splash)
        splash.finish(mainwindow);

    if (medPluginManager::instance()->plugins().isEmpty()) {
        QMessageBox::warning(mainwindow,
                             QObject::tr("No plugin loaded"),
                             QObject::tr("Warning : no plugin loaded successfully."));
    }

    // Handle file associations open requests that were not handled in the application
    QObject::connect(&application,SIGNAL(messageReceived(const QString&)),
                     mainwindow,SLOT(processNewInstanceMessage(const QString&)));

    application.setActivationWindow(mainwindow);
    application.setMainWindow(mainwindow);

    forceShow(*mainwindow);

    //  Start main loop.
    const int status = application.exec();

    medPluginManager::instance()->uninitialize();

    return status;
}
