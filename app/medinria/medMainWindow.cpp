/* medMainWindow.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:48:07 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 15:25:37 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 503
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include "medHomepageButton.h"
#include "medBrowserArea.h"
#include "medMainWindow.h"
#include "medViewerArea.h"
#include "medHomepageArea.h"
#include "medViewerConfigurator.h"

#include <dtkCore/dtkGlobal.h>
#include <dtkScript/dtkScriptInterpreter.h>
#include <dtkScript/dtkScriptInterpreterPool.h>
#include <dtkScript/dtkScriptInterpreterPython.h>
#include <dtkScript/dtkScriptInterpreterTcl.h>

#include <dtkGui/dtkSpacer.h>

#include <medMessageController.h>
#include <medStatusBar.h>
#include <medSettingsManager.h>
#include <medDbControllerFactory.h>
#include <medJobManager.h>
#include <medDataManager.h>

#include <medButton.h>
#include <medQuickAccessMenu.h>
#include <medWorkspaceShifter.h>

#include <medDatabaseController.h>
#include <medDatabaseNonPersistentController.h>
#include <medDatabaseModel.h>
#include <medDatabaseItem.h>

#include <medViewerConfiguration.h>
#include <medViewerConfigurationFactory.h>
#include <medSettingsWidgetFactory.h>
#include <medSystemSettingsWidget.h>
#include <medStartupSettingsWidget.h>
#include <medDatabaseSettingsWidget.h>
#include <medSettingsEditor.h>

#include "medViewerConfigurationVisualization.h"
#include "medViewerConfigurationRegistration.h"
#include "medViewerConfigurationDiffusion.h"
#include "medViewerConfigurationFiltering.h"

#include "medSaveModifiedDialog.h"

#include <QtGui>

// /////////////////////////////////////////////////////////////////
// medMainWindowStyle
// /////////////////////////////////////////////////////////////////

class medMainWindowStyle : public QPlastiqueStyle
{
public:
    void drawPrimitive ( PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = 0 ) const
    {
        switch ( element )
        {
        case PE_FrameFocusRect:
            break;
        default:
            QPlastiqueStyle::drawPrimitive ( element, option, painter, widget );
            break;
        }
    }
};

// /////////////////////////////////////////////////////////////////
// medMainWindow
// /////////////////////////////////////////////////////////////////

class medMainWindowPrivate
{
public:
    QStackedWidget *stack;
    QList<QString> importUuids;

    medBrowserArea *browserArea;
    medViewerArea  *viewerArea;
    medHomepageArea * homepageArea;

    medSettingsEditor * settingsEditor;

    QHBoxLayout * statusBarLayout;

    medStatusBar * statusBar;
//     QWidget * quickAccessWidget;
    medQuickAccessMenu * quickAccessWidget;
    bool quickAccessVisible;

    medQuickAccessPushButton * quickAccessButton;
    QPropertyAnimation * quickAccessAnimation;

    QWidget * quitMessage;

    medButton *quitButton;
};

#if defined(HAVE_SWIG) && defined(HAVE_PYTHON)
extern "C" int init_core ( void );               // -- Initialization core layer python wrapped functions
#endif

#if defined(HAVE_SWIG) && defined(HAVE_TCL)
extern "C" int Core_Init ( Tcl_Interp *interp ); // -- Initialization core layer tcl    wrapped functions
#endif

medMainWindow::medMainWindow ( QWidget *parent ) : QMainWindow ( parent ), d ( new medMainWindowPrivate )
{
    // To avoid strange behaviours with the homepageshifter
    this->setMinimumHeight ( 600 );
    this->setMinimumWidth ( 800 );

    // register controller, configurations etc
    this->registerToFactories();

    // Setting up database connection
    if ( !medDatabaseController::instance()->createConnection() )
        qDebug() << "Unable to create a connection to the database";

    connect (medDatabaseNonPersistentController::instance(),SIGNAL(updated(const medDataIndex &, const QString&)),
             this,SLOT(onOpenFile(const medDataIndex&,const QString&)));

    // Setting up menu
    QAction *windowFullScreenAction = new QAction ( "Toggle fullscreen mode", this );
    windowFullScreenAction->setShortcut ( Qt::ControlModifier + Qt::ShiftModifier + Qt::Key_F );
    windowFullScreenAction->setToolTip ( "Toggle fullscreen mode" );
    windowFullScreenAction->setCheckable ( true );
    windowFullScreenAction->setChecked ( false );

    connect ( windowFullScreenAction, SIGNAL ( toggled ( bool ) ), this, SLOT ( setFullScreen ( bool ) ) );

    this->addAction ( windowFullScreenAction );

    // Setting up widgets
    d->settingsEditor = NULL;
    d->browserArea = new medBrowserArea ( this );
    d->viewerArea = new medViewerArea ( this );
    d->homepageArea = new medHomepageArea ( this );

    d->browserArea->setObjectName ( "Browser" );
    d->viewerArea->setObjectName ( "Viewer" );
    d->homepageArea->setObjectName ( "Homepage" );


    d->stack = new QStackedWidget ( this );
    d->stack->addWidget ( d->homepageArea );
    d->stack->addWidget ( d->browserArea );
    d->stack->addWidget ( d->viewerArea );

    d->statusBarLayout = new QHBoxLayout;
    d->statusBarLayout->setMargin ( 0 );
    d->statusBarLayout->setSpacing ( 5 );

    connect(d->browserArea, SIGNAL(open(const QString&)), this, SLOT(open(const QString&)));
    connect(d->browserArea, SIGNAL(load(const QString&)), this, SLOT(load(const QString&)));
    connect(d->browserArea, SIGNAL(open(const medDataIndex&)), this, SLOT(open(const medDataIndex&)));

    // Setting up status bar
    //Setup quick access menu
    d->quickAccessButton = new medQuickAccessPushButton ( this );
    d->quickAccessButton->setFocusPolicy ( Qt::NoFocus );
    d->quickAccessButton->setMinimumHeight(31);
    d->quickAccessButton->setStyleSheet("border: 0px;");
    d->quickAccessButton->setIcon(QIcon(":medinria.ico"));
    d->quickAccessButton->setCursor(Qt::PointingHandCursor);
    d->quickAccessButton->setText ( "Workspaces access menu" );
    connect ( d->quickAccessButton,  SIGNAL ( clicked() ), this, SLOT ( onShowQuickAccess() ) );
    
    d->quickAccessWidget = new medQuickAccessMenu( this );
    d->quickAccessWidget->setFocusPolicy(Qt::ClickFocus);
    d->quickAccessWidget->setProperty ( "pos", QPoint ( 0, -500 ) );
    d->quickAccessWidget->setMinimumWidth(180);
    connect(d->quickAccessWidget, SIGNAL(hideMenu()), this, SLOT(onHideQuickAccess()));

    d->quickAccessVisible = false;
    d->quickAccessAnimation = new QPropertyAnimation ( d->quickAccessWidget, "pos",this );

    //Add quit button
    d->quitButton = new medButton ( this,":/icons/quit.png", tr ( "Quit Application" ) );
    connect ( d->quitButton, SIGNAL ( triggered() ), this, SLOT ( onQuit() ) );
    connect(d->quitButton, SIGNAL( triggered()), this, SLOT (onSaveModified()));
    d->quitButton->setMaximumWidth ( 31 );

    //Setup quit message
    d->quitMessage = new QWidget ( this );
    QHBoxLayout * quitLayout = new QHBoxLayout;
    QLabel *icon = new QLabel ( this );
    icon->setMinimumHeight ( 30 );
    icon->setPixmap ( QPixmap ( ":/icons/information.png" ) );
    QLabel *info = new QLabel ( this );
    info->setMinimumHeight ( 30 );
    info->setText ( "Are you sure you want to quit ?" );
    QPushButton *ok_button = new QPushButton ( "Yes", this );
    ok_button->setFocusPolicy ( Qt::NoFocus );
    QObject::connect ( ok_button, SIGNAL ( clicked() ), this, SLOT ( close() ) );
    QPushButton *no_button = new QPushButton ( "No", this );
    no_button->setFocusPolicy ( Qt::NoFocus );
    QObject::connect ( no_button, SIGNAL ( clicked() ), this, SLOT ( onNoQuit() ) );

    quitLayout->setContentsMargins ( 5, 0, 5, 0 );
    quitLayout->setSpacing ( 5 );
    quitLayout->addWidget ( icon );
    quitLayout->addWidget ( info );
    quitLayout->addWidget ( ok_button );
    quitLayout->addWidget ( no_button );

    d->quitMessage->setLayout ( quitLayout );

    d->statusBarLayout->addWidget ( d->quickAccessButton );
    d->statusBarLayout->addStretch();
    d->statusBarLayout->addWidget ( d->quitMessage );
    d->statusBarLayout->addWidget ( d->quitButton );
    d->quitMessage->hide();

    //Create a container widget for the status bar
    QWidget * statusBarWidget = new QWidget ( this );
    statusBarWidget->setContentsMargins ( 5, 0, 5, 0 );
    statusBarWidget->setLayout ( d->statusBarLayout );

    //Setup status bar
    d->statusBar = new medStatusBar(this);
    d->statusBar->setStatusBarLayout(d->statusBarLayout);
    d->statusBar->setSizeGripEnabled ( false );
    d->statusBar->setContentsMargins ( 5, 0, 5, 0 );
    d->statusBar->setFixedHeight ( 31 );
    d->statusBar->addPermanentWidget ( statusBarWidget, 1 );

    this->setStatusBar(d->statusBar);
    
//     this->statusBar()->setSizeGripEnabled ( false );
//     this->statusBar()->setContentsMargins ( 5, 0, 5, 0 );
//     this->statusBar()->setFixedHeight ( 31 );
//     this->statusBar()->addPermanentWidget ( statusBarWidget, 1 );

    this->readSettings();

    //Init homepage with configuration buttons
    d->homepageArea->initPage();
    QObject::connect ( d->homepageArea, SIGNAL ( showBrowser() ), this, SLOT ( switchToBrowserArea() ) );
    QObject::connect ( d->homepageArea, SIGNAL ( showConfiguration ( QString ) ), this, SLOT ( onShowConfiguration ( QString ) ) );
    QObject::connect ( d->homepageArea,SIGNAL ( showSettings() ), this, SLOT ( onEditSettings() ) );

    //Add configuration button to the quick access menu
    this->updateQuickAccessMenu();

    this->setCentralWidget ( d->stack );

    // Now use the Qt preferred method by setting the Application style instead.
    // The ownership of the style object is not transferred.
    // this->setStyle(new QPlastiqueStyle());
    this->setWindowTitle ( "medinria" );

    //Connect the messageController with the status for notification messages management
    QObject::connect(medMessageController::instance(), SIGNAL(addMessage(QWidget*)), d->statusBar, SLOT(addMessage(QWidget*)));
    QObject::connect(medMessageController::instance(), SIGNAL(removeMessage(QWidget*)), d->statusBar, SLOT(removeMessage(QWidget*)));

    d->viewerArea->setupConfiguration ( "Visualization" );

    connect ( qApp, SIGNAL ( aboutToQuit() ), this, SLOT ( close() ) );


}

medMainWindow::~medMainWindow ( void )
{
    delete d;

    d = NULL;
}

void medMainWindow::readSettings ( void )
{
    // if the user configured a default area we need to show it
    medSettingsManager * mnger = medSettingsManager::instance();

    //if nothing is configured then Browser is the default area
    int areaIndex = mnger->value ( "startup", "default_starting_area", 0 ).toInt();

    switch ( areaIndex )
    {
    case 0:
        this->switchToHomepageArea();
        break;

    case 1:
        this->switchToBrowserArea();
        break;

    case 2:
        this->switchToViewerArea();
        break;

    default:
        this->switchToHomepageArea();
        break;
    }

    // restore size
    if ( !this->isFullScreen() )
    {
        QPoint pos = mnger->value ( "application", "pos", QPoint ( 200, 200 ) ).toPoint();
        QSize size = mnger->value ( "application", "size", QSize ( 600, 400 ) ).toSize();
        move ( pos );
        resize ( size );
    }

}

void medMainWindow::writeSettings()
{
    // if the app is in full screen mode we do not want to save the pos and size
    // as there is a setting that defines either the user wants to open the app in FS or not
    // so, if he/she chose not to and quit the app while in FS mode, we skip the settings saving
    if ( !this->isFullScreen() )
    {
        medSettingsManager::instance()->setValue ( "application","pos", pos() );
        medSettingsManager::instance()->setValue ( "application","size", size() );
    }
}

void medMainWindow::updateQuickAccessMenu ( void )
{
    QList<QString> configList = medViewerConfigurationFactory::instance()->configurations();
    QVBoxLayout * configurationButtonsLayout = new QVBoxLayout;
    configurationButtonsLayout->setMargin(0);
    configurationButtonsLayout->setSpacing ( 0 );

    //Setup quick access menu title
    QLabel * configurationLabel = new QLabel ( "<b>Switch to workspaces</b>" );
    configurationLabel->setMaximumWidth(300);
    configurationLabel->setFixedHeight(25);
    configurationLabel->setAlignment(Qt::AlignHCenter);
    configurationLabel->setTextFormat(Qt::RichText);
    //It's more easy to set the stylesheet here than in the qss file
    configurationLabel->setStyleSheet("border-image: url(:/pixmaps/toolbox-header.png) 16 16 0 16 repeat-x;\
                                       border-left-width: 0px;\
                                       border-right-width: 0px;\
                                       border-top-width: 8px;\
                                       border-bottom-width: 0px;");
    configurationButtonsLayout->addWidget ( configurationLabel );

    //Setup homepage access button
    medHomepagePushButton * homeButton = new medHomepagePushButton ( this );
    homeButton->setText("Home");
    homeButton->setIcon ( QIcon ( ":icons/home.png" ) );
    homeButton->setFixedHeight ( 40 );
    homeButton->setMaximumWidth ( 250 );
    homeButton->setMinimumWidth ( 250 );
    homeButton->setStyleSheet("border: 0px;");
    homeButton->setFocusPolicy ( Qt::NoFocus );
    homeButton->setCursor(Qt::PointingHandCursor);
    configurationButtonsLayout->addWidget ( homeButton );
    QObject::connect ( homeButton, SIGNAL ( clicked() ), this, SLOT ( switchToHomepageArea() ) );

    //Setup browser access button
    medHomepagePushButton * browserButton = new medHomepagePushButton ( this );
    browserButton->setCursor(Qt::PointingHandCursor);
    browserButton->setStyleSheet("border: 0px;");
    browserButton->setIcon ( QIcon ( ":/icons/folder.png" ) );
    browserButton->setText ( "Browser" );
    browserButton->setFixedHeight ( 40 );
    browserButton->setMaximumWidth ( 250 );
    browserButton->setMinimumWidth ( 250 );
    browserButton->setFocusPolicy ( Qt::NoFocus );
    configurationButtonsLayout->addWidget ( browserButton );
    QObject::connect ( browserButton, SIGNAL ( clicked() ),this, SLOT ( switchToBrowserArea()) );

    //Dynamically setup configurations access button
    for ( int i = 0; i< configList.size(); i++ )
    {
        medHomepagePushButton * button = new medHomepagePushButton ( this );
        button->setText ( configList.at ( i ) );
        button->setFocusPolicy ( Qt::NoFocus );
        button->setCursor(Qt::PointingHandCursor);
        button->setStyleSheet("border: 0px;");
        button->setFixedHeight ( 40 );
        button->setMaximumWidth ( 250 );
        button->setMinimumWidth ( 250 );;
        configurationButtonsLayout->addWidget ( button );
        QObject::connect ( button, SIGNAL ( clicked ( QString ) ),this, SLOT ( onShowConfiguration ( QString ) ) );
    }
    configurationButtonsLayout->addStretch();
    d->quickAccessAnimation->setEndValue ( QPoint ( 0,this->height() - d->quickAccessWidget->height() - 30 ) );
    d->quickAccessWidget->setMinimumHeight ( 20 + 40 * ( 2 + configList.size() ) );
    d->quickAccessWidget->setLayout(configurationButtonsLayout);
}

void medMainWindow::resizeEvent ( QResizeEvent* event )
{
    QWidget::resizeEvent ( event );
    d->quickAccessWidget->setProperty ( "pos", QPoint ( 0, this->height() - 30 ));
    this->onHideQuickAccess();
}


void medMainWindow::setWallScreen ( bool full )
{
    if ( full )
    {
        this->move ( 0, -30 );
        this->resize ( 3528, 1200 );
    }
    else
    {
        this->showNormal();
    }
}

void medMainWindow::setFullScreen ( bool full )
{
    if ( full )
        this->showFullScreen();
    else
        this->showNormal();
}

void medMainWindow::switchToHomepageArea ( void )
{
    d->quickAccessButton->setText("Workspaces access menu");
    d->quickAccessButton->setMinimumWidth(170);
    if (d->quickAccessVisible)
        this->onHideQuickAccess();
    d->stack->setCurrentWidget ( d->homepageArea );
    d->homepageArea->onHideAbout();

    if ( d->homepageArea->getAnimation() )
        d->homepageArea->getAnimation()->start();
}

void medMainWindow::switchToBrowserArea ( void )
{
    d->quickAccessButton->setText("Workspace: Browser");
    d->quickAccessButton->setMinimumWidth(170);
    if (d->quickAccessVisible)
        this->onHideQuickAccess();
    d->browserArea->setup ( this->statusBar() );
    d->viewerArea->setdw ( this->statusBar() );

    d->stack->setCurrentWidget ( d->browserArea );
}

void medMainWindow::switchToViewerArea ( void )
{
    if (d->quickAccessVisible)
        this->onHideQuickAccess();
    d->browserArea->setdw ( this->statusBar() );
    d->viewerArea->setup ( this->statusBar() );

    d->stack->setCurrentWidget ( d->viewerArea );
}

void medMainWindow::onShowConfiguration ( QString config )
{
    d->quickAccessButton->setMinimumWidth(170);
    d->viewerArea->setupConfiguration(config);
    this->switchToViewerArea();
    d->quickAccessButton->setText("Workspace: " + config);
}

void medMainWindow::onShowQuickAccess ( void )
{
    if ( d->quickAccessVisible )
    {
        this->onHideQuickAccess();
        return;
    }
    d->quickAccessWidget->setFocus();
    d->quickAccessVisible = true;
    d->quickAccessAnimation->setDuration ( 100 );
    d->quickAccessAnimation->setStartValue ( QPoint ( 0,this->height() - 30 ) );
    d->quickAccessAnimation->setEndValue ( QPoint ( 0,this->height() - d->quickAccessWidget->height() - 30 ) );

    d->quickAccessAnimation->start();
}

void medMainWindow::onHideQuickAccess ( void )
{
    if (!d->quickAccessVisible)
        return;
    d->quickAccessVisible = false;
    d->quickAccessAnimation->setDuration ( 100 );
    d->quickAccessAnimation->setStartValue ( QPoint ( 0,this->height() - d->quickAccessWidget->height() -30 ));
    d->quickAccessAnimation->setEndValue ( QPoint ( 0,this->height() - 30 ) );
    d->quickAccessAnimation->start();
}

void medMainWindow::onConfigurationTriggered ( QAction *action )
{
    d->viewerArea->setupConfiguration ( action->text() );
}

void medMainWindow::onNoQuit ( void )
{
    d->quitMessage->hide();
    d->quitButton->show();
}

void medMainWindow::onQuit ( void )
{
    d->quitMessage->show();
    d->quitButton->hide();
}

void medMainWindow::onSaveModified( void )
{
    QList<medDataIndex> indexes = medDatabaseNonPersistentController::instance()->availableItems();

    if(!indexes.isEmpty())
    {
        medSaveModifiedDialog *saveDialog = new medSaveModifiedDialog(this);
        saveDialog->show();
    }
}

void medMainWindow::onEditSettings()
{
    QDialog * dialog = new QDialog(this);
    dialog->setMinimumHeight(400);
    dialog->setMinimumWidth(500);
    dialog->setMaximumHeight(400);
    dialog->setMaximumWidth(500);

    QVBoxLayout * layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(0,0,0,0);;

    d->settingsEditor = new medSettingsEditor ( this, true );
    d->settingsEditor->setGeometry ( 100,100, 500, 500 );
    d->settingsEditor->setWindowFlags ( Qt::Tool );
    d->settingsEditor->initialize();
    d->settingsEditor->queryWidgets();

    layout->addWidget(d->settingsEditor);
    dialog->setLayout(layout);
    
    connect ( d->settingsEditor, SIGNAL ( finished() ), dialog, SLOT ( close() ) );

    dialog->exec();
}

void medMainWindow::open ( const medDataIndex& index )
{
   if(d->viewerArea->openInTab(index))
    {
        d->quickAccessButton->setText("Workspace: Visualization");
        d->quickAccessButton->setMinimumWidth(170);
        this->switchToViewerArea();
    }
}

void medMainWindow::open ( const QString& file )
{
    QString importUuid = QUuid::createUuid().toString();
    d->importUuids.append(importUuid);
    qDebug() << "about to open file" << file;
    qDebug()<< "with uuid:" << importUuid;
    medDataManager::instance()->importNonPersistent (file,importUuid);
}

void medMainWindow::onOpenFile(const medDataIndex & index,const QString& importUuid)
{
//    qDebug() << "Opened file from uuid" << importUuid;
//    qDebug() << "uuids in list" << d->importUuids;
    if (!importUuid.isEmpty() && d->importUuids.contains(importUuid))
    {
        if (index.isValid())
        {
            d->viewerArea->openInTab(index);
            d->quickAccessButton->setText("Workspace: Visualization");
            d->quickAccessButton->setMinimumWidth(170);
            this->switchToViewerArea();
        }
        else
        {
            qDebug() << "Could not Load file with uuid" << importUuid;
        }
        d->importUuids.removeOne(importUuid);
    }
}


void medMainWindow::load(const QString& file)
{
    qDebug() << "DEBUG : entering medMainWindow::load";
    medDataManager::instance()->importNonPersistent (file);
}

void medMainWindow::closeEvent(QCloseEvent *event)
{

    if ( QThreadPool::globalInstance()->activeThreadCount() > 0 )
    {
        switch ( QMessageBox::information ( this, "System message", "Running background jobs detected! Quit anyway?",
                                            "Quit", "WaitForDone", "Cancel",0, 1 ) )
        {
        case 0:
            // send cancel request to all running jobs, then wait for them
            // Note: most Jobs don't have the cancel method implemented, so this will be effectively the same as waitfordone.
            this->hide();
            medJobManager::instance()->dispatchGlobalCancelEvent();
            QThreadPool::globalInstance()->waitForDone();
            event->accept();
            break;

        case 1:
            // just hide the window and wait
            this->hide();
            QThreadPool::globalInstance()->waitForDone();
            event->accept();
            break;

        default:
            event->ignore();
            return;
            break;
        }

    }

    this->writeSettings();

    medDatabaseController::destroy();
    medDatabaseNonPersistentController::destroy();
    medDataManager::destroy();
}

void medMainWindow::registerToFactories()
{
    //Register dbController
    medDbControllerFactory::instance()->registerDbController("DbController", createDbController);
    medDbControllerFactory::instance()->registerDbController("NonPersistentDbController", createNonPersistentDbController);


#if defined(HAVE_SWIG) && defined(HAVE_PYTHON)
    // Setting up core python module

    dtkScriptInterpreterPythonModuleManager::instance()->registerInitializer(&init_core);
    dtkScriptInterpreterPythonModuleManager::instance()->registerCommand(
        "import core"
        );
    dtkScriptInterpreterPythonModuleManager::instance()->registerCommand(
        "dataFactory    = core.dtkAbstractDataFactory.instance()"
        );
    dtkScriptInterpreterPythonModuleManager::instance()->registerCommand(
        "processFactory = core.dtkAbstractProcessFactory.instance()"
        );
    dtkScriptInterpreterPythonModuleManager::instance()->registerCommand(
        "viewFactory    = core.dtkAbstractViewFactory.instance()"
        );
    dtkScriptInterpreterPythonModuleManager::instance()->registerCommand(
        "pluginManager  = core.dtkPluginManager.instance()"
        );
#endif
#if defined(HAVE_SWIG) && defined(HAVE_TCL)
    // Setting up core tcl module

    dtkScriptInterpreterTclModuleManager::instance()->registerInitializer(&Core_Init);
    dtkScriptInterpreterTclModuleManager::instance()->registerCommand(
        "set dataFactory    [dtkAbstractDataFactory_instance]"
        );
    dtkScriptInterpreterTclModuleManager::instance()->registerCommand(
        "set processFactory [dtkAbstractProcessFactory_instance]"
        );
    dtkScriptInterpreterTclModuleManager::instance()->registerCommand(
        "set viewFactory    [dtkAbstractViewFactory_instance]"
        );
    dtkScriptInterpreterTclModuleManager::instance()->registerCommand(
        "set pluginManager  [dtkPluginManager_instance]"
        );
#endif

    // Registering different configurations
    medViewerConfigurationFactory::instance()->registerConfiguration("Visualization", createMedViewerConfigurationVisualization);
    medViewerConfigurationFactory::instance()->registerConfiguration("Registration",  createMedViewerConfigurationRegistration);
    medViewerConfigurationFactory::instance()->registerConfiguration("Diffusion",     createMedViewerConfigurationDiffusion);
    medViewerConfigurationFactory::instance()->registerConfiguration("Filtering",     createMedViewerConfigurationFiltering);
    
    //Register settingsWidgets
    medSettingsWidgetFactory::instance()->registerSettingsWidget("System", createSystemSettingsWidget);
    medSettingsWidgetFactory::instance()->registerSettingsWidget("Startup", createStartupSettingsWidget);
    medSettingsWidgetFactory::instance()->registerSettingsWidget("Database", createDatabaseSettingsWidget);
}
