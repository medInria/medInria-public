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
#include <medLogger.h>
#include <medMainWindow.h>
#include <medPluginManager.h>
#include <medRegistrationWorkspace.h>
#include <medSeedPointAnnotationData.h>
#include <medSegmentationWorkspace.h>
#include <medSettingsManager.h>
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
};

// /////////////////////////////////////////////////////////////////
// medApplication
// /////////////////////////////////////////////////////////////////

medApplication::medApplication(int & argc, char**argv) :
        QtSingleApplication(argc,argv),
        d(new medApplicationPrivate)
{
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
    this->setApplicationName(PROJECT_NAME);     /*Beware, change database path*/
    this->setApplicationVersion(MEDINRIA_VERSION);
    this->setOrganizationName("INRIA_IHU-LIRYC"); /*Beware, change database path*/
    this->setOrganizationDomain("fr");

    medLogger::initialize();

    dtkInfo() << "####################################";
    dtkInfo() << "Version: "    << MEDINRIA_VERSION;
    dtkInfo() << "Build Date: " << MEDINRIA_BUILD_DATE;

    //  Redirect msgs to the logs
    QObject::connect(medPluginManager::instance(), SIGNAL(loadError(const QString &)),
                     &medLogger::instance(), SLOT(redirectErrorMessage(const QString&)));
    QObject::connect(medPluginManager::instance(), SIGNAL(loaded(const QString&)),
                     &medLogger::instance(), SLOT(redirectMessage(const QString&)));
    QObject::connect(this, SIGNAL(messageReceived(const QString&)),
                     &medLogger::instance(), SLOT(redirectMessage(const QString&)));

    QDate expiryDate = QDate::fromString(QString(MEDINRIA_BUILD_DATE), "dd_MM_yyyy").addYears(1);
    if ( ! expiryDate.isValid() || QDate::currentDate() > expiryDate)
    {
        QString expiredInfo = "This copy of ";
        expiredInfo += (char*)(PROJECT_NAME);
        expiredInfo += " has expired, please contact ";
        expiredInfo += (char*)(PROJECT_CONTACT);
        expiredInfo += " for more information.";
        QMessageBox msg;
        msg.setText(expiredInfo);
        msg.exec();
        ::exit(1);
    }
    d->mainWindow = NULL;

    // CSS
    QVariant themeChosen = medSettingsManager::instance()->value("startup","theme");
    int themeIndex = themeChosen.toInt();

    QString qssFile;
    switch (themeIndex)
    {
    case 0:
    default:
        qssFile = ":/music_dark.qss";
        this->setWindowIcon(QIcon(":music_logo_small_dark.png"));
        break;
    case 1:
        qssFile = ":/music_light.qss";
        this->setWindowIcon(QIcon(":music_logo_small_light.png"));
        break;
    }
    medStyleSheetParser parser(dtkReadFile(qssFile));
    this->setStyleSheet(parser.result());

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
