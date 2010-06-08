/* medBrowserArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 25 12:23:43 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Jun  9 00:27:43 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 341
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medBrowserArea.h"

#include <QtGui>

#include <medSql/medDatabaseController.h>
#include <medSql/medDatabaseExporter.h>
#include <medSql/medDatabaseImporter.h>
#include <medSql/medDatabaseModel.h>
#include <medSql/medDatabaseView.h>
#include <medSql/medDatabasePreview.h>

#include <medGui/medProgressionStack.h>
#include <medGui/medToolBox.h>
#include <medGui/medToolBoxContainer.h>
#include <medGui/medToolBoxJobs.h>
#include <medGui/medToolBoxSource.h>

// /////////////////////////////////////////////////////////////////
// medBrowserArea
// /////////////////////////////////////////////////////////////////

class medBrowserAreaPrivate
{
public:
    medToolBoxContainer *toolbox_container;
    medToolBoxJobs *toolbox_jobs;
    medToolBoxSource *toolbox_source;

    medDatabasePreview *preview;
    medDatabaseModel *model;
    medDatabaseView *view;

    QTreeView *filesystem_view;
    QFileSystemModel *filesystem_model;

    QStatusBar *status;
};

medBrowserArea::medBrowserArea(QWidget *parent) : QWidget(parent), d(new medBrowserAreaPrivate)
{
    d->preview = new medDatabasePreview(this);

    d->model = new medDatabaseModel;

    d->view = new medDatabaseView(this);
    d->view->setModel(d->model);

    // connect(d->preview, SIGNAL(patientClicked(int)), d->view, SLOT(onPatientClicked(int)));
    // connect(d->preview, SIGNAL(studyClicked(int)), d->view, SLOT(onStudyClicked(int)));
    // connect(d->preview, SIGNAL(seriesClicked(int)), d->view, SLOT(onSeriesClicked(int)));
    // connect(d->preview, SIGNAL(imageClicked(int)), d->view, SLOT(onImageClicked(int)));

    connect(d->view, SIGNAL(patientClicked(int)), d->preview, SLOT(onPatientClicked(int)));
    connect(d->view, SIGNAL(seriesClicked(int)), d->preview, SLOT(onSeriesClicked(int)));

    // Database widget /////////////////////////////////////////////////

    QWidget *database_widget = new QWidget(this);

    QVBoxLayout *database_layout = new QVBoxLayout(database_widget);
    database_layout->setContentsMargins(0, 0, 0, 0);
    database_layout->setSpacing(0);
    database_layout->addWidget(d->view);
    database_layout->addWidget(d->preview);

    // Filesystem widget ///////////////////////////////////////////////

    d->filesystem_model = new QFileSystemModel;
    d->filesystem_model->setRootPath(QDir::rootPath());

    d->filesystem_view = new QTreeView(this);
    d->filesystem_view->setFrameStyle(QFrame::NoFrame);
    d->filesystem_view->setAttribute(Qt::WA_MacShowFocusRect, false);
    d->filesystem_view->setUniformRowHeights(true);
    d->filesystem_view->setAlternatingRowColors(true);
    d->filesystem_view->setSortingEnabled(true);
    d->filesystem_view->setModel(d->filesystem_model);
    d->filesystem_view->setCurrentIndex(d->filesystem_model->index(QDir::homePath()));
    d->filesystem_view->setExpanded(d->filesystem_view->currentIndex(), true);
    d->filesystem_view->header()->setResizeMode(QHeaderView::Interactive);

    // Pacs widget ///////////////////////////////////////////////

    QTreeView *pacs_widget = new QTreeView(this);
    pacs_widget->setFrameStyle(QFrame::NoFrame);
    pacs_widget->setAttribute(Qt::WA_MacShowFocusRect, false);
    pacs_widget->setUniformRowHeights(true);
    pacs_widget->setAlternatingRowColors(true);
    pacs_widget->setSortingEnabled(true);

    // /////////////////////////////////////////////////////////////////

    QStackedWidget *stack = new QStackedWidget(this);
    stack->addWidget(database_widget);
    stack->addWidget(d->filesystem_view);
    stack->addWidget(pacs_widget);

    // Source toolbox ///////////////////////////////////////////////

    d->toolbox_source = new medToolBoxSource(this);

    connect(d->toolbox_source, SIGNAL(expandAll()), d->view, SLOT(expandAll()));
    connect(d->toolbox_source, SIGNAL(collapseAll()), d->view, SLOT(collapseAll()));
    connect(d->toolbox_source, SIGNAL(importCurrent()), this, SLOT(onFileSystemImportClicked()));
    connect(d->toolbox_source, SIGNAL(exportCurrent()), this, SLOT(onFileSystemExportClicked()));
    connect(d->toolbox_source, SIGNAL(viewCurrent()), this, SLOT(onFileSystemViewClicked()));
    connect(d->toolbox_source, SIGNAL(indexChanged(int)), stack, SLOT(setCurrentIndex(int)));

    // Jobs //////////////////////////////////////////

    d->toolbox_jobs = new medToolBoxJobs(this);
    d->toolbox_jobs->setVisible(false);

    // Toolbox container /////////////////////////////////////////////

    d->toolbox_container = new medToolBoxContainer(this);
    d->toolbox_container->setFixedWidth(300);
    d->toolbox_container->addToolBox(d->toolbox_source);
    d->toolbox_container->addToolBox(d->toolbox_jobs);

    // Layout /////////////////////////////////////////////

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d->toolbox_container);
    layout->addWidget(stack);
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
    QFileInfo info(d->filesystem_model->filePath(d->filesystem_view->currentIndex()));

    medDatabaseImporter *importer = new medDatabaseImporter(info.absoluteFilePath());

    connect(importer, SIGNAL(progressed(int)), d->toolbox_jobs->stack(), SLOT(setProgress(int)));
    connect(importer, SIGNAL(done()), this, SLOT(onFileImported()));

    d->toolbox_jobs->stack()->setLabel(importer, info.baseName());

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
    QFileInfo info(d->filesystem_model->filePath(d->filesystem_view->currentIndex()));

    emit open(info.absoluteFilePath());
}

void medBrowserArea::onFileImported(void)
{
    medDatabaseController::instance()->import("");

    d->preview->reset();
    d->preview->init();
    d->preview->update();
}
