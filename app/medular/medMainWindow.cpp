/* medMainWindow.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:48:07 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Fri Sep 25 14:11:16 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 29
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
#include "medWelcomeArea.h"

#include <dtkScript/dtkScriptInterpreter.h>
#include <dtkScript/dtkScriptInterpreterPool.h>
#include <dtkScript/dtkScriptInterpreterPython.h>
#include <dtkScript/dtkScriptInterpreterTcl.h>

#include <medSql/medDatabaseController.h>

#include <QtGui>

class medMainWindowPrivate
{
public:
    QStackedWidget *stack;

    medWelcomeArea *welcomeArea;
    medBrowserArea *browserArea;
    medViewerArea  *viewerArea;

    QAction *switchToWelcomeAreaAction;
    QAction *switchToBrowserAreaAction;
    QAction *switchToViewerAreaAction;
};

extern "C" int init_core(void);                  // -- Initialization core layer python wrapped functions
extern "C" int Core_Init(Tcl_Interp *interp);    // -- Initialization core layer tcl    wrapped functions

medMainWindow::medMainWindow(QWidget *parent) : QMainWindow(parent), d(new medMainWindowPrivate)
{
    // Setting up database connection

    if(!medDatabaseController::instance()->createConnection())
        qDebug() << "Unable to create a connection to the database";

    // Setting up widgets

    d->welcomeArea = new medWelcomeArea(this);
    d->browserArea = new medBrowserArea(this);
    d->viewerArea  = new medViewerArea(this);

    d->stack = new QStackedWidget(this);
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

    connect(d->switchToWelcomeAreaAction, SIGNAL(triggered()), this, SLOT(switchToWelcomeArea()));
    connect(d->switchToBrowserAreaAction, SIGNAL(triggered()), this, SLOT(switchToBrowserArea()));
    connect(d->switchToViewerAreaAction,  SIGNAL(triggered()), this, SLOT(switchToViewerArea()));

    this->addAction(d->switchToWelcomeAreaAction);
    this->addAction(d->switchToBrowserAreaAction);
    this->addAction(d->switchToViewerAreaAction);

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

void medMainWindow::switchToWelcomeArea(void)
{
    d->stack->setCurrentWidget(d->welcomeArea);
}

void medMainWindow::switchToBrowserArea(void)
{
    d->stack->setCurrentWidget(d->browserArea);
}

void medMainWindow::switchToViewerArea(void)
{
    d->stack->setCurrentWidget(d->viewerArea);
}

void medMainWindow::closeEvent(QCloseEvent *event)
{
    this->writeSettings();

    delete medDatabaseController::instance();
}
