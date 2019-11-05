/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medApplication.h"

#include <medAbstractDataFactory.h>
#include <medCore.h>
#include <medDatabaseSettingsWidget.h>
#include <medDataManager.h>
#include <medDatabaseController.h>
#include <medDiffusionWorkspace.h>
#include <medFilteringWorkspace.h>
#include <medGenericWorkspace.h>
#include <medLogger.h>
#include <medMainWindow.h>
#include <medPluginManager.h>
#include <medSeedPointAnnotationData.h>
#include <medSettingsManager.h>
#include <medSettingsWidgetFactory.h>
#include <medStartupSettingsWidget.h>
#include <medStyleSheetParser.h>
#include <medVisualizationWorkspace.h>
#include <medWorkspaceFactory.h>

class medApplicationPrivate
{
public:
    medMainWindow *mainWindow;
    QStringList systemOpenInstructions;
};

// /////////////////////////////////////////////////////////////////
// medApplication
// /////////////////////////////////////////////////////////////////

medApplication::medApplication(int & argc, char**argv) :
    QtSingleApplication(argc,argv),
    d(new medApplicationPrivate)
{
    d->mainWindow = nullptr;

    this->setApplicationName("medInria");
    this->setApplicationVersion(MEDINRIA_VERSION);
    this->setOrganizationName("inria");
    this->setOrganizationDomain("fr");
    this->setWindowIcon(QIcon(":/medInria.ico"));

    medLogger::initialize();

    dtkInfo() << "####################################";
    dtkInfo() << "Version: "    << MEDINRIA_VERSION;
    dtkInfo() << "Build Date: " << MEDINRIA_BUILD_DATE;

    QApplication::setStyle(QStyleFactory::create("fusion"));
    medStyleSheetParser parser(dtkReadFile(":/medInria.qss"));
    this->setStyleSheet(parser.result());

    this->initialize();
}

medApplication::~medApplication(void)
{
    delete d;
    d = NULL;
}

bool medApplication::event(QEvent *event)
{
    switch (event->type())
    {
        // Handle file system open requests, but only if the main window has been created and set
        case QEvent::FileOpen:
            if (d->mainWindow)
                emit messageReceived(QString("/open ") + static_cast<QFileOpenEvent *>(event)->file());
            else
                d->systemOpenInstructions.append(QString("/open ") + static_cast<QFileOpenEvent *>(event)->file());

            return true;
        default:
            return QtSingleApplication::event(event);
    }
}

void medApplication::setMainWindow(medMainWindow *mw)
{
    d->mainWindow = mw;

    QVariant var = QVariant::fromValue((QObject*)d->mainWindow);
    this->setProperty("MainWindow",var);
    d->systemOpenInstructions.clear();
}

void medApplication::redirectMessageToSplash(const QString &message)
{
    emit showMessage(message);
}

void medApplication::open(const medDataIndex & index)
{
    d->mainWindow->open(index);
}

void medApplication::open(QString path)
{
    d->mainWindow->open(path);
}

void medApplication::initialize()
{
    qRegisterMetaType<medDataIndex>("medDataIndex");

    //  Setting up database connection
    if ( ! medDatabaseController::instance()->createConnection())
    {
        qDebug() << "Unable to create a connection to the database";
    }

    medDataManager::initialize();

    // Registering different workspaces
    medWorkspaceFactory * viewerWSpaceFactory = medWorkspaceFactory::instance();
    viewerWSpaceFactory->registerWorkspace<medVisualizationWorkspace>();
    viewerWSpaceFactory->registerWorkspace<medDiffusionWorkspace>();
    viewerWSpaceFactory->registerWorkspace<medFilteringWorkspace>();

    bool genericWorkpace = medSettingsManager::instance()->value("startup", "genericWorkspace", false).toBool();
    viewerWSpaceFactory->registerWorkspace<medGenericWorkspace>(genericWorkpace);

    //Register settingsWidgets
    medSettingsWidgetFactory* settingsWidgetFactory = medSettingsWidgetFactory::instance();
    settingsWidgetFactory->registerSettingsWidget<medStartupSettingsWidget>();
    settingsWidgetFactory->registerSettingsWidget<medDatabaseSettingsWidget>();

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
