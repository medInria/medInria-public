/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QtGui>
#include <QtOpenGL>
#include <QSurfaceFormat>

#ifdef WIN32
#include <QtPlatformHeaders/QWindowsWindowFunctions>
#endif

#ifdef USE_PYTHON
#include <pyncpp.h>
#endif

#include <medMainWindow.h>
#include <medApplication.h>
#include <medPluginManager.h>
#include <medDataIndex.h>
#include <medDataManager.h>
#include <medSettingsManager.h>
#include <medStorage.h>

#include <dtkCoreSupport/dtkGlobal.h>


void forceShow(medMainWindow& mainwindow )
{
    //Idea and code taken from the OpenCOR project, Thanks Allan for the code!


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
//    //       should be allowed to bring itself to the foreground when another
//    //       application is already in the foreground. Fair enough, but it
//    //       happens that, here, the user wants medInria to be brought to the
//    //       foreground, hence the above code to get the effect we are after...
//#else
    mainwindow.activateWindow();
    mainwindow.raise();
//#endif
}

int main(int argc,char* argv[])
{
    // Setup openGL surface compatible with QVTKOpenGLWidget,
    // required by medVtkView
    QSurfaceFormat fmt;
    fmt.setRenderableType(QSurfaceFormat::OpenGL);
    fmt.setVersion(3, 2);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    fmt.setRedBufferSize(1);
    fmt.setGreenBufferSize(1);
    fmt.setBlueBufferSize(1);
    fmt.setDepthBufferSize(1);
    fmt.setStencilBufferSize(0);
    fmt.setAlphaBufferSize(1);
    fmt.setStereo(false);
    fmt.setSamples(0);
    QSurfaceFormat::setDefaultFormat(fmt);

    medApplication application(argc,argv);

    setlocale(LC_NUMERIC, "C");
    QLocale::setDefault(QLocale("C"));

    if (dtkApplicationArgumentsContain(&application, "-h") || dtkApplicationArgumentsContain(&application, "--help"))
    {
        qDebug() << "Usage: "
                 << QFileInfo(argv[0]).baseName().toStdString().c_str()
                 << "[--fullscreen|--no-fullscreen] "
                 << "[--stereo] "
                 << "[--debug] "
            #ifdef ACTIVATE_WALL_OPTION
                 << "[[--wall] [--tracker=URL]] "
            #endif
                 << "[[--view] [files]]";
        return 1;
    }

    QStringList posargs;
    for (int i=1;i<application.arguments().size();++i)
    {
        const QString arg = application.arguments().at(i);
        if (arg.startsWith("--"))
        {
            bool valid_option = false;
            const QStringList options =
                    (QStringList()
                     << "--fullscreen"
                     << "--no-fullscreen"
                     << "--wall"
                     << "--tracker"
                     << "--stereo"
                     << "--view"
                     << "--debug");
            for (QStringList::const_iterator opt=options.constBegin();opt!=options.constEnd();++opt)
            {
                if (arg.startsWith(*opt))
                {
                    valid_option = true;
                }
            }
            if (!valid_option)
            {
                qDebug() << "Ignoring unknown option " << arg;
            }
            continue;
        }
        posargs.append(arg);
    }

    const bool DirectView = dtkApplicationArgumentsContain(&application,"--view") || posargs.size()!=0;

    int runningMedInria = 0;
    if (DirectView)
    {
        for (QStringList::const_iterator i=posargs.constBegin();i!=posargs.constEnd();++i) {
            const QString& message = QString("/open ")+*i;
            runningMedInria = application.sendMessage(message);
        }
    } else {
        runningMedInria = application.sendMessage("");
    }

    if (runningMedInria)
        return 0;

    medDataManager::instance().setDatabaseLocation();

#ifdef USE_PYTHON
    pyncpp::Manager pythonManager;
    QString pythonHomePath = PYTHON_HOME;
    QDir applicationPath = qApp->applicationDirPath();
    QDir pythonHome = applicationPath;
    QDir pythonPluginPath = pythonHome;
    bool pythonHomeFound = false;
    bool pythonPluginsFound = false;
    QString pythonErrorMessage;

    if (pythonHome.cd(PYTHON_HOME))
    {
        pythonHomeFound = true;
        pythonPluginsFound = pythonPluginPath.cd(PYTHON_PLUGIN_PATH);
    }
    else
    {
        if (pythonHome.cd(BUILD_PYTHON_HOME))
        {
            pythonHomeFound = true;
            pythonPluginsFound = pythonPluginPath.cd(BUILD_PYTHON_PLUGIN_PATH);
        }
    }

    if (!pythonHomeFound)
    {
        pythonErrorMessage = "The embedded Python could not be found ";
    }
    else
    {
        if(!pythonManager.initialize(qUtf8Printable(pythonHome.absolutePath())))
        {
            pythonErrorMessage = "Initialization of the embedded Python failed.";
        }
        else
        {
            if (pythonPluginsFound)
            {
                try
                {
                    pyncpp::Module sysModule = pyncpp::Module::import("sys");
                    pyncpp::Object sysPath = sysModule.attribute("path");
                    sysPath.append(pyncpp::Object(pythonPluginPath.absolutePath()));
                    qInfo() << "Added Python plugin path: " << pythonPluginPath.path();

#ifdef Q_OS_WIN
                    QDir sitePackages = pythonHome;

                    if (sitePackages.cd("lib/site-packages"))
                    {
                        sysPath.append(pyncpp::Object(sitePackages.absolutePath()));
                    }
                    else
                    {
                        pythonErrorMessage = "Cannot find site directory.";
                    }

                    pyncpp::Module osModule = pyncpp::Module::import("os");
                    osModule.callMethod("add_dll_directory", applicationPath.absolutePath());
                    qInfo() << "Added Python DLL path: " << applicationPath.path();

                    QDir pluginsLegacyPath = applicationPath;

                    if (pluginsLegacyPath.cd(PLUGINS_LEGACY_PATH))
                    {
                        osModule.callMethod("add_dll_directory", pluginsLegacyPath.absolutePath());
                        qInfo() << "Added Python DLL path: " << pluginsLegacyPath.path();
                    }
                    else
                    {
                        pythonErrorMessage = "Could not find legacy plugins path.";
                    }
#endif // Q_OS_WIN
                }
                catch (pyncpp::Exception& e)
                {
                    pythonErrorMessage = e.what();
                }
            }
        }
    }
#endif // USE_PYTHON

    medPluginManager::instance().setVerboseLoading(true);
    medPluginManager::instance().initialize();

    //Use Qt::WA_DeleteOnClose attribute to be sure to always have only one closeEvent.
    medMainWindow *mainwindow = new medMainWindow;
    mainwindow->setAttribute(Qt::WA_DeleteOnClose, true);

    if (DirectView)
        mainwindow->setStartup(medMainWindow::WorkSpace,posargs);

    bool fullScreen = medSettingsManager::instance().value("startup", "fullscreen", false).toBool();
    
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
#ifdef WIN32
    QWindowsWindowFunctions::setHasBorderInFullScreen(mainwindow->windowHandle(), true);
#endif

    if(application.arguments().contains("--stereo")) {
       QGLFormat format;
       format.setAlpha(true);
       format.setDoubleBuffer(true);
       format.setStereo(true);
       format.setDirectRendering(true);
       QGLFormat::setDefaultFormat(format);
    }

#ifdef USE_PYTHON
    if(!pythonErrorMessage.isEmpty())
    {
        qWarning() << "(Python error) " << pythonErrorMessage;
        QMessageBox::warning(mainwindow, "Python error", pythonErrorMessage);
    }
#endif

    if (medPluginManager::instance().plugins().isEmpty()) {
        QMessageBox::warning(mainwindow,
                             QObject::tr("No plugin loaded"),
                             QObject::tr("Warning : no plugin loaded successfully."));
    }

    // Handle file associations open requests that were not handled in the application
    QObject::connect(&application,SIGNAL(messageReceived(const QString&)),
                     mainwindow,SLOT(processNewInstanceMessage(const QString&)));

    application.setMainWindow(mainwindow);

    forceShow(*mainwindow);

    qInfo() << "### Application is running...";

    //  Start main loop.
    const int status = application.exec();

    medPluginManager::instance().uninitialize();

    return status;
}
