/* medMainWindow.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:48:07 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Jun  9 09:15:28 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 404
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medBrowserArea.h"
#include "medDocumentationArea.h"
#include "medMainWindow.h"
#include "medViewerArea.h"
#include "medViewerConfigurator.h"
#include "medWelcomeArea.h"

#include <dtkCore/dtkGlobal.h>

#include <dtkScript/dtkScriptInterpreter.h>
#include <dtkScript/dtkScriptInterpreterPool.h>
#include <dtkScript/dtkScriptInterpreterPython.h>
#include <dtkScript/dtkScriptInterpreterTcl.h>

#include <dtkGui/dtkSpacer.h>

#include <medGui/medWorkspaceShifter.h>

#include <medSql/medDatabaseController.h>
#include <medSql/medDatabaseView.h>
#include <medSql/medDatabaseModel.h>
#include <medSql/medDatabaseItem.h>

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

    medWelcomeArea       *welcomeArea;
    medBrowserArea       *browserArea;
    medViewerArea        *viewerArea;
    medDocumentationArea *documentationArea;

    medWorkspaceShifterAction *shiftToWelcomeAreaAction;
    medWorkspaceShifterAction *shiftToBrowserAreaAction;
    medWorkspaceShifterAction *shiftToViewerAreaAction;
    medWorkspaceShifterAction *shiftToDocumentationAreaAction;
};

extern "C" int init_core(void);                  // -- Initialization core layer python wrapped functions
extern "C" int Core_Init(Tcl_Interp *interp);    // -- Initialization core layer tcl    wrapped functions

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

    QMenu *windowMenu = this->menuBar()->addMenu("Window");
    windowMenu->addAction(windowFullScreenAction);

    // Setting up widgets

    d->welcomeArea = new medWelcomeArea(this);
    d->browserArea = new medBrowserArea(this);
    d->viewerArea = new medViewerArea(this);
    d->documentationArea = new medDocumentationArea(this);

    d->welcomeArea->setObjectName("Welcome");
    d->browserArea->setObjectName("Browser");
    d->viewerArea->setObjectName("Viewer");
    d->viewerArea->setObjectName("Documentation");

    d->stack = new QStackedWidget(this);
    d->stack->addWidget(d->welcomeArea);
    d->stack->addWidget(d->browserArea);
    d->stack->addWidget(d->viewerArea);
    d->stack->addWidget(d->documentationArea);

    connect(d->browserArea->view(), SIGNAL(patientDoubleClicked(const QModelIndex&)), this, SLOT(onPatientDoubleClicked (const QModelIndex&)));
    connect(d->browserArea->view(), SIGNAL(studyDoubleClicked(const QModelIndex&)), this, SLOT(onStudyDoubleClicked (const QModelIndex&)));
    connect(d->browserArea->view(), SIGNAL(seriesDoubleClicked(const QModelIndex&)), this, SLOT(onSeriesDoubleClicked (const QModelIndex&)));

    connect(d->browserArea, SIGNAL(open(const QString&)), this, SLOT(open(const QString&)));

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

    // Setting up console script interpreter

    // dtkScriptInterpreter *interpreter = dtkScriptInterpreterPool::instance()->console("python");
    // interpreter->start();
    // QObject::connect(qApp, SIGNAL(aboutToQuit()), interpreter, SLOT(stop()));

    // Setting up status bar

    d->shiftToWelcomeAreaAction = new medWorkspaceShifterAction("Welcome");
    d->shiftToBrowserAreaAction = new medWorkspaceShifterAction("Browser");
    d->shiftToViewerAreaAction  = new medWorkspaceShifterAction("Viewer");
    d->shiftToDocumentationAreaAction  = new medWorkspaceShifterAction("Documentation");

    d->shiftToWelcomeAreaAction->setChecked(true);

    connect(d->shiftToWelcomeAreaAction, SIGNAL(triggered()), this, SLOT(switchToWelcomeArea()));
    connect(d->shiftToBrowserAreaAction, SIGNAL(triggered()), this, SLOT(switchToBrowserArea()));
    connect(d->shiftToViewerAreaAction,  SIGNAL(triggered()), this, SLOT(switchToViewerArea()));
    connect(d->shiftToDocumentationAreaAction, SIGNAL(triggered()), this, SLOT(switchToDocumentationArea()));

    medWorkspaceShifterMenu *menu = new medWorkspaceShifterMenu(this);
    menu->addAction("Visualization");
    menu->addAction("Registration");
    menu->addAction("Diffusion");

    connect(menu, SIGNAL(triggered(QAction *)), this, SLOT(onConfigurationTriggered(QAction *)));

    medWorkspaceShifter *shifter = new medWorkspaceShifter(this);
    shifter->addAction(d->shiftToWelcomeAreaAction);
    shifter->addAction(d->shiftToBrowserAreaAction);
    shifter->addAction(d->shiftToViewerAreaAction)->setMenu(menu);
    shifter->addAction(d->shiftToDocumentationAreaAction);

    this->statusBar()->setSizeGripEnabled(false);
    this->statusBar()->setContentsMargins(5, 0, 5, 0);
    this->statusBar()->setFixedHeight(31);
    this->statusBar()->addPermanentWidget(shifter);

    this->readSettings();
    this->setCentralWidget(d->stack);
    this->setStyle(new medMainWindowStyle);
    this->setStyleSheet(dtkReadFile(":/medular.qss"));
    this->setWindowTitle("medular");
    this->switchToWelcomeArea();
}

medMainWindow::~medMainWindow(void)
{
    delete d;

    d = NULL;
}

void medMainWindow::readSettings(void)
{
    QSettings settings("inria", "medular");
    settings.beginGroup("medular");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(600, 400)).toSize();
    move(pos);
    resize(size);
    settings.endGroup();
}

void medMainWindow::writeSettings(void)
{
    QSettings settings("inria", "medular");
    settings.beginGroup("medular");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.endGroup();
}

void medMainWindow::setWallScreen(bool full)
{
    if(full) {
        this->move(0, -50);
        this->resize(3528, 1230);
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

void medMainWindow::switchToWelcomeArea(void)
{
    d->welcomeArea->setup(this->statusBar());
    d->browserArea->setdw(this->statusBar());
    d->viewerArea->setdw(this->statusBar());
    d->documentationArea->setdw(this->statusBar());

    d->stack->setCurrentWidget(d->welcomeArea);

    d->shiftToWelcomeAreaAction->setChecked(true);
    d->shiftToBrowserAreaAction->setChecked(false);
    d->shiftToViewerAreaAction->setChecked(false);
    d->shiftToDocumentationAreaAction->setChecked(false);
}

void medMainWindow::switchToBrowserArea(void)
{
    d->welcomeArea->setdw(this->statusBar());
    d->browserArea->setup(this->statusBar());
    d->viewerArea->setdw(this->statusBar());
    d->documentationArea->setdw(this->statusBar());

    d->stack->setCurrentWidget(d->browserArea);

    d->shiftToWelcomeAreaAction->setChecked(false);
    d->shiftToBrowserAreaAction->setChecked(true);
    d->shiftToViewerAreaAction->setChecked(false);
    d->shiftToDocumentationAreaAction->setChecked(false);
}

void medMainWindow::switchToViewerArea(void)
{
    d->welcomeArea->setdw(this->statusBar());
    d->browserArea->setdw(this->statusBar());
    d->viewerArea->setup(this->statusBar());
    d->documentationArea->setdw(this->statusBar());

    d->stack->setCurrentWidget(d->viewerArea);

    d->shiftToWelcomeAreaAction->setChecked(false);
    d->shiftToBrowserAreaAction->setChecked(false);
    d->shiftToViewerAreaAction->setChecked(true);
    d->shiftToDocumentationAreaAction->setChecked(false);
}

void medMainWindow::switchToDocumentationArea(void)
{
    d->welcomeArea->setdw(this->statusBar());
    d->browserArea->setdw(this->statusBar());
    d->viewerArea->setdw(this->statusBar());
    d->documentationArea->setup(this->statusBar());

    d->stack->setCurrentWidget(d->documentationArea);

    d->shiftToWelcomeAreaAction->setChecked(false);
    d->shiftToBrowserAreaAction->setChecked(false);
    d->shiftToViewerAreaAction->setChecked(false);
    d->shiftToDocumentationAreaAction->setChecked(true);
}

void medMainWindow::onPatientDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
      return;

    d->viewerArea->setPatientIndex(index.row()+1);
    d->viewerArea->update();

    switchToViewerArea();
}

void medMainWindow::onStudyDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
      return;
  
    QModelIndex patientIndex = index.parent();
 
    if (!patientIndex.isValid())
        return;
    
    d->viewerArea->setPatientIndex(patientIndex.row()+1);
    d->viewerArea->setStudyIndex(index.row()+1);
    
    switchToViewerArea();
}

void medMainWindow::onSeriesDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
      return;
    
    QModelIndex studyIndex = index.parent();
    
    if (!studyIndex.isValid())
        return;
    
    QModelIndex patientIndex = studyIndex.parent();
    
    if (!patientIndex.isValid())
        return;
    
    d->viewerArea->setPatientIndex(patientIndex.row()+1);
    d->viewerArea->setStudyIndex(studyIndex.row()+1);

    switchToViewerArea();
    this->update();
    
    d->viewerArea->setSeriesIndex(index.row()+1);
}

void medMainWindow::onConfigurationTriggered(QAction *action)
{
    medViewerConfigurator::instance()->setConfiguration(action->text());
}

void medMainWindow::open(const QString& file)
{
    d->viewerArea->open(file);

    this->switchToViewerArea();
}

void medMainWindow::closeEvent(QCloseEvent *event)
{
    this->writeSettings();
    
    delete medDatabaseController::instance();
}
