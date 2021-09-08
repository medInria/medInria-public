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

#ifdef WIN32
#include <QtPlatformHeaders/QWindowsWindowFunctions>
#endif

#include <dtkCoreSupport/dtkGlobal.h>
#include <medApplication.h>
#include <medDataIndex.h>
#include <medDataManager.h>
#include <medMainWindow.h>
#include <medPluginManager.h>
#include <medSettingsManager.h>
#include <medSplashScreen.h>
#include <medStorage.h>

#include <medSourcesLoader.h>
#include <medDataModel.h>

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

    // this needs to be done before creating the QApplication object, as per the
    // Qt doc, otherwise there are some edge cases where the style is not fully
    // applied
    // QApplication::setStyle("plastique");
    medApplication application(argc, argv);
    medSplashScreen splash(QPixmap(":/pixmaps/medInria-logo-homepage.png"));

    setlocale(LC_NUMERIC, "C");
    QLocale::setDefault(QLocale("C"));

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addOptions({
        // A boolean option with a single name (--fullscreen)
        {"fullscreen",
            QCoreApplication::translate("main", "Open application in fullscreen mode")},
        {"no-fullscreen",
            QCoreApplication::translate("main", "Open application in windowed mode")},
        {"stereo",
            QCoreApplication::translate("main", "Open application in opengl direct rendering")},
       {"debug", 
            QCoreApplication::translate("main", "Open application in debug mode")},
        {{"remotedb", "psql"},
            QCoreApplication::translate("main", "Connect application to a remote database controller (psql)")},
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
        {{"c", "center"},
            QCoreApplication::translate("main", "Open database for a specific center <center>."),
            QCoreApplication::translate("main", "center")},
        {"host",
            QCoreApplication::translate("main", "database server host or socket directory (default: \"local socket\"). \nThis parameter is taken into account only when remotedb parameter is defined"),
            QCoreApplication::translate("main", "HOSTNAME")},
        {{"port","p"},
            QCoreApplication::translate("main", "database server port (default: \"5432\"). \nThis parameter is taken into account only when remotedb parameter is defined"),
            QCoreApplication::translate("main", "PORT")},
       {"db_prefix_path",
            QCoreApplication::translate("main", "set database prefix path. \nThis parameter is taken into account only when remotedb parameter is defined"),
            QCoreApplication::translate("main", "<db_prefix_path>")},
    });

        // Process the actual command line arguments given by the user
        parser.process(application);

        // Do not show the splash screen in debug builds because it hogs the
        // foreground, hiding all other windows. This makes debugging the startup
        // operations difficult.

#if !defined(_DEBUG)
        bool show_splash = true;
#else
    bool show_splash = false;
#endif

        if (parser.isSet("center"))
        {
            int center = parser.value("center").toInt();
            medSettingsManager *mnger = medSettingsManager::instance();
            mnger->setValue("database", "center", center, false);
        }

        const bool remoteDb = parser.isSet("remotedb");
        if (remoteDb)
        {
            medSettingsManager *mnger = medSettingsManager::instance();
            mnger->setValue("database", "remotedb", remoteDb, false);
            if (parser.isSet("host"))
            {
                QString hostname = parser.value("host");
                mnger->setValue("database", "hostname", hostname, false);
            }
            if (parser.isSet("port"))
            {
                int port = parser.value("port").toInt();
                mnger->setValue("database", "port", port, false);
            }
            if (parser.isSet("db_prefix_path"))
            {
                QString db_prefix_path = parser.value("db_prefix_path");
                mnger->setValue("database", "db_prefix_path", db_prefix_path, false);
            }
        }
        const bool DirectView = parser.isSet("view");
        QStringList viewPaths = parser.values("view");

        // const bool DirectView =
        // dtkApplicationArgumentsContain(&application,"--view") ||
        // posargs.size()!=0;
        int runningMedInria = 0;
        if (DirectView)
        {
            show_splash = false;
            for (QStringList::const_iterator i = viewPaths.constBegin();
                 i != viewPaths.constEnd(); ++i)
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

        if (show_splash)
        {
            QObject::connect(medPluginManager::instance(),
                             SIGNAL(loaded(QString)), &application,
                             SLOT(redirectMessageToSplash(QString)));
            QObject::connect(&application, SIGNAL(showMessage(const QString &)),
                             &splash, SLOT(showMessage(const QString &)));
            splash.show();
            splash.showMessage("Loading plugins...");
        }

        medDataManager::instance()->setDatabaseLocation();
        medPluginManager::instance()->setVerboseLoading(true);
        medPluginManager::instance()->initialize();

        // [LIKE PORKY]
//        QString foo;
//
        //medDBSourcesLoader::instance()->createCnx(foo, "medSQLite");
        //medDBSourcesLoader::instance()->renameSource(foo, "Legacy medInria 3 DB");

        auto testModel = new medDataModel();
        QObject::connect(medDBSourcesLoader::instance(), SIGNAL(sourceAdded(medAbstractSource *)), testModel, SLOT(addSource(medAbstractSource *)));
        medDBSourcesLoader::instance()->loadFromDisk();

        auto testWindow = new QMainWindow();
        auto treeView = new QTreeView();
        auto tableView = new QTableView();

        treeView->setModel(testModel->getModel(""));
        tableView->setModel(testModel->getModel(""));
        bool c0Ok = QObject::connect(treeView, SIGNAL(pressed(const QModelIndex &)), tableView, SIGNAL(pressed(const QModelIndex &)));
        bool c1Ok = QObject::connect(treeView, SIGNAL(pressed(const QModelIndex &)), testModel->getModel(""), SLOT(itemPressed(QModelIndex const &)));
        bool c2Ok = QObject::connect(tableView, SIGNAL(pressed(const QModelIndex &)), testModel->getModel(""), SLOT(itemPressed(QModelIndex const &)));
        QWidget *w = new QWidget;
        QVBoxLayout *vLayout = new QVBoxLayout;
        vLayout->addWidget(treeView);
        vLayout->addWidget(tableView);
        w->setLayout(vLayout);

        testWindow->setCentralWidget(w);
        testWindow->show();


        // Use Qt::WA_DeleteOnClose attribute to be sure to always have only one
        // closeEvent.
        medMainWindow *mainwindow = new medMainWindow;
        mainwindow->setAttribute(Qt::WA_DeleteOnClose, true);

        if (DirectView)
            mainwindow->setStartup(medMainWindow::WorkSpace, viewPaths);

        bool fullScreen = medSettingsManager::instance()
                              ->value("startup", "fullscreen", false)
                              .toBool();
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
            dtkWarn() << "Conflicting command line parameters between "
                         "--fullscreen, --no-fullscreen and -wall. Ignoring.";
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
        QWindowsWindowFunctions::setHasBorderInFullScreen(
            mainwindow->windowHandle(), true);
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

        if (show_splash)
            splash.finish(mainwindow);

        if (medPluginManager::instance()->plugins().isEmpty())
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

        medPluginManager::instance()->uninitialize();

        return status;
}
