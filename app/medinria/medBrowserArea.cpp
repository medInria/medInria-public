/* medBrowserArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 25 12:23:43 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 11:15:55 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 461
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medBrowserArea.h"
#include "medBrowserArea_p.h"

#include "medBrowserToolBoxPacsHost.h"
#include "medBrowserToolBoxPacsNodes.h"
#include "medBrowserToolBoxPacsSearch.h"
#include "medBrowserToolBoxSource.h"

#include <QtGui>

#include <dtkCore/dtkGlobal.h>
#include <dtkGui/dtkFinder.h>

#include <medCore/medMessageController.h>
#include <medCore/medJobManager.h>

#include <medSql/medDatabaseController.h>
#include <medSql/medDatabaseExporter.h>
#include <medSql/medDatabaseImporter.h>
#include <medSql/medDatabaseModel.h>
#include <medSql/medDatabaseView.h>
#include <medSql/medDatabasePreview.h>

#include <medGui/medProgressionStack.h>
#include <medGui/medToolBox.h>
#include <medGui/medToolBoxContainer.h>
#include <medGui/medPacsSelector.h>
#include <medGui/medBrowserToolBoxJobs.h>

#include <medPacs/medPacsWidget.h>
#include <medPacs/medPacsMover.h>

// /////////////////////////////////////////////////////////////////
// medBrowserArea
// /////////////////////////////////////////////////////////////////

medBrowserArea::medBrowserArea(QWidget *parent) : QWidget(parent), d(new medBrowserAreaPrivate)
{
    d->preview = new medDatabasePreview(this);

    d->model = new medDatabaseModel;

    d->view = new medDatabaseView(this);
    d->view->setModel(d->model);

    connect(d->view, SIGNAL(patientClicked(int)), d->preview, SLOT(onPatientClicked(int)));
    connect(d->view, SIGNAL(seriesClicked(int)), d->preview, SLOT(onSeriesClicked(int)));
    connect(d->view, SIGNAL(open(const medDataIndex&)), this, SIGNAL(open(const medDataIndex&)));

    // Database widget /////////////////////////////////////////////////

    QWidget *database_widget = new QWidget(this);

    QVBoxLayout *database_layout = new QVBoxLayout(database_widget);
    database_layout->setContentsMargins(0, 0, 0, 0);
    database_layout->setSpacing(0);
    database_layout->addWidget(d->view);
    database_layout->addWidget(d->preview);

    // Filesystem widget ///////////////////////////////////////////////

    d->finder = new dtkFinder(this);
    d->finder->setPath(QDir::homePath());

    d->path = new dtkFinderPathBar(this);
    d->path->setPath(QDir::homePath());
    
    d->toolbar = new dtkFinderToolBar (this);
    d->toolbar->setPath(QDir::currentPath());

    d->side = new dtkFinderSideView(this);
    d->side->setStyleSheet(
        "dtkFinderSideView {"
        "    color: #b2b2b2;"
        "    padding: 5px;"
        "    background: #494949;"
        "    show-decoration-selected: 1;"
        "}"
        ""
        "dtkFinderSideView::item {"
        "    margin-left: 0px;"
        "    border-top-color: transparent;"
        "    border-bottom-color: transparent;"
        "}"
        ""
        "dtkFinderSideView::item:selected {"
        "    border-top: 1px solid #567dbc;"
        "    border-bottom: 1px solid #567dbc;"
        "}"
        ""
        "dtkFinderSideView::item:selected:active{"
        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6ea1f1, stop: 1 #567dbc);"
        "}"
        ""
        "dtkFinderSideView::item:selected:!active {"
        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6b9be8, stop: 1 #577fbf);"
        "}"
        ""
        "dtkFinderSideView::branch:selected {"
        "    border-top: 1px solid #567dbc;"
        "    border-bottom: 1px solid #567dbc;"
        "}"
        ""
        "dtkFinderSideView::branch:selected:active{"
        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6ea1f1, stop: 1 #567dbc);"
        "}"
        ""
        "dtkFinderSideView::branch:selected:!active {"
        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6b9be8, stop: 1 #577fbf);"
        "}");

    QAction *importAction = new QAction("Import", this);
    QAction   *viewAction = new QAction("View", this);

    d->finder->addContextMenuAction(importAction);
    d->finder->addContextMenuAction(viewAction);

    connect(importAction, SIGNAL(triggered()), this, SLOT(onFileSystemImportClicked()));
    connect(  viewAction, SIGNAL(triggered()), this, SLOT(onFileSystemViewClicked()));

    QWidget *filesystem_widget = new QWidget(this);
    
    QHBoxLayout *toolbar_layout = new QHBoxLayout;
    toolbar_layout->setContentsMargins(0, 0, 0, 0);
    toolbar_layout->setSpacing(0);
    toolbar_layout->addWidget  (d->toolbar);
    toolbar_layout->addWidget  (d->path);    

    QVBoxLayout *filesystem_layout = new QVBoxLayout(filesystem_widget);
    filesystem_layout->setContentsMargins(10, 10, 10, 10);
    filesystem_layout->setSpacing(0);
    filesystem_layout->addLayout (toolbar_layout);
    filesystem_layout->addWidget(d->finder);

    connect(d->finder, SIGNAL(changed(QString)), d->path, SLOT(setPath(QString)));
    connect(d->finder, SIGNAL(changed(QString)), d->side, SLOT(setPath(QString)));
    connect(d->finder, SIGNAL(changed(QString)), d->toolbar, SLOT(setPath(QString)));
    
    connect(d->path, SIGNAL(changed(QString)), d->finder, SLOT(setPath(QString)));
    connect(d->path, SIGNAL(changed(QString)), d->side, SLOT(setPath(QString)));
    connect(d->path, SIGNAL(changed(QString)), d->toolbar, SLOT(setPath(QString)));
    
    connect(d->side, SIGNAL(changed(QString)), d->finder, SLOT(setPath(QString)));
    connect(d->side, SIGNAL(changed(QString)), d->path, SLOT(setPath(QString)));
    connect(d->side, SIGNAL(changed(QString)), d->toolbar, SLOT(setPath(QString)));
    
    connect(d->finder, SIGNAL(bookmarked(QString)), d->side, SLOT(addBookmark(QString)));
    
    connect (d->toolbar, SIGNAL(changed(QString)), d->finder, SLOT(setPath(QString)));
    connect (d->toolbar, SIGNAL(changed(QString)), d->path,   SLOT(setPath(QString)));
    
    connect (d->toolbar, SIGNAL(treeView()),       d->finder, SLOT(switchToTreeView()));
    connect (d->toolbar, SIGNAL(listView()),       d->finder, SLOT(switchToListView()));    

    // Pacs widget ///////////////////////////////////////////////

    d->pacs = new medPacsWidget(this);

    connect(d->pacs, SIGNAL(moveList(const QVector<medMoveCommandItem>&)), this, SLOT(onPacsMove(const QVector<medMoveCommandItem>&)));
    connect(d->pacs, SIGNAL(import(QString)), this, SLOT(onFileImport(QString)));

    // /////////////////////////////////////////////////////////////////

    d->stack = new QStackedWidget(this);
    d->stack->addWidget(database_widget);
    d->stack->addWidget(filesystem_widget);
    d->stack->addWidget(d->pacs);

    // Jobs //////////////////////////////////////////

    d->toolbox_jobs = new medBrowserToolBoxJobs(this);
    d->toolbox_jobs->setVisible(false);

    // Toolbox pacs host ///////////////////////////////////////////

    d->toolbox_pacs_host = new medBrowserToolBoxPacsHost(this);
    d->toolbox_pacs_host->setVisible(false);

    // Toolbox pacs nodes //////////////////////////////////////////

    d->toolbox_pacs_nodes = new medBrowserToolBoxPacsNodes(this);
    d->toolbox_pacs_nodes->setVisible(false);
    connect(d->toolbox_pacs_nodes, SIGNAL(echoRequest()), d->pacs, SLOT(onEchoRequest()));
    connect(d->pacs, SIGNAL(echoResponse(QVector<bool>)), d->toolbox_pacs_nodes, SLOT(onEchoResponse(QVector<bool>)));

    // Pacs Selector //////////////////////////////////////////

    d->pacs_selector = new medPacsSelector(this);

    connect(d->toolbox_pacs_nodes, SIGNAL(nodesUpdated()), d->pacs_selector, SLOT(updateList()));
    connect(d->pacs_selector, SIGNAL(selectionChanged(QVector<int>)), d->pacs, SLOT(updateSelectedNodes(QVector<int>)));

    // Toolbox pacs search //////////////////////////////////////////

    d->toolbox_pacs_search = new medBrowserToolBoxPacsSearch(this);
    d->toolbox_pacs_search->setVisible(false);

    connect(d->toolbox_pacs_search, SIGNAL(search(QString)), d->pacs, SLOT(search(QString)));


    // Source toolbox ///////////////////////////////////////////////

    d->toolbox_source = new medBrowserToolBoxSource(this);
    d->toolbox_source->setFileSystemWidget(d->side);
    d->toolbox_source->setPacsWidget(d->pacs_selector);

    connect(d->toolbox_source, SIGNAL(indexChanged(int)), this, SLOT(onSourceIndexChanged(int)));


    // Jobs //////////////////////////////////////////

    d->toolbox_jobs = new medBrowserToolBoxJobs(this);
    d->toolbox_jobs->setVisible(false);


    // Toolbox container /////////////////////////////////////////////

    d->toolbox_container = new medToolBoxContainer(this);
    d->toolbox_container->setFixedWidth(300);
    d->toolbox_container->addToolBox(d->toolbox_source);
    d->toolbox_container->addToolBox(d->toolbox_pacs_host);
    d->toolbox_container->addToolBox(d->toolbox_pacs_nodes);
    d->toolbox_container->addToolBox(d->toolbox_pacs_search);
    d->toolbox_container->addToolBox(d->toolbox_jobs);

    // Layout /////////////////////////////////////////////

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d->stack);
    layout->addWidget(d->toolbox_container);
}

medBrowserArea::~medBrowserArea(void)
{
    delete d->preview;
    delete d->model;
    delete d->view;
    delete d;

    d = NULL;
}

void medBrowserArea::setup(QStatusBar *status)
{
    d->status = status;
}

void medBrowserArea::setdw(QStatusBar *status)
{
    d->status = status;
}

medDatabaseView *medBrowserArea::view(void)
{
    return d->view;
}

medDatabaseModel *medBrowserArea::model(void)
{
    return d->model;
}


void medBrowserArea::onFileSystemImportClicked(void)
{
    QFileInfo info(d->finder->selectedPath());
    this->onFileImport(info.absoluteFilePath());
}

void medBrowserArea::onFileImport(QString path)
{
    qDebug() << path;
    QFileInfo info(path);

    medDatabaseImporter *importer = new medDatabaseImporter(info.absoluteFilePath());
    connect(importer, SIGNAL(success(QObject*)), this, SLOT(onFileImported()), Qt::QueuedConnection);
    d->toolbox_jobs->stack()->AddJobItem(importer, info.baseName());
    medJobManager::instance()->registerJobItem(importer);
    QThreadPool::globalInstance()->start(importer);

}

void medBrowserArea::onFileSystemExportClicked(void)
{
    medDatabaseExporter *exporter = new medDatabaseExporter;

    connect(exporter, SIGNAL(progressed(int)), d->toolbox_jobs->stack(), SLOT(setProgress(int)));

    QThreadPool::globalInstance()->start(exporter);
}

void medBrowserArea::onFileSystemViewClicked(void)
{
    QFileInfo info(d->finder->selectedPath());

    emit open(info.absoluteFilePath());
}

void medBrowserArea::onFileImported(void)
{
    medDatabaseController::instance()->import("");

    d->preview->reset();
    d->preview->init();
    d->preview->update();
}

void medBrowserArea::onSourceIndexChanged(int index)
{
    d->stack->setCurrentIndex(index);

    if(index == 2) {
        d->toolbox_pacs_host->setVisible(true);
        d->toolbox_pacs_nodes->setVisible(true);
        d->toolbox_pacs_search->setVisible(true);
    } else {
        d->toolbox_pacs_host->setVisible(false);
        d->toolbox_pacs_nodes->setVisible(false);
        d->toolbox_pacs_search->setVisible(false);
    }
}

void medBrowserArea::onPacsMove( const QVector<medMoveCommandItem>& cmdList)
{
    medPacsMover* mover = new medPacsMover(cmdList);
    connect(mover, SIGNAL(import(QString)), this, SLOT(onFileImport(QString)));
    d->toolbox_jobs->stack()->AddJobItem(mover, "Retrieving");
    medJobManager::instance()->registerJobItem(mover);
    QThreadPool::globalInstance()->start(mover);    
}

