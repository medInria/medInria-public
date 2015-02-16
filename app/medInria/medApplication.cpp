/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medApplication.h>

#include <locale.h>

#include <QtGui>

#include <dtkCore/dtkGlobal.h>
#include <dtkLog/dtkLog.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>

#include <medPluginManager.h>

#include <medWorkspaceFactory.h>
#include <medAbstractWorkspace.h>
#include <medFilteringWorkspace.h>
#include <medDiffusionWorkspace.h>
#include <medRegistrationWorkspace.h>
#include <medVisualizationWorkspace.h>
#include <medSegmentationWorkspace.h>

#include <medAbstractDataFactory.h>
#include <medSeedPointAnnotationData.h>

#include <medSettingsWidgetFactory.h>
#include <medSettingsWidget.h>
#include <medStartupSettingsWidget.h>
#include <medDatabaseSettingsWidget.h>

#include <medDataManager.h>
#include <medDatabaseController.h>
#include <medDatabaseNonPersistentController.h>

#include <medDatabaseDataSource.h>
#include <medFileSystemDataSource.h>
#include <medPacsDataSource.h>
#include <medAbstractDataSourceFactory.h>

#include <medDataSourceManager.h>

#include <medMainWindow.h>

#include <medStyleSheetParser.h>

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
    d->mainWindow = NULL;

    // set software information
    this->setApplicationName("medInria");
    qDebug() << "Version:" << MEDINRIA_VERSION;
    this->setApplicationVersion(MEDINRIA_VERSION);
    this->setOrganizationName("inria");
    this->setOrganizationDomain("fr");
    this->setWindowIcon(QIcon(":/medInria.ico"));

    // set style
    medStyleSheetParser parser(dtkReadFile(":/medInria.qss"));
    this->setStyleSheet(parser.result());

    //  Redirect msgs to the logs
    connect(medPluginManager::instance(), SIGNAL(loadError(const QString &)),
                     this, SLOT(redirectErrorMessageToLog(const QString&)) );
    connect(medPluginManager::instance(), SIGNAL(loaded(QString)),
                     this, SLOT(redirectMessageToLog(QString)) );
    connect(this,SIGNAL(messageReceived(const QString&)),
                     this,SLOT(redirectMessageToLog(QString)));

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
    connect(this, SIGNAL(openRequest(medDataIndex)),
            mw, SLOT(open(medDataIndex)));
    connect(this, SIGNAL(openRequest(QString)),
            mw, SLOT(open(QString)));

    //TODO - Fix this, shouldn't be needed.
    QVariant var = QVariant::fromValue((QObject*)d->mainWindow);
    this->setProperty("MainWindow", var);

    // If there are any requests to open files not yet treated, send signal to do so
    foreach(QString openInstruction, d->systemOpenInstructions)
        emit messageReceived(openInstruction);

    d->systemOpenInstructions.clear();
}

void medApplication::redirectMessageToLog(const QString &message)
{
    dtkTrace()<< message;
}

void medApplication::redirectErrorMessageToLog(const QString &message)
{
    dtkError()<< message;
}

void medApplication::redirectMessageToSplash(const QString &message)
{
    emit showMessage(message);
}

void medApplication::initialize()
{
    qRegisterMetaType<QUuid>("QUuid");

    //WARN - medDataManager::initialize(); must be called before establishing connection with any dataBase.
    medDataManager::initialize();
    //  Setting up database connection

    //WARN - medDatabaseController::createConnection() must be called before creation of dataBaseDataSource;
    if (!medDatabaseController::instance()->createConnection())
        qDebug() << "Unable to create a connection to the database";


    // Register dataSource types from app
    if (!medPacsDataSource::registered())
        dtkWarn() << "Unable to register medPacsDataSource type";
    else
        qDebug() << "medPacsDataSource type Registered";
    if (!medFileSystemDataSource::registered())
        dtkWarn() << "Unable to register medFileSystemDataSource type";
    else
        qDebug() << "medFileSystemDataSource type Registered";
    if (!medDatabaseDataSource::registered())
        dtkWarn() << "Unable to register medDatabaseDataSource type";
    else
        qDebug() << "medDatabaseDataSource type Registered";

    connect(medDataSourceManager::instance(), SIGNAL(openRequest(medDataIndex)),
            this, SIGNAL(openRequest(medDataIndex)));

    // Registering different workspaces
    medWorkspaceFactory * viewerWSpaceFactory = medWorkspaceFactory::instance();
    viewerWSpaceFactory->registerWorkspace<medVisualizationWorkspace>();
    viewerWSpaceFactory->registerWorkspace<medRegistrationWorkspace>();
    viewerWSpaceFactory->registerWorkspace<medDiffusionWorkspace>();
    viewerWSpaceFactory->registerWorkspace<medFilteringWorkspace>();
    viewerWSpaceFactory->registerWorkspace<medSegmentationWorkspace>();

    //Register settingsWidgets
    medSettingsWidgetFactory * settingsWidgetFactory = medSettingsWidgetFactory::instance();
    settingsWidgetFactory->registerSettingsWidget<medStartupSettingsWidget>();
    settingsWidgetFactory->registerSettingsWidget<medDatabaseSettingsWidget>();

    //Register annotations
    medAbstractDataFactory * datafactory = medAbstractDataFactory::instance();
    datafactory->registerDataType<medSeedPointAnnotationData>();
}
