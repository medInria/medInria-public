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

#include <medGui/medStatusQuitButton.h>
#include <medGui/medSettingsButton.h>
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
#include "medViewerConfigurationVisualization2.h"
#include "medViewerConfigurationRegistration.h"
#include "medViewerConfigurationDiffusion.h"
#include "medViewerConfigurationFiltering.h"


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
    
    medWorkspaceShifter *shifter;
    medWorkspaceShifterAction *shiftToBrowserAreaAction;
    medWorkspaceShifterAction *shiftToViewerAreaAction;

    medSettingsEditor * settingsEditor;
    
    QPointer<medMessageControllerMessageQuestion> quitMessage;
};

#if defined(HAVE_SWIG) && defined(HAVE_PYTHON)
extern "C" int init_core(void);                  // -- Initialization core layer python wrapped functions
#endif

#if defined(HAVE_SWIG) && defined(HAVE_TCL)
extern "C" int Core_Init(Tcl_Interp *interp);    // -- Initialization core layer tcl    wrapped functions
#endif

medMainWindow::medMainWindow(QWidget *parent) : QMainWindow(parent), d(new medMainWindowPrivate)
{
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

    d->browserArea->setObjectName("Browser");
    d->viewerArea->setObjectName("Viewer");

    d->stack = new QStackedWidget(this);
    d->stack->addWidget(d->browserArea);
    d->stack->addWidget(d->viewerArea);

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
    medViewerConfigurationFactory::instance()->registerConfiguration("Filtering",     createMedViewerConfigurationFiltering);
    

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

    d->shiftToBrowserAreaAction->setChecked(true);

    connect(d->shiftToBrowserAreaAction, SIGNAL(triggered()), this, SLOT(switchToBrowserArea()));
    connect(d->shiftToViewerAreaAction,  SIGNAL(triggered()), this, SLOT(switchToViewerArea()));

    // medWorkspaceShifterMenu *menu = new medWorkspaceShifterMenu(this);
    // menu->addAction("Visualization");
    // menu->addAction("Registration");
    // menu->addAction("Diffusion");

    // connect(menu, SIGNAL(triggered(QAction *)), this, SLOT(onConfigurationTriggered(QAction *)));

    d->shifter = new medWorkspaceShifter(this);
    d->shifter->addAction(d->shiftToBrowserAreaAction);
    d->shifter->addAction(d->shiftToViewerAreaAction); //->setMenu(menu);

    medStatusQuitButton *quitButton = new medStatusQuitButton(this);

    connect(quitButton, SIGNAL(quit()), this, SLOT(onQuit()));
    
    medSettingsButton *settingsButton = new medSettingsButton(this);

    connect(settingsButton, SIGNAL(editSettings()), this, SLOT(onEditSettings()));

    QComboBox *configurationSwitcher = new QComboBox(this);
    configurationSwitcher->addItems (medViewerConfigurationFactory::instance()->configurations());
    configurationSwitcher->setFocusPolicy (Qt::NoFocus);

    this->statusBar()->setSizeGripEnabled(false);
    this->statusBar()->setContentsMargins(5, 0, 5, 0);
    this->statusBar()->setFixedHeight(31);
    this->statusBar()->addPermanentWidget(configurationSwitcher);
    this->statusBar()->addPermanentWidget(d->shifter);
    this->statusBar()->addPermanentWidget(settingsButton);
    this->statusBar()->addPermanentWidget(quitButton);

    this->readSettings();
    this->setCentralWidget(d->stack);
    this->setStyle(new medMainWindowStyle);
    this->setStyleSheet(dtkReadFile(":/medinria.qss"));
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
        this->switchToBrowserArea();
        break;

    case 1: 
        this->switchToViewerArea();
        break;

    default:
        this->switchToBrowserArea();
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

void medMainWindow::switchToBrowserArea(void)
{
    d->browserArea->setup(this->statusBar());
    d->viewerArea->setdw(this->statusBar());

    d->stack->setCurrentWidget(d->browserArea);

    d->shiftToBrowserAreaAction->setChecked(true);
    d->shiftToViewerAreaAction->setChecked(false);

    d->shifter->update();
}

void medMainWindow::switchToViewerArea(void)
{
    d->browserArea->setdw(this->statusBar());
    d->viewerArea->setup(this->statusBar());

    d->stack->setCurrentWidget(d->viewerArea);

    d->shiftToBrowserAreaAction->setChecked(false);
    d->shiftToViewerAreaAction->setChecked(true);

    d->shifter->update();
}

void medMainWindow::onConfigurationTriggered(QAction *action)
{
   d->viewerArea->setupConfiguration(action->text());
}

void medMainWindow::onQuit(void)
{

    // remove old quit message
    if(d->quitMessage != 0 )
    {
        WId id = d->quitMessage->effectiveWinId();

        if (this->statusBar()->find(id))
        {
            this->statusBar()->removeWidget(d->quitMessage);
            disconnect(d->quitMessage, SIGNAL(accepted()), this, SLOT(close()));
            disconnect(d->quitMessage, SIGNAL(rejected()), d->quitMessage, SLOT(deleteLater()));
            delete d->quitMessage;
        }

    }

    d->quitMessage = new medMessageControllerMessageQuestion(this, QString("Are sure you want to quit ?"), this);

    connect(d->quitMessage, SIGNAL(accepted()), this, SLOT(close()));
    connect(d->quitMessage, SIGNAL(rejected()), d->quitMessage, SLOT(deleteLater()));

    this->statusBar()->addWidget(d->quitMessage);
}

void medMainWindow::onEditSettings()
{
    if (d->settingsEditor)
    {
        d->settingsEditor->show();
        return;
    }

    d->settingsEditor = new medSettingsEditor(this);
    d->settingsEditor->setGeometry(100,100, 500, 500);
    d->settingsEditor->setWindowFlags(Qt::Tool);

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
}
