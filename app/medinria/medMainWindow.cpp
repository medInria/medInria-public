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

#include <medCore/medMessageController.h>
#include <medCore/medSettingsManager.h>
#include <medCore/medDbControllerFactory.h>
#include <medCore/medJobManager.h>
#include <medCore/medDataManager.h>

#include <medGui/medButton.h>
#include <medGui/medWorkspaceShifter.h>

#include <medSql/medDatabaseController.h>
#include <medSql/medDatabaseNonPersistentController.h>
#include <medSql/medDatabaseView.h>
#include <medSql/medDatabaseModel.h>
#include <medSql/medDatabaseItem.h>

#include <medGui/medViewerConfiguration.h>
#include <medGui/medViewerConfigurationFactory.h>
#include <medGui/medSettingsWidgetFactory.h>
#include <medGui/medSystemSettingsWidget.h>
#include <medGui/medStartupSettingsWidget.h>
#include <medGui/medDatabaseSettingsWidget.h>
#include <medGui/medSettingsEditor.h>

#include "medViewerConfigurationVisualization.h"
#include "medViewerConfigurationRegistration.h"
#include "medViewerConfigurationDiffusion.h"


#include <QtGui>

// /////////////////////////////////////////////////////////////////
// medMainWindowStyle
// /////////////////////////////////////////////////////////////////

class medMainWindowStyle : public QPlastiqueStyle
{
public:
    void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = 0) const
    {
        switch(element) {
        case PE_FrameFocusRect:
            break;
        default:
            QPlastiqueStyle::drawPrimitive(element, option, painter, widget);
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

    medBrowserArea *browserArea;
    medViewerArea  *viewerArea;
    medHomepageArea * homepageArea;
    
    medWorkspaceShifter *shifter;
    medWorkspaceShifterAction *shiftToBrowserAreaAction;
    medWorkspaceShifterAction *shiftToViewerAreaAction;
    medWorkspaceShifterAction *shiftToHomepageAreaAction;

    medSettingsEditor * settingsEditor;

    QHBoxLayout * statusBarLayout;
    
    QWidget * quitMessage;
    medButton *quitButton;
};

#if defined(HAVE_SWIG) && defined(HAVE_PYTHON)
extern "C" int init_core(void);                  // -- Initialization core layer python wrapped functions
#endif

#if defined(HAVE_SWIG) && defined(HAVE_TCL)
extern "C" int Core_Init(Tcl_Interp *interp);    // -- Initialization core layer tcl    wrapped functions
#endif

medMainWindow::medMainWindow(QWidget *parent) : QMainWindow(parent), d(new medMainWindowPrivate)
{
    // To avoid strange behaviours with the homepage
    this->setMinimumHeight(600);
    this->setMinimumWidth(800);
    
    // Setting up database connection
    if(!medDatabaseController::instance()->createConnection())
        qDebug() << "Unable to create a connection to the database";

    // Setting up menu
    QAction *windowFullScreenAction = new QAction("Toggle fullscreen mode", this);
    windowFullScreenAction->setShortcut(Qt::ControlModifier + Qt::ShiftModifier + Qt::Key_F);
    windowFullScreenAction->setToolTip("Toggle fullscreen mode");
    windowFullScreenAction->setCheckable(true);
    windowFullScreenAction->setChecked(false);

    connect(windowFullScreenAction, SIGNAL(toggled(bool)), this, SLOT(setFullScreen(bool)));

    this->addAction(windowFullScreenAction);

    // Setting up widgets
    d->settingsEditor = NULL;
    d->browserArea = new medBrowserArea(this);
    d->viewerArea = new medViewerArea(this);
    d->homepageArea = new medHomepageArea(this);
    d->homepageArea->setViewerArea(d->viewerArea);
    
    d->browserArea->setObjectName("Browser");
    d->viewerArea->setObjectName("Viewer");
    d->homepageArea->setObjectName("Homepage");

    d->stack = new QStackedWidget(this);
    d->stack->addWidget(d->homepageArea);
    d->stack->addWidget(d->browserArea);
    d->stack->addWidget(d->viewerArea);

    d->statusBarLayout = new QHBoxLayout;
    d->statusBarLayout->setMargin(0);
    d->statusBarLayout->setSpacing(0);
    
    connect(d->browserArea, SIGNAL(open(const QString&)), this, SLOT(open(const QString&)));
    connect(d->browserArea, SIGNAL(open(const medDataIndex&)), this, SLOT(open(const medDataIndex&)));
    

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

    //Register settingsWidgets
    medSettingsWidgetFactory::instance()->registerSettingsWidget("System", createSystemSettingsWidget);
    medSettingsWidgetFactory::instance()->registerSettingsWidget("Startup", createStartupSettingsWidget);
    medSettingsWidgetFactory::instance()->registerSettingsWidget("Database", createDatabaseSettingsWidget);

    //Register dbController 
    medDbControllerFactory::instance()->registerDbController("DbController", createDbController);
    medDbControllerFactory::instance()->registerDbController("NonPersistentDbController", createNonPersistentDbController);

    // Setting up status bar
    d->shiftToBrowserAreaAction = new medWorkspaceShifterAction("Browser");
    d->shiftToViewerAreaAction = new medWorkspaceShifterAction("Viewer");
    d->shiftToHomepageAreaAction = new medWorkspaceShifterAction("Homepage");

    d->shiftToBrowserAreaAction->setChecked(true);

    connect(d->shiftToBrowserAreaAction, SIGNAL(triggered()), this, SLOT(switchToBrowserArea()));
    connect(d->shiftToViewerAreaAction,  SIGNAL(triggered()), this, SLOT(switchToViewerArea()));
    connect(d->shiftToHomepageAreaAction,  SIGNAL(triggered()), this, SLOT(switchToHomepageArea()));

    // medWorkspaceShifterMenu *menu = new medWorkspaceShifterMenu(this);
    // menu->addAction("Visualization");
    // menu->addAction("Registration");
    // menu->addAction("Diffusion");

    // connect(menu, SIGNAL(triggered(QAction *)), this, SLOT(onConfigurationTriggered(QAction *)));

    d->shifter = new medWorkspaceShifter(this);
    d->shifter->addAction(d->shiftToHomepageAreaAction);
    d->shifter->addAction(d->shiftToBrowserAreaAction);
    d->shifter->addAction(d->shiftToViewerAreaAction); //->setMenu(menu);
    

    d->quitButton = new medButton(this,":/icons/quit.png", tr("Quit Application"));
    connect(d->quitButton, SIGNAL(triggered()), this, SLOT(onQuit()));
    
    medButton *settingsButton = new medButton(this, ":/icons/settings.png", tr("Edit Application Settings"));
    connect(settingsButton, SIGNAL(triggered()), this, SLOT(onEditSettings()));

    QComboBox *configurationSwitcher = new QComboBox(this);
    configurationSwitcher->addItems (medViewerConfigurationFactory::instance()->configurations());
    configurationSwitcher->setFocusPolicy (Qt::NoFocus);

    d->homepageArea->initPage();
    QObject::connect(d->homepageArea, SIGNAL(showBrowser()), this, SLOT(switchToBrowserArea()));
    QObject::connect(d->homepageArea, SIGNAL(showViewer()), this, SLOT(switchToViewerArea()));
    QObject::connect(d->homepageArea, SIGNAL(showConfiguration(QString)), d->viewerArea, SLOT(setupConfiguration(QString)));
    QObject::connect(d->homepageArea,SIGNAL(showSettings()), this, SLOT(onEditSettings()));
    
    
    this->statusBar()->setSizeGripEnabled(false);
    this->statusBar()->setContentsMargins(5, 0, 5, 0);
    this->statusBar()->setFixedHeight(31);

    QPushButton * homeButton = new QPushButton(this);
    homeButton->setIcon(QIcon(":icons/home.png"));
    homeButton->setStyleSheet("border: 1px solid #a7b6d3;");
    homeButton->setFlat(true);
    homeButton->setFocusPolicy(Qt::NoFocus);
    QObject::connect(homeButton, SIGNAL(clicked()), this, SLOT(switchToHomepageArea()));
    
    d->statusBarLayout->addWidget(homeButton);
    d->statusBarLayout->addStretch();

    d->quitButton->setMaximumWidth(31);
    settingsButton->setMaximumWidth(31);

    configurationSwitcher->hide();
    d->shifter->hide();
    settingsButton->hide();
    
//     d->statusBarLayout->insertWidget(3,configurationSwitcher);
//     d->statusBarLayout->insertWidget(4,d->shifter);
//     d->statusBarLayout->insertWidget(5,settingsButton);

    d->quitMessage = new QWidget(this);
    QHBoxLayout * quitLayout = new QHBoxLayout(this);
    QLabel *icon = new QLabel(this);
    icon->setMinimumHeight(30);
    icon->setPixmap(QPixmap(":/icons/information.png"));

    QLabel *info = new QLabel(this);
    info->setMinimumHeight(30);
    info->setText("Are you sure you want to quit ?");

    QPushButton *ok_button = new QPushButton("Yes", this);
    ok_button->setStyleSheet("border: 1px solid #a7b6d3;");
    ok_button->setFocusPolicy(Qt::NoFocus);
    QObject::connect(ok_button, SIGNAL(clicked()), this, SLOT(close()));
    QPushButton *no_button = new QPushButton("No", this);
    no_button->setStyleSheet("border: 1px solid #a7b6d3;");
    no_button->setFocusPolicy(Qt::NoFocus);
    QObject::connect(no_button, SIGNAL(clicked()), this, SLOT(onNoQuit()));
    
    quitLayout->setContentsMargins(5, 0, 5, 0);
    quitLayout->setSpacing(5);
    quitLayout->addWidget(icon);
    quitLayout->addWidget(info);
    quitLayout->addWidget(ok_button);
    quitLayout->addWidget(no_button);

    d->quitMessage->setLayout(quitLayout);
    
    d->statusBarLayout->addWidget(d->quitMessage);
    d->statusBarLayout->addWidget(d->quitButton);

    d->quitMessage->hide();

    QWidget * statusBarWidget = new QWidget(this);
    statusBarWidget->setContentsMargins(5, 0, 5, 0);
    statusBarWidget->setLayout(d->statusBarLayout);

    this->statusBar()->addPermanentWidget(statusBarWidget, 1);

    this->readSettings();
    this->setCentralWidget(d->stack);

    // Now use the Qt preferred method by setting the Application style instead.
    //   The ownership of the style object is not transferred.
//    this->setStyle(new QPlastiqueStyle());
    this->setWindowTitle("medinria");

    medMessageController::instance()->attach(this->statusBar());

    d->viewerArea->setupConfiguration("Visualization");

    connect(configurationSwitcher, SIGNAL(activated(QString)), d->viewerArea, SLOT(setupConfiguration(QString)));
    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(close()));
}

medMainWindow::~medMainWindow(void)
{

    delete d;

    d = NULL;
}

void medMainWindow::readSettings(void)
{
    // if the user configured a default area we need to show it
    medSettingsManager * mnger = medSettingsManager::instance();

    //if nothing is configured then Browser is the default area
    int areaIndex = mnger->value("startup", "default_starting_area", 0).toInt();

    switch (areaIndex)
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
    if (!this->isFullScreen())
    {
        QPoint pos = mnger->value("application", "pos", QPoint(200, 200)).toPoint();
        QSize size = mnger->value("application", "size", QSize(600, 400)).toSize();
        move(pos);
        resize(size);
    }

}

void medMainWindow::writeSettings()
{
    // if the app is in full screen mode we do not want to save the pos and size
    // as there is a setting that defines either the user wants to open the app in FS or not
    // so, if he/she chose not to and quit the app while in FS mode, we skip the settings saving
    if (!this->isFullScreen())
    {
        medSettingsManager::instance()->setValue("application","pos", pos());
        medSettingsManager::instance()->setValue("application","size", size());
    }
}

void medMainWindow::setWallScreen(bool full)
{
    if(full) {
        this->move(0, -30);
        this->resize(3528, 1200);
    } else {
        this->showNormal();
    }
}

void medMainWindow::setFullScreen(bool full)
{
    if(full)
        this->showFullScreen();
    else
        this->showNormal();
}

void medMainWindow::switchToHomepageArea ( void )
{
    d->stack->setCurrentWidget(d->homepageArea);
    d->homepageArea->onHideAbout();
    
    d->shiftToBrowserAreaAction->setChecked(false);
    d->shiftToViewerAreaAction->setChecked(false);
    d->shiftToHomepageAreaAction->setChecked(true);

    d->shifter->update();
    if (d->homepageArea->getAnimation())
        d->homepageArea->getAnimation()->start();
}


void medMainWindow::switchToBrowserArea(void)
{
    d->browserArea->setup(this->statusBar());
    d->viewerArea->setdw(this->statusBar());

    d->stack->setCurrentWidget(d->browserArea);

    d->shiftToBrowserAreaAction->setChecked(true);
    d->shiftToViewerAreaAction->setChecked(false);
    d->shiftToHomepageAreaAction->setChecked(false);

    d->shifter->update();
}

void medMainWindow::switchToViewerArea(void)
{
    d->browserArea->setdw(this->statusBar());
    d->viewerArea->setup(this->statusBar());

    d->stack->setCurrentWidget(d->viewerArea);

    d->shiftToBrowserAreaAction->setChecked(false);
    d->shiftToViewerAreaAction->setChecked(true);
    d->shiftToHomepageAreaAction->setChecked(false);

    d->shifter->update();
}

void medMainWindow::onConfigurationTriggered(QAction *action)
{
   d->viewerArea->setupConfiguration(action->text());
}

void medMainWindow::onNoQuit ( void )
{
    d->quitMessage->hide();
    d->quitButton->show();
}


void medMainWindow::onQuit(void)
{
    d->quitMessage->show();
    d->quitButton->hide();
}

void medMainWindow::onEditSettings()
{
    if (d->settingsEditor)
    {
        d->settingsEditor->show();
        return;
    }

    d->settingsEditor = new medSettingsEditor(this, true);
    d->settingsEditor->setGeometry(100,100, 500, 500);
    d->settingsEditor->setWindowFlags(Qt::Tool);
    d->settingsEditor->initialize();
    d->settingsEditor->queryWidgets();
    
    connect(d->settingsEditor, SIGNAL(finished()), d->settingsEditor, SLOT(close()) );
    
    d->settingsEditor->show();
}

void medMainWindow::open(const medDataIndex& index)
{
    this->switchToViewerArea();

    d->viewerArea->open(index);
}

void medMainWindow::open(const QString& file)
{
    d->viewerArea->open(file);

    this->switchToViewerArea();
}

void medMainWindow::closeEvent(QCloseEvent *event)
{

    if (QThreadPool::globalInstance()->activeThreadCount() > 0)
    {
        switch(QMessageBox::information( this, "System message", "Running background jobs detected! Quit anyway?",
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


