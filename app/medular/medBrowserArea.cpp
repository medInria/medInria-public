/* medBrowserArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 25 12:23:43 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Mar 25 18:43:40 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 290
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

// /////////////////////////////////////////////////////////////////
// medBrowserArea
// /////////////////////////////////////////////////////////////////

class medBrowserAreaPrivate
{
public:
    medToolBoxContainer *toolbox_container;

    medDatabasePreview *preview;
    medDatabaseModel *model;
    medDatabaseView *view;

    QTreeView *filesystem_view;
    QFileSystemModel *filesystem_model;

    QStatusBar *status;

    medProgressionStack *progression_stack;
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

    // Source selector ///////////////////////////////////////////////

    QWidget *database_page = new QWidget(this);

    QPushButton *database_page_expand_all_button = new QPushButton("All", database_page);
    QPushButton *database_page_collapse_all_button = new QPushButton("All", database_page);

    connect(database_page_expand_all_button, SIGNAL(clicked()), d->view, SLOT(expandAll()));
    connect(database_page_collapse_all_button, SIGNAL(clicked()), d->view, SLOT(collapseAll()));

    QFormLayout *database_page_layout = new QFormLayout(database_page);
    database_page_layout->addRow("Expand:", database_page_expand_all_button);
    database_page_layout->addRow("Collapse:", database_page_collapse_all_button);
    database_page_layout->setFormAlignment(Qt::AlignHCenter);

    QWidget *filesystem_page = new QWidget(this);

    QPushButton *filesystem_import_button = new QPushButton("Import", filesystem_page);
    QPushButton *filesystem_export_button = new QPushButton("Export", filesystem_page);

    QFormLayout *filesystem_page_layout = new QFormLayout(filesystem_page);
    filesystem_page_layout->addRow("Current item:", filesystem_import_button);
    filesystem_page_layout->addRow("Current item:", filesystem_export_button);
    filesystem_page_layout->setFormAlignment(Qt::AlignHCenter);

    connect(filesystem_import_button, SIGNAL(clicked()), this, SLOT(onFileSystemImportClicked()));
    connect(filesystem_export_button, SIGNAL(clicked()), this, SLOT(onFileSystemExportClicked()));

    QWidget *pacs_page = new QWidget(this);

    QFormLayout *pacs_page_layout = new QFormLayout(pacs_page);
    pacs_page_layout->addRow("Url:", new QLineEdit(pacs_page));
    pacs_page_layout->addRow("Login:", new QLineEdit(pacs_page));
    pacs_page_layout->addRow("Password:", new QLineEdit(pacs_page));
    pacs_page_layout->addWidget(new QPushButton("Connect", pacs_page));
    pacs_page_layout->setFormAlignment(Qt::AlignHCenter);

    medToolBoxTab *tab = new medToolBoxTab(this);
    tab->addTab(database_page, "Db");
    tab->addTab(filesystem_page, "Fs");
    tab->addTab(pacs_page, "Pc");
    connect(tab, SIGNAL(currentChanged(int)), stack, SLOT(setCurrentIndex(int)));

    medToolBox *sourceSelectorToolBox = new medToolBox(this);
    sourceSelectorToolBox->setTitle("Source selector");
    sourceSelectorToolBox->setWidget(tab);

    // Jobs //////////////////////////////////////////

    d->progression_stack = new medProgressionStack(filesystem_page);

    medToolBox *jobsToolBox = new medToolBox(this);
    jobsToolBox->setTitle("Jobs");
    jobsToolBox->setWidget(d->progression_stack);
    jobsToolBox->setVisible(false);

    connect(d->progression_stack, SIGNAL(shown()), jobsToolBox, SLOT(show()));
    connect(d->progression_stack, SIGNAL(hidden()), jobsToolBox, SLOT(hide()));

    // Toolbox container /////////////////////////////////////////////

    d->toolbox_container = new medToolBoxContainer(this);
    d->toolbox_container->setFixedWidth(300);
    d->toolbox_container->addToolBox(sourceSelectorToolBox);
    d->toolbox_container->addToolBox(jobsToolBox);

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

    connect(importer, SIGNAL(progressed(int)), d->progression_stack, SLOT(setProgress(int)));
    connect(importer, SIGNAL(done()), this, SLOT(onFileImported()));

    d->progression_stack->setLabel(importer, info.baseName());

    QThreadPool::globalInstance()->start(importer);
}

void medBrowserArea::onFileSystemExportClicked(void)
{
    medDatabaseExporter *exporter = new medDatabaseExporter;

    connect(exporter, SIGNAL(progressed(int)), d->progression_stack, SLOT(setProgress(int)));

    QThreadPool::globalInstance()->start(exporter);
}

void medBrowserArea::onFileImported(void)
{
    medDatabaseController::instance()->import("");

    d->preview->reset();
    d->preview->init();
    d->preview->update();
}
