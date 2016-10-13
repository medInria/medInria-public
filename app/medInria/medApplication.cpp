/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medApplication.h>

#include <dtkCore/dtkGlobal.h>
#include <dtkLog/dtkLog.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>

#include <locale.h>

#include <medAbstractDataFactory.h>
#include <medAbstractWorkspace.h>
#include <medDataManager.h>
#include <medDatabaseController.h>
#include <medDatabaseNonPersistentController.h>
#include <medDatabaseSettingsWidget.h>
#include <medDiffusionWorkspace.h>
#include <medFilteringWorkspace.h>
#include <medMainWindow.h>
#include <medPluginManager.h>
#include <medQtMessageHandler.h>
#include <medRegistrationWorkspace.h>
#include <medSeedPointAnnotationData.h>
#include <medSegmentationWorkspace.h>
#include <medSettingsWidget.h>
#include <medSettingsWidgetFactory.h>
#include <medStartupSettingsWidget.h>
#include <medStyleSheetParser.h>
#include <medVisualizationWorkspace.h>
#include <medWorkspaceFactory.h>

#include <QtGui>


class medApplicationPrivate
{
public:
    medMainWindow *mainWindow;
    QStringList systemOpenInstructions;
    QThreadStorage<bool> logRecursionFlag;
};

// /////////////////////////////////////////////////////////////////
// medApplication
// /////////////////////////////////////////////////////////////////

medApplication::medApplication(int & argc, char**argv) :
        QtSingleApplication(argc,argv),
        d(new medApplicationPrivate)
{
    QDate expiryDate = QDate::fromString(QString(MEDINRIA_BUILD_DATE), "dd_MM_yyyy").addYears(1);
    if ( ! expiryDate.isValid() || QDate::currentDate() > expiryDate)
    {
        QMessageBox msg;
        msg.setText("This copy of MUSIC has expired, please contact "
        "maxime.sermesant@inria.fr for more information.");
        msg.exec();
        ::exit(1);
    }
    d->mainWindow = NULL;

    this->setApplicationName("MUSIC");            /*Beware, change database path*/
    this->setApplicationVersion(MEDINRIA_VERSION);
    this->setOrganizationName("INRIA_IHU-LIRYC"); /*Beware, change database path*/
    this->setOrganizationDomain("fr");
    this->setWindowIcon(QIcon(":music_logo_small.png"));

    medStyleSheetParser parser(dtkReadFile(":/medInria.qss"));
    this->setStyleSheet(parser.result());

    //  Redirect msgs to the logs

    QObject::connect(medPluginManager::instance(), SIGNAL(loadError(const QString &)),
                     this, SLOT(redirectErrorMessageToLog(const QString&)) );
    QObject::connect(medPluginManager::instance(), SIGNAL(loaded(QString)),
                     this, SLOT(redirectMessageToLog(QString)) );

    QObject::connect(this,SIGNAL(messageReceived(const QString&)),
                     this,SLOT(redirectMessageToLog(QString)));

    QObject::connect(&medQtMessageHandler::instance(), SIGNAL(newQtMsg(QtMsgType, const QString&)),
                    this,SLOT(receiveQtMsg(QtMsgType , const QString&)));

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

    // If there are any requests to open files not yet treated, send signal to do so
    foreach(QString openInstruction, d->systemOpenInstructions)
    {
        emit messageReceived(openInstruction);
    }

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

    //  Setting up database connection
    if ( ! medDatabaseController::instance()->createConnection())
        qDebug() << "Unable to create a connection to the database";

    medDataManager::initialize();

    // Registering different workspaces
    medWorkspaceFactory * viewerWSpaceFactory = medWorkspaceFactory::instance();
    viewerWSpaceFactory->registerWorkspace<medVisualizationWorkspace>();
    viewerWSpaceFactory->registerWorkspace<medRegistrationWorkspace>();
    //viewerWSpaceFactory->registerWorkspace<medDiffusionWorkspace>();
    viewerWSpaceFactory->registerWorkspace<medFilteringWorkspace>();
    viewerWSpaceFactory->registerWorkspace<medSegmentationWorkspace>();

    //Register settingsWidgets
    medSettingsWidgetFactory* settingsWidgetFactory = medSettingsWidgetFactory::instance();
    settingsWidgetFactory->registerSettingsWidget<medStartupSettingsWidget>();
    settingsWidgetFactory->registerSettingsWidget<medDatabaseSettingsWidget>();

    //Register annotations
    //TODO there is obviously something that have to be done here. - RDE
    //TODO I did something... was it enough ? - Flo
    medAbstractDataFactory * datafactory = medAbstractDataFactory::instance();
    datafactory->registerDataType<medSeedPointAnnotationData>();
}

void medApplication::receiveQtMsg(QtMsgType type, const QString& message)
{
    if (!d->logRecursionFlag.hasLocalData())
    {
        d->logRecursionFlag.setLocalData(false);
    }

    if (!d->logRecursionFlag.localData())
    {
        d->logRecursionFlag.setLocalData(true);

        switch (type)
        {
        case QtDebugMsg:
            dtkDebug()<<message;
            break;
        case QtWarningMsg:
            dtkWarn()<<message;
            break;
        case QtCriticalMsg:
            dtkError()<<message;
            break;
        case QtFatalMsg:
            dtkFatal()<<message;
            abort();
        }
    }

    d->logRecursionFlag.setLocalData(false);
}
