/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.

 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.

=========================================================================*/

#include "medApplication.h"

#include <medAbstractDataFactory.h>
#include <medCore.h>
#include <medDataHub.h>
#include <medDataManager.h>
#include <medDiffusionWorkspace.h>
#include <medFilteringWorkspace.h>
#include <medNewLogger.h>
#include <medMainWindow.h>
#include <medPluginManager.h>
#include <medSeedPointAnnotationData.h>
#include <medSettingsManager.h>
#include <medStyleSheetParser.h>
#include <medVisualizationWorkspace.h>
#include <medWorkspaceFactory.h>

class medApplicationPrivate
{
public:
    medMainWindow *mainWindow;
    QStringList systemOpenInstructions;
    QSplashScreen *splashScreen;
    QtLocalPeer *peer;
    QWidget *actWin;

    bool listenClick;
};

// /////////////////////////////////////////////////////////////////
// medApplication
// /////////////////////////////////////////////////////////////////

medApplication::medApplication(int & argc, char**argv) :
    //QtSingleApplication(argc,argv),
    QApplication(argc, argv),
    d(new medApplicationPrivate)
{
    // Do not show the splash screen in debug builds because it hogs the
    // foreground, hiding all other windows. This makes debugging the startup
    // operations difficult.
#ifdef NDEBUG
    initializeSplashScreen();
#endif

    d->listenClick = false;

    d->actWin = 0;
    d->peer = new QtLocalPeer(this, "");
    connect(d->peer, SIGNAL(messageReceived(const QString&)), SIGNAL(messageReceived(const QString&)));

    d->mainWindow = nullptr;

    this->setApplicationName("medInria");
    this->setApplicationVersion(MEDINRIA_VERSION);
    this->setOrganizationName("inria");
    this->setOrganizationDomain("fr");
    this->setWindowIcon(QIcon(":/medInria.ico"));

    medNewLogger::initialize(&medNewLogger::mainInstance());

    qInfo() << "####################################";
    qInfo() << "Version: "    << MEDINRIA_VERSION;
    qInfo() << "Build Date: " << MEDINRIA_BUILD_DATE;

    this->initialize();
}

medApplication::~medApplication(void)
{
    delete d;
    d = nullptr;
}

bool medApplication::event(QEvent *event)
{
    auto type = event->type();
    switch (type)
    {
        // Handle file system open requests, but only if the main window has been created and set
        case QEvent::FileOpen:
        {
            if (d->mainWindow)
                emit messageReceived(QString("/open ") + static_cast<QFileOpenEvent *>(event)->file());
            else
                d->systemOpenInstructions.append(QString("/open ") + static_cast<QFileOpenEvent *>(event)->file());

            return true;
        }
        default:
        {
            return QApplication::event(event);
        }
    }
}

bool medApplication::notify(QObject * receiver, QEvent * e)
{
    if (d->listenClick && e->type() == QEvent::MouseButtonRelease)
    {
        auto *mouseEvent = static_cast<QMouseEvent*>(e);
        emit mouseGlobalClick(mouseEvent->globalPos());
    }

    return QApplication::notify(receiver, e);
}

void medApplication::setMainWindow(medMainWindow *mw)
{
    d->mainWindow = mw;

    QVariant var = QVariant::fromValue((QObject*)d->mainWindow);
    this->setProperty("MainWindow",var);
    d->systemOpenInstructions.clear();

#ifdef NDEBUG
    // Wait until the app is displayed to close itself
    d->splashScreen->finish(d->mainWindow);
#endif
}

/*!
    Tries to send the text \a message to the currently running
    instance. The QtSingleApplication object in the running instance
    will emit the messageReceived() signal when it receives the
    message.

    This function returns true if the message has been sent to, and
    processed by, the current instance. If there is no instance
    currently running, or if the running instance fails to process the
    message within \a timeout milliseconds, this function return false.

    \sa isRunning(), messageReceived()
*/
bool medApplication::sendMessage(const QString &message, int timeout)
{
    return d->peer->sendMessage(message, timeout);
}

void medApplication::open(const medDataIndex & index)
{
    d->mainWindow->open(index);
}

void medApplication::open(QString path)
{
    d->mainWindow->open(path);
}

void medApplication::listenClick(bool listen)
{
    d->listenClick = listen;
}

void medApplication::initialize()
{
    initializeThemes();

    qRegisterMetaType<medDataIndex>("medDataIndex");

    // Registering different workspaces
    medWorkspaceFactory * viewerWSpaceFactory = medWorkspaceFactory::instance();
    viewerWSpaceFactory->registerWorkspace<medVisualizationWorkspace>();
    viewerWSpaceFactory->registerWorkspace<medDiffusionWorkspace>();
    viewerWSpaceFactory->registerWorkspace<medFilteringWorkspace>();

    //Register annotations
    medAbstractDataFactory * datafactory = medAbstractDataFactory::instance();
    datafactory->registerDataType<medSeedPointAnnotationData>();

    // process layer:
    QString pluginsPath = getenv("MEDINRIA_PLUGINS_DIR");
    QString defaultPath;
    QDir plugins_dir;
#ifdef Q_OS_MAC
    plugins_dir = qApp->applicationDirPath() + "/../PlugIns";
#elif defined(Q_OS_WIN)
    plugins_dir = qApp->applicationDirPath() + "/plugins";
#else
    plugins_dir = qApp->applicationDirPath() + "/plugins";
#endif
    defaultPath = plugins_dir.absolutePath();

    if ( !pluginsPath.isEmpty() )
        medCore::pluginManager::initialize(pluginsPath);
    else
        medCore::pluginManager::initialize(defaultPath);
}

/**
 * @brief Get back the previous screen used to display the application
 * 
 * @return QScreen 
 */
QScreen* medApplication::getPreviousScreen()
{
    medSettingsManager *manager = medSettingsManager::instance();
    int currentScreen = 0;
    QVariant currentScreenQV = manager->value("medMainWindow", "currentScreen");
    if (!currentScreenQV.isNull())
    {
        currentScreen = currentScreenQV.toInt();

        // If the previous used screen has been removed, initialization
        if (currentScreen >= QApplication::desktop()->screenCount())
        {
            currentScreen = 0;
        }
    }
    return screens().at(currentScreen);
}

/**
 * @brief Set the Qt splash screen to the application logo.
 * 
 */
void medApplication::initializeSplashScreen()
{
    d->splashScreen = new QSplashScreen(getPreviousScreen(), QPixmap(":/pixmaps/medInria-splash.png"),
        Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
    d->splashScreen->setAttribute(Qt::WA_DeleteOnClose, true);
    d->splashScreen->show();
    this->processEvents();
}

/**
 * @brief Parse the theme file chosen by user in settings.
 * 
 */
void medApplication::initializeThemes()
{
    QApplication::setStyle(QStyleFactory::create("fusion"));

    int themeIndex = medSettingsManager::instance().value("startup","theme").toInt();
    QString qssFile;
    switch (themeIndex)
    {
    case 0:
    default:
        qssFile = ":/dark.qss";
        QIcon::setThemeName(QStringLiteral("dark"));
        break;
    case 1:
        qssFile = ":/grey.qss";
        QIcon::setThemeName(QStringLiteral("light"));
        break;
    case 2:
        qssFile = ":/light.qss";
        QIcon::setThemeName(QStringLiteral("light"));
        break;
    }
    medStyleSheetParser parser(dtkReadFile(qssFile));
    this->setStyleSheet(parser.result());

    this->setWindowIcon(QIcon(":medInria.png"));

    // Unblur icons for instance on retina screens
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps); 
}
