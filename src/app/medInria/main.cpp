/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QSurfaceFormat>
#include <QtGui>
#include <QtOpenGL>
#include <QMainWindow>
#include <QNetworkAccessManager>

#ifdef WIN32
#include <QtPlatformHeaders/QWindowsWindowFunctions>
#endif

#include <dtkCoreSupport/dtkGlobal.h>
#include <medApplication.h>
#include <medApplicationContext.h>
#include <medDataIndex.h>
#include <medDataManager.h>
#include <medMainWindow.h>
#include <medPluginManager.h>
#include <medSettingsManager.h>

#include <medSourcesLoader.h>
#include <medDataHub.h>
#include <medVirtualRepresentation.h>
#include <medSourceModelPresenter.h>


#include<medNotifSys.h>
#include<medNotificationPresenter.h>

#include<medNotifWindow.h>

#include<medFirstStart.h>

void forceShow(medMainWindow &mainwindow)
{
    // Idea and code taken from the OpenCOR project, Thanks Allan for the code!

    // Note: to show ourselves, one would normally use activateWindow(), but
    //       depending on the operating system it may or not bring medInria to
    //       the foreground, so... instead we do what follows, depending on the
    //       operating system...

    //#if defined(Q_OS_WIN)
    //    // Retrieve window Id

    //    WId mainWinId = mainwindow.winId();

    //    // Bring main window to the foreground
    //    SetForegroundWindow(mainWinId);

    //    // Show/restore the window, depending on its current state

    //    if (IsIconic(mainWinId))
    //        ShowWindow(mainWinId, SW_RESTORE);
    //    else
    //        ShowWindow(mainWinId, SW_SHOW);

    //    // Note: under Windows, to use activateWindow() will only highlight the
    //    //       application in the taskbar, since under Windows no application
    //    //       should be allowed to bring itself to the foreground when
    //    another
    //    //       application is already in the foreground. Fair enough, but it
    //    //       happens that, here, the user wants medInria to be brought to
    //    the
    //    //       foreground, hence the above code to get the effect we are
    //    after...
    //#else
    mainwindow.activateWindow();
    mainwindow.raise();
    //#endif
}

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    // Setup openGL surface compatible with QVTKOpenGLWidget,
    QSurfaceFormat fmt;
    fmt.setRenderableType(QSurfaceFormat::OpenGL);
    fmt.setVersion(3, 2);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    fmt.setRedBufferSize(8);
    fmt.setGreenBufferSize(8);
    fmt.setBlueBufferSize(8);
    fmt.setDepthBufferSize(8);
    fmt.setAlphaBufferSize(8);
    fmt.setStencilBufferSize(0);
    fmt.setStereo(false);
    fmt.setSamples(0); // we never need multisampling in the context since the FBO can support
                       // multisamples independently
    QSurfaceFormat::setDefaultFormat(fmt);


    medApplication application(argc, argv);

    setlocale(LC_NUMERIC, "C");
    QLocale::setDefault(QLocale("C"));

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addOptions({
        // A boolean option with a single name (--fullscreen)
        {"fullscreen", QCoreApplication::translate("main", "Open application in fullscreen mode")},
        {"no-fullscreen", QCoreApplication::translate("main", "Open application in windowed mode")},
        {"stereo", QCoreApplication::translate("main", "Open application in opengl direct rendering")},
        {"debug",  QCoreApplication::translate("main", "Open application in debug mode")},
        // Options with a value
#ifdef ACTIVATE_WALL_OPTION
       {{"wall", "tracker"
            QCoreApplication::translate("main", "Open application in wall screen "),
            QCoreApplication::translate("main", "tracker=<url>")},
#endif
        {{"v","view"},
            QCoreApplication::translate("main", "Open application with opened <data> "),
            QCoreApplication::translate("main", "data")},
        {{"r", "role"},
            QCoreApplication::translate("main", "Open database with defined role <junior/expert/coordinateur>."),
            QCoreApplication::translate("main", "junior/expert/coordinateur")},
    });

    // Process the actual command line arguments given by the user
    parser.process(application);

    medSettingsManager &manager = medSettingsManager::instance();

    QStringList posargs;
    for (int i=1;i<application.arguments().size();++i)
    {
        const QString arg = application.arguments().at(i);
        if (arg.startsWith("--"))
        {
            QString hostname = parser.value("host");
            manager->setValue("database", "hostname", hostname, false);
        }
        if (parser.isSet("port"))
        {
            manager.setValue("database", "remotedb", remoteDb, false);
            if (parser.isSet("host"))
            {
                QString hostname = parser.value("host");
                manager.setValue("database", "hostname", hostname, false);
            }
            if (parser.isSet("port"))
            {
                int port = parser.value("port").toInt();
                manager.setValue("database", "port", port, false);
            }
            if (parser.isSet("db_prefix_path"))
            {
                QString db_prefix_path = parser.value("db_prefix_path");
                manager.setValue("database", "db_prefix_path", db_prefix_path, false);
            }
        }
        if (parser.isSet("db_prefix_path"))
        {
            QString db_prefix_path = parser.value("db_prefix_path");
            manager->setValue("database", "db_prefix_path", db_prefix_path, false);
        }
    }
    const bool DirectView = parser.isSet("view");
    QStringList viewPaths = parser.values("view");

    int runningMedInria = 0;
    if (DirectView)
    {
    for (QStringList::const_iterator i = viewPaths.constBegin(); i != viewPaths.constEnd(); ++i)
        {
            const QString &message = QString("/open ") + *i;
            runningMedInria = application.sendMessage(message);
        }
    }
    else
    {
        runningMedInria = application.sendMessage("");
    }
    if (runningMedInria)
        return 0;

    QNetworkAccessManager  *qnam = new QNetworkAccessManager(&application);
    medFirstStart firstStart(qnam);
    firstStart.pushPathToCheck(medSourcesLoader::path(), ":/configs/DataSourcesDefault.json", "dataSourceLoader", "", medSourcesLoader::initSourceLoaderCfg);

    firstStart.pushPathToCheck(medMetaDataKeys::path() + "/dicom.json",      ":configs/metaDataDictionaries/dicom.json",      "keysDictionnaries");
    firstStart.pushPathToCheck(medMetaDataKeys::path() + "/itk_mha.json",    ":configs/metaDataDictionaries/itk_mha.json",    "keysDictionnaries");
    firstStart.pushPathToCheck(medMetaDataKeys::path() + "/itk_mhd.json",    ":configs/metaDataDictionaries/itk_mhd.json",    "keysDictionnaries");
    firstStart.pushPathToCheck(medMetaDataKeys::path() + "/itk_nhdr.json",   ":configs/metaDataDictionaries/itk_nhdr.json",   "keysDictionnaries");
    firstStart.pushPathToCheck(medMetaDataKeys::path() + "/itk_nii.gz.json", ":configs/metaDataDictionaries/itk_nii.gz.json", "keysDictionnaries");
    firstStart.pushPathToCheck(medMetaDataKeys::path() + "/itk_nii.json",    ":configs/metaDataDictionaries/itk_nii.json",    "keysDictionnaries");
    firstStart.pushPathToCheck(medMetaDataKeys::path() + "/itk_nrrd.json",   ":configs/metaDataDictionaries/itk_nrrd.json",   "keysDictionnaries");
    firstStart.pushPathToCheck(medMetaDataKeys::path() + "/itk.json",        ":configs/metaDataDictionaries/itk.json",        "keysDictionnaries");

    firstStart.checkAndUpdate();


    medSourcesLoader::instance(&application);     

    medPluginManager::instance()->setVerboseLoading(true);
    medPluginManager::instance()->initialize();
    auto sourceHandler = medSourceHandler::instance(&application);
    auto model = medDataHub::instance(&application);
    auto virtualRepresentation = new medVirtualRepresentation(&application);
    QObject::connect(medSourcesLoader::instance(), SIGNAL(sourceAdded(medAbstractSource *)), sourceHandler, SLOT(addSource(medAbstractSource *)));
    QObject::connect(medSourcesLoader::instance(), SIGNAL(sourceRemoved(medAbstractSource *)), sourceHandler, SLOT(removeSource(medAbstractSource *)));
    QObject::connect(medSourcesLoader::instance(), &medSourcesLoader::defaultWorkingSource, sourceHandler, &medSourceHandler::setDefaultWorkingSource);
    model->setVirtualRepresentation(virtualRepresentation);
    
    medSourcesLoader::instance()->loadFromDisk();
    

    auto notifSys = medNotifSys::instance();
 
    medApplicationContext::instance()->setParent(&application);
    medApplicationContext::instance()->setVirtualRepresentation(virtualRepresentation);
    medApplicationContext::instance()->setDataHub(model);
    medApplicationContext::instance()->setNotifSys(notifSys);
    medApplicationContext::instance()->setSourceHandler(sourceHandler);
    medApplicationContext::instance()->setPluginManager(&medPluginManager::instance());
    medApplicationContext::instance()->setDataManager(&medDataManager::instance());

    notifSys->setOperatingSystemNotification(true);
    notifSys->setOperatingSystemNotification(true);
    notifSys->setOSNotifOnlyNonFocus(true);
    QObject::connect(&application, &QGuiApplication::focusWindowChanged,
                     [=](QWindow *focusWindow)
                     {
                         notifSys->windowOnTop(focusWindow != nullptr);
                     }
    );



    // Use Qt::WA_DeleteOnClose attribute to be sure to always have only one closeEvent.
    medMainWindow *mainwindow = new medMainWindow;

    auto notifBanner = static_cast<medNotificationPaneWidget*>(medNotifSysPresenter(notifSys).buildNotificationWindow());
    notifBanner->setParent(mainwindow);

    mainwindow->setAttribute(Qt::WA_DeleteOnClose, true);

    if (DirectView)
        mainwindow->setStartup(medMainWindow::WorkSpace, viewPaths);

    bool fullScreen = medSettingsManager::instance().value("startup", "fullscreen", false).toBool();
    const bool hasFullScreenArg = parser.isSet("fullscreen");
    const bool hasNoFullScreenArg = parser.isSet("no-fullscreen");
    const bool hasWallArg = false;
#ifdef ACTIVATE_WALL_OPTION
    hasWallArg = parser.isSet("wall");
#endif

    const int conflict = static_cast<int>(hasFullScreenArg) +
                         static_cast<int>(hasNoFullScreenArg) +
                         static_cast<int>(hasWallArg);

    if (conflict > 1)
    {
        dtkWarn() << "Conflicting command line parameters between " "--fullscreen, --no-fullscreen and -wall. Ignoring.";
    }
    else
    {
        if (hasWallArg)
        {
            mainwindow->setWallScreen(true);
            fullScreen = false;
        }

        if (hasFullScreenArg)
            fullScreen = true;

        if (hasNoFullScreenArg)
            fullScreen = false;
    }

    mainwindow->setFullScreen(fullScreen);
#ifdef WIN32
    QWindowsWindowFunctions::setHasBorderInFullScreen(mainwindow->windowHandle(), true);
#endif

    if (parser.isSet("stereo"))
    {
        QGLFormat format;
        format.setAlpha(true);
        format.setDoubleBuffer(true);
        format.setStereo(true);
        format.setDirectRendering(true);
        QGLFormat::setDefaultFormat(format);
    }

    if (medPluginManager::instance().plugins().isEmpty())
    {
        QMessageBox::warning(
            mainwindow, QObject::tr("No plugin loaded"),
            QObject::tr("Warning : no plugin loaded successfully."));
    }

    // Handle file associations open requests that were not handled in the
    // application
    QObject::connect(&application, SIGNAL(messageReceived(const QString &)),
                     mainwindow,
                     SLOT(processNewInstanceMessage(const QString &)));

    application.setMainWindow(mainwindow);

    forceShow(*mainwindow);

    qInfo() << "### Application is running...";

    //  Start main loop.
    const int status = application.exec();

    medPluginManager::instance().uninitialize();

    return status;
}
