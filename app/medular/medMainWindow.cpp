/* medMainWindow.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:48:07 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Mon Oct 12 18:17:54 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 94
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
#include "medViewerAreaViewContainer.h"
#include "medWelcomeArea.h"

#include <dtkScript/dtkScriptInterpreter.h>
#include <dtkScript/dtkScriptInterpreterPool.h>
#include <dtkScript/dtkScriptInterpreterPython.h>
#include <dtkScript/dtkScriptInterpreterTcl.h>

#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataReader.h>

#include <medSql/medDatabaseController.h>
#include <medSql/medDatabaseView.h>
#include <medSql/medDatabaseModel.h>
#include <medSql/medDatabaseItem.h>

#include <QtGui>

class medMainWindowPrivate
{
public:
    QStackedWidget *stack;

    medWelcomeArea *welcomeArea;
    medBrowserArea *browserArea;
    medViewerArea  *viewerArea;

    QToolBar *toolBar;

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

    if(!(qApp->arguments().contains("--fullscreen"))) {

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

    connect (d->browserArea->databaseView(), SIGNAL (studyDoubleClicked (const QModelIndex&)), this, SLOT (onStudyDoubleClicked (const QModelIndex&)));
    connect (d->browserArea->databaseView(), SIGNAL (seriesDoubleClicked (const QModelIndex&)), this, SLOT (onSeriesDoubleClicked (const QModelIndex&)));

    this->addAction(d->switchToWelcomeAreaAction);
    this->addAction(d->switchToBrowserAreaAction);
    this->addAction(d->switchToViewerAreaAction);

    this->setWindowTitle("Medular");
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

    d->switchToWelcomeAreaAction->setEnabled(false);
    d->switchToBrowserAreaAction->setEnabled(true);
    d->switchToViewerAreaAction->setEnabled(true);
}

void medMainWindow::switchToBrowserArea(void)
{
    d->stack->setCurrentWidget(d->browserArea);

    d->switchToWelcomeAreaAction->setEnabled(true);
    d->switchToBrowserAreaAction->setEnabled(false);
    d->switchToViewerAreaAction->setEnabled(true);
}

void medMainWindow::switchToViewerArea(void)
{
    d->stack->setCurrentWidget(d->viewerArea);

    d->switchToWelcomeAreaAction->setEnabled(true);
    d->switchToBrowserAreaAction->setEnabled(true);
    d->switchToViewerAreaAction->setEnabled(false);
}

void medMainWindow::onSeriesDoubleClicked (const QModelIndex &index)
{
    medViewerAreaViewContainer* current = d->viewerArea->current();
    if (!current) {
      return;
    }

    dtkAbstractView *view = dtkAbstractViewFactory::instance()->create ("v3dView2D");
    if (!view)
      return;

    current->addWidget (view->widget(), 0, 0);

    medDatabaseItem *dataItem = static_cast<medDatabaseItem*>(index.internalPointer());

    QStringList filenames;
    for (int i=0; i<dataItem->childCount(); i++)
      filenames << dataItem->child(i)->value (2).toString();

    dtkAbstractData *imData = 0;

    typedef dtkAbstractDataFactory::dtkAbstractDataTypeHandler dtkAbstractDataTypeHandler;
    
    QList<dtkAbstractDataTypeHandler> readers = dtkAbstractDataFactory::instance()->readers();
    for (int i=0; i<readers.size(); i++) {
      dtkAbstractDataReader* dataReader = dtkAbstractDataFactory::instance()->reader(readers[i].first, readers[i].second);
      if (dataReader->canRead(filenames)) {
	dataReader->read(filenames);
	imData = dataReader->data();
	delete dataReader;
	break;
      }
    }

    if (imData) {
      view->setData (imData);
      view->reset();
    }

    switchToViewerArea();
}

void medMainWindow::onStudyDoubleClicked (const QModelIndex &index)
{
    medViewerAreaViewContainer* current = d->viewerArea->current();
    if (!current) {
      return;
    }


    medDatabaseItem *studyItem = static_cast<medDatabaseItem*>(index.internalPointer());
    int seCount = studyItem->childCount();

    for (int i=0; i<seCount; i++) {

      dtkAbstractView *view = dtkAbstractViewFactory::instance()->create ("v3dView2D");
      if (!view)
	return;
      
      current->addWidget (view->widget(), 0, i);

      
      medDatabaseItem *seriesItem = studyItem->child (i);

      QStringList filenames;
      for (int i=0; i<seriesItem->childCount(); i++)
	filenames << seriesItem->child(i)->value (2).toString();

      dtkAbstractData *imData = 0;
      
      typedef dtkAbstractDataFactory::dtkAbstractDataTypeHandler dtkAbstractDataTypeHandler;
      
      QList<dtkAbstractDataTypeHandler> readers = dtkAbstractDataFactory::instance()->readers();
      for (int i=0; i<readers.size(); i++) {
	dtkAbstractDataReader* dataReader = dtkAbstractDataFactory::instance()->reader(readers[i].first, readers[i].second);
	if (dataReader->canRead(filenames)) {
	  dataReader->read(filenames);
	  imData = dataReader->data();
	  delete dataReader;
	  break;
	}
      }
      
      if (imData) {
	view->setData (imData);
	view->reset();
      }
    }

    switchToViewerArea();
}

void medMainWindow::closeEvent(QCloseEvent *event)
{
    this->writeSettings();

    delete medDatabaseController::instance();
}
