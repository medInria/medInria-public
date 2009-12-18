/* medMainWindow.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:48:07 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Nov 12 09:55:04 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 182
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medAdminArea.h"
#include "medBrowserArea.h"
#include "medMainWindow.h"
#include "medViewerArea.h"
#include "medWelcomeArea.h"

#include <dtkScript/dtkScriptInterpreter.h>
#include <dtkScript/dtkScriptInterpreterPool.h>
#include <dtkScript/dtkScriptInterpreterPython.h>
#include <dtkScript/dtkScriptInterpreterTcl.h>

#include <medGui/medWorkspaceShifter.h>
//#include <medGui/medWorkspaceSwitcher.h>

#include <medSql/medDatabaseController.h>
#include <medSql/medDatabaseView.h>
#include <medSql/medDatabaseModel.h>
#include <medSql/medDatabaseItem.h>

#include <QtGui>

class medMainWindowPrivate
{
public:
    QStackedWidget *stack;

    medAdminArea   *adminArea;
    medWelcomeArea *welcomeArea;
    medBrowserArea *browserArea;
    medViewerArea  *viewerArea;

    QToolBar *toolBar;

    QAction *switchToWelcomeAreaAction;
    QAction *switchToBrowserAreaAction;
    QAction *switchToViewerAreaAction;

    medWorkspaceShifterAction *shiftToWelcomeAreaAction;
    medWorkspaceShifterAction *shiftToBrowserAreaAction;
    medWorkspaceShifterAction *shiftToViewerAreaAction;
};

extern "C" int init_core(void);                  // -- Initialization core layer python wrapped functions
extern "C" int Core_Init(Tcl_Interp *interp);    // -- Initialization core layer tcl    wrapped functions

medMainWindow::medMainWindow(QWidget *parent) : QMainWindow(parent), d(new medMainWindowPrivate)
{
    // Setting up database connection

    if(!medDatabaseController::instance()->createConnection())
        qDebug() << "Unable to create a connection to the database";

    // Setting up widgets

    d->adminArea   = new medAdminArea(this);
    d->welcomeArea = new medWelcomeArea(this);
    d->browserArea = new medBrowserArea(this);
    d->viewerArea  = new medViewerArea(this);

    d->welcomeArea->setObjectName("Welcome");
    d->browserArea->setObjectName("Browser");
    d->viewerArea->setObjectName("Viewer");

    connect(d->welcomeArea, SIGNAL(accepted()), this, SLOT(switchToAdminArea()));
    connect(d->adminArea, SIGNAL(accepted()), this, SLOT(switchToWelcomeArea()));
    connect(d->adminArea, SIGNAL(rejected()), this, SLOT(switchToWelcomeArea()));

    d->stack = new QStackedWidget(this);
    d->stack->addWidget(d->adminArea);
    d->stack->addWidget(d->welcomeArea);
    d->stack->addWidget(d->browserArea);
    d->stack->addWidget(d->viewerArea);

    d->switchToWelcomeAreaAction = new QAction(this);
    d->switchToBrowserAreaAction = new QAction(this);
    d->switchToViewerAreaAction  = new QAction(this);

#if defined(Q_WS_MAC)
    d->switchToWelcomeAreaAction->setShortcut(Qt::MetaModifier + Qt::Key_1);
    d->switchToBrowserAreaAction->setShortcut(Qt::MetaModifier + Qt::Key_2);
    d->switchToViewerAreaAction->setShortcut(Qt::MetaModifier + Qt::Key_3);
#else
    d->switchToWelcomeAreaAction->setShortcut(Qt::ControlModifier + Qt::Key_1);
    d->switchToBrowserAreaAction->setShortcut(Qt::ControlModifier + Qt::Key_2);
    d->switchToViewerAreaAction->setShortcut(Qt::ControlModifier + Qt::Key_3);
#endif

    if(qApp->arguments().contains("--no-fullscreen")) {

        d->switchToWelcomeAreaAction->setEnabled(false);
        d->switchToWelcomeAreaAction->setText("Welcome");
        d->switchToWelcomeAreaAction->setToolTip("Switch to the welcome area (Ctrl+1)");
        d->switchToWelcomeAreaAction->setIcon(QIcon(":/icons/widget.tiff"));

        d->switchToBrowserAreaAction->setEnabled(true);
        d->switchToBrowserAreaAction->setText("Browser");
        d->switchToBrowserAreaAction->setToolTip("Switch to the borwser area (Ctrl+2)");
        d->switchToBrowserAreaAction->setIcon(QIcon(":/icons/widget.tiff"));

        d->switchToViewerAreaAction->setEnabled(true);
        d->switchToViewerAreaAction->setText("Viewer");
        d->switchToViewerAreaAction->setToolTip("Switch to the viewer area (Ctrl+3)");
        d->switchToViewerAreaAction->setIcon(QIcon(":/icons/widget.tiff"));
        
        d->toolBar = this->addToolBar("Areas");
#ifdef Q_WS_MAC
        d->toolBar->setStyle(QStyleFactory::create("macintosh"));
        d->toolBar->setStyleSheet("*:enabled { color: black; } *:disabled { color: gray; }");
#endif
        d->toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        d->toolBar->setIconSize(QSize(32, 32));
        d->toolBar->addAction(d->switchToWelcomeAreaAction);
        d->toolBar->addAction(d->switchToBrowserAreaAction);
        d->toolBar->addAction(d->switchToViewerAreaAction);
        this->setUnifiedTitleAndToolBarOnMac(true);
    }

    connect(d->switchToWelcomeAreaAction, SIGNAL(triggered()), this, SLOT(switchToWelcomeArea()));
    connect(d->switchToBrowserAreaAction, SIGNAL(triggered()), this, SLOT(switchToBrowserArea()));
    connect(d->switchToViewerAreaAction,  SIGNAL(triggered()), this, SLOT(switchToViewerArea()));

    connect(d->browserArea->view(), SIGNAL(patientDoubleClicked(const QModelIndex&)), this, SLOT(onPatientDoubleClicked (const QModelIndex&)));
    connect(d->browserArea->view(), SIGNAL(studyDoubleClicked(const QModelIndex&)), this, SLOT(onStudyDoubleClicked (const QModelIndex&)));
    connect(d->browserArea->view(), SIGNAL(seriesDoubleClicked(const QModelIndex&)), this, SLOT(onSeriesDoubleClicked (const QModelIndex&)));

    this->addAction(d->switchToWelcomeAreaAction);
    this->addAction(d->switchToBrowserAreaAction);
    this->addAction(d->switchToViewerAreaAction);

    this->setWindowTitle("medular");
    this->setCentralWidget(d->stack);
    this->readSettings();

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
    dtkScriptInterpreterPythonModuleManager::instance()->registerCommand(
        "deviceFactory  = core.dtkAbstractDeviceFactory.instance()"
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

//    d->switcher = new medWorkspaceSwitcher(this);
//    d->switcher->addWorkspace(d->welcomeArea);
//    d->switcher->addWorkspace(d->browserArea);
//    d->switcher->addWorkspace(d->viewerArea);
//
//    connect(d->switcher, SIGNAL(triggered(int)), this, SLOT(switchToArea(int)));

    d->shiftToWelcomeAreaAction = new medWorkspaceShifterAction("Welcome");
    d->shiftToBrowserAreaAction = new medWorkspaceShifterAction("Browser");
    d->shiftToViewerAreaAction  = new medWorkspaceShifterAction("Viewer");

    d->shiftToWelcomeAreaAction->setChecked(true);

    connect(d->shiftToWelcomeAreaAction, SIGNAL(triggered()), this, SLOT(switchToWelcomeArea()));
    connect(d->shiftToBrowserAreaAction, SIGNAL(triggered()), this, SLOT(switchToBrowserArea()));
    connect(d->shiftToViewerAreaAction,  SIGNAL(triggered()), this, SLOT(switchToViewerArea()));

    medWorkspaceShifter *shifter = new medWorkspaceShifter(this);
    shifter->addAction(d->shiftToWelcomeAreaAction);
    shifter->addAction(d->shiftToBrowserAreaAction);
    shifter->addAction(d->shiftToViewerAreaAction);

    this->statusBar()->setSizeGripEnabled(false);
    this->statusBar()->setContentsMargins(5, 0, 5, 0);
    this->statusBar()->setFixedHeight(31);
    this->statusBar()->addPermanentWidget(shifter);

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

void medMainWindow::switchToArea(int index)
{
    switch(index) {
    case 0:
        this->switchToWelcomeArea();
        break;
    case 1:
        this->switchToBrowserArea();
        break;
    case 2:
        this->switchToViewerArea();
        break;
    default:
        break;
    }
}

void medMainWindow::switchToAdminArea(void)
{
    d->stack->setCurrentWidget(d->adminArea);

    d->switchToWelcomeAreaAction->setEnabled(false);
    d->switchToBrowserAreaAction->setEnabled(true);
    d->switchToViewerAreaAction->setEnabled(true);

    d->shiftToWelcomeAreaAction->setChecked(true);
    d->shiftToBrowserAreaAction->setChecked(false);
    d->shiftToViewerAreaAction->setChecked(false);

    disconnect(d->switchToWelcomeAreaAction, SIGNAL(triggered()));
       connect(d->switchToWelcomeAreaAction, SIGNAL(triggered()), this, SLOT(switchToAdminArea()));
}

void medMainWindow::switchToWelcomeArea(void)
{
    d->welcomeArea->setup(this->statusBar());
    d->browserArea->setdw(this->statusBar());
    d->viewerArea->setdw(this->statusBar());

    d->stack->setCurrentWidget(d->welcomeArea);

    d->switchToWelcomeAreaAction->setEnabled(false);
    d->switchToBrowserAreaAction->setEnabled(true);
    d->switchToViewerAreaAction->setEnabled(true);

    d->shiftToWelcomeAreaAction->setChecked(true);
    d->shiftToBrowserAreaAction->setChecked(false);
    d->shiftToViewerAreaAction->setChecked(false);

    disconnect(d->switchToWelcomeAreaAction, SIGNAL(triggered()));
       connect(d->switchToWelcomeAreaAction, SIGNAL(triggered()), this, SLOT(switchToWelcomeArea()));
}

void medMainWindow::switchToBrowserArea(void)
{
    d->welcomeArea->setdw(this->statusBar());
    d->browserArea->setup(this->statusBar());
    d->viewerArea->setdw(this->statusBar());

    d->stack->setCurrentWidget(d->browserArea);

    d->switchToWelcomeAreaAction->setEnabled(true);
    d->switchToBrowserAreaAction->setEnabled(false);
    d->switchToViewerAreaAction->setEnabled(true);

    d->shiftToWelcomeAreaAction->setChecked(false);
    d->shiftToBrowserAreaAction->setChecked(true);
    d->shiftToViewerAreaAction->setChecked(false);
}

void medMainWindow::switchToViewerArea(void)
{
    d->welcomeArea->setdw(this->statusBar());
    d->browserArea->setdw(this->statusBar());
    d->viewerArea->setup(this->statusBar());

    d->stack->setCurrentWidget(d->viewerArea);

    d->switchToWelcomeAreaAction->setEnabled(true);
    d->switchToBrowserAreaAction->setEnabled(true);
    d->switchToViewerAreaAction->setEnabled(false);

    d->shiftToWelcomeAreaAction->setChecked(false);
    d->shiftToBrowserAreaAction->setChecked(false);
    d->shiftToViewerAreaAction->setChecked(true);
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

void medMainWindow::closeEvent(QCloseEvent *event)
{
    this->writeSettings();
    
    delete medDatabaseController::instance();
}

//void medMainWindow::keyPressEvent(QKeyEvent *event)
//{
//    switch(event->key()) {
//    case Qt::Key_F1:
//        d->switcher->start();
//        break;
//    case Qt::Key_F2:
//        d->switcher->stop();
//        break;
//    default:
//        QMainWindow::keyPressEvent(event);
//        break;
//    };
//}

//void medMainWindow::resizeEvent(QResizeEvent *event)
//{
//    d->switcher->resize(event->size());
//
//    event->accept();
//}
