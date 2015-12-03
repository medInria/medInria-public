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
#include <QList>

#include <dtkCoreSupport/dtkGlobal.h>
#include <dtkLog/dtkLog.h>
#include <dtkCoreSupport/dtkAbstractDataFactory.h>
#include <dtkCoreSupport/dtkAbstractData.h>

#include <medPluginManager.h>


#include <medWorkspaceFactory.h>
#include <medAbstractWorkspaceLegacy.h>
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

#include <medMainWindow.h>

#include <medStyleSheetParser.h>
#include <medGenericWorkspace.h>

#include <medCore.h>
#include <medAbstractArithmeticOperationProcess.h>

#include <medJobManager.h>

//#include <medGuiLayer.h>
//#include <medAbstractArea.h>

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

    this->setApplicationName("medInria");
    dtkDebug() << "Version:" << MEDINRIA_VERSION;
    this->setApplicationVersion(MEDINRIA_VERSION);
    this->setOrganizationName("inria");
    this->setOrganizationDomain("fr");
    this->setWindowIcon(QIcon(":/medInria.ico"));

    QApplication::setStyle(QStyleFactory::create("fusion"));
    medStyleSheetParser parser(dtkReadFile(":/medInria.qss"));
    this->setStyleSheet(parser.result());

    //  Redirect msgs to the logs

    QObject::connect(medPluginManager::instance(), SIGNAL(loadError(const QString &)),
                     this, SLOT(redirectErrorMessageToLog(const QString&)) );
    QObject::connect(medPluginManager::instance(), SIGNAL(loaded(QString)),
                     this, SLOT(redirectMessageToLog(QString)) );

    QObject::connect(this,SIGNAL(messageReceived(const QString&)),
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
        if (!d->mainWindow)
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
    qRegisterMetaType<QUuid>("QUuid");
    //    qRegisterMetaType<medAbstractImageData>("medAbstractImageData");

    //  Setting up database connection
    if ( ! medDatabaseController::instance()->createConnection())
        dtkDebug() << "Unable to create a connection to the database";

    medDataManager::initialize();

    // Registering different workspaces
    medWorkspaceFactory * viewerWSpaceFactory = medWorkspaceFactory::instance();
    viewerWSpaceFactory->registerWorkspace<medVisualizationWorkspace>();
    viewerWSpaceFactory->registerWorkspace<medRegistrationWorkspace>();
    viewerWSpaceFactory->registerWorkspace<medDiffusionWorkspace>();
    viewerWSpaceFactory->registerWorkspace<medFilteringWorkspace>();
    viewerWSpaceFactory->registerWorkspace<medSegmentationWorkspace>();
    viewerWSpaceFactory->registerWorkspace<medGenericWorkspace>();

    //Register settingsWidgets
    medSettingsWidgetFactory* settingsWidgetFactory = medSettingsWidgetFactory::instance();
    settingsWidgetFactory->registerSettingsWidget<medStartupSettingsWidget>();
    settingsWidgetFactory->registerSettingsWidget<medDatabaseSettingsWidget>();

    //Register annotations
    //TODO there is obviously something that have to be done here. - RDE
    //TODO I did something... was it enough ? - Flo
    medAbstractDataFactory * datafactory = medAbstractDataFactory::instance();
    datafactory->registerDataType<medSeedPointAnnotationData>();


    // process layer:
    QString pluginsPath = getenv("MEDINRIA_PLUGINS_DIR");
    medCore::pluginManager::initialize(pluginsPath);
}
