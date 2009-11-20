/* medBrowserArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 25 12:23:43 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Nov  3 09:16:04 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 232
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
#include <medSql/medDatabaseModel.h>
#include <medSql/medDatabaseView.h>

#include <medGui/medImagePreview.h>
#include <medGui/medPatientPreview.h>
#include <medGui/medStudyPreview.h>
#include <medGui/medSeriesPreview.h>
#include <medGui/medToolBox.h>
#include <medGui/medToolBoxContainer.h>

// /////////////////////////////////////////////////////////////////
// medBrowserAreaPreview
// /////////////////////////////////////////////////////////////////

class medBrowserAreaPreviewPrivate
{
public:
    QStackedWidget *stack;

    medPatientPreview *patient_preview;
    medStudyPreview *study_preview;
    medSeriesPreview *series_preview;
    medImagePreview *image_preview;
};

medBrowserAreaPreview::medBrowserAreaPreview(QWidget *parent) : QWidget(parent), d(new medBrowserAreaPreviewPrivate)
{
    d->patient_preview = new medPatientPreview(this);
    d->study_preview = new medStudyPreview(this);
    d->series_preview = new medSeriesPreview(this);
    d->image_preview = new medImagePreview(this);

    d->stack = new QStackedWidget(this);
    d->stack->addWidget(d->patient_preview);
    d->stack->addWidget(d->study_preview);
    d->stack->addWidget(d->series_preview);
    d->stack->addWidget(d->image_preview);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d->stack);

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
}

medBrowserAreaPreview::~medBrowserAreaPreview(void)
{
    delete d->patient_preview;
    delete d->study_preview;
    delete d->series_preview;
    delete d->image_preview;
    delete d->stack;
    delete d;

    d = NULL;
}

QSize medBrowserAreaPreview::sizeHint(void) const
{
    return d->patient_preview->sizeHint();
}

void medBrowserAreaPreview::onPatientClicked(int id)
{
    d->patient_preview->setup(id);
    
    d->stack->setCurrentWidget(d->patient_preview);
}

void medBrowserAreaPreview::onStudyClicked(int id)
{
    d->study_preview->setup(id);

    d->stack->setCurrentWidget(d->study_preview);
}

void medBrowserAreaPreview::onSeriesClicked(int id)
{
    d->stack->setCurrentWidget(d->series_preview);
}

void medBrowserAreaPreview::onImageClicked(int id)
{
    d->stack->setCurrentWidget(d->image_preview);
}

// /////////////////////////////////////////////////////////////////
// medBrowserArea
// /////////////////////////////////////////////////////////////////

//class medTabBar : public QTabBar
//{
//public:
//    medTabBar(QWidget *parent) : QTabBar(parent)
//    {
//        this->setDrawBase(true);
//    }
//
////    void paintEvent(QPaintEvent *event)
////    {
////        QPainter painter(this);
////        painter.fillRect(event->rect(), Qt::green);
////        painter.end();
////
//////        QTabBar::paintEvent(event);
////    }
//};

class medBrowserAreaPrivate
{
public:
    medToolBoxContainer *toolbox_container;

    medBrowserAreaPreview *preview;
    medDatabaseModel *model;
    medDatabaseView *view;

    QTreeView *filesystem_view;
    QDirModel *filesystem_model;

    QProgressBar *progress;
    QStatusBar *status;
};

medBrowserArea::medBrowserArea(QWidget *parent) : QWidget(parent), d(new medBrowserAreaPrivate)
{
    d->preview = new medBrowserAreaPreview(this);

    d->model = new medDatabaseModel;

    d->view = new medDatabaseView(this);
    d->view->setModel(d->model);

    d->progress = new QProgressBar(this);
    d->progress->setTextVisible(true);

    // Database widget /////////////////////////////////////////////////

    QWidget *database_widget = new QWidget(this);
    
    QVBoxLayout *database_layout = new QVBoxLayout(database_widget);
    database_layout->setContentsMargins(0, 0, 0, 0);
    database_layout->setSpacing(0);
    database_layout->addWidget(d->view);
    database_layout->addWidget(d->preview);

    // Filesystem widget ///////////////////////////////////////////////

    d->filesystem_model = new QDirModel;

    d->filesystem_view = new QTreeView(this);
    d->filesystem_view->setFrameStyle(QFrame::NoFrame);
    d->filesystem_view->setAttribute(Qt::WA_MacShowFocusRect, false);
    d->filesystem_view->setUniformRowHeights(true);
    d->filesystem_view->setAlternatingRowColors(true);
    d->filesystem_view->setSortingEnabled(true);
    d->filesystem_view->setModel(d->filesystem_model);
    d->filesystem_view->setRootIndex(d->filesystem_model->index(QDir::homePath()));

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
    tab->addTab(new QWidget, "Db");
    tab->addTab(filesystem_page, "Fs");
    tab->addTab(pacs_page, "Pc");
    connect(tab, SIGNAL(currentChanged(int)), stack, SLOT(setCurrentIndex(int)));

    medToolBox *sourceSelectorToolBox = new medToolBox(this);
    sourceSelectorToolBox->setTitle("Source selector");
    sourceSelectorToolBox->setWidget(tab);

    d->toolbox_container = new medToolBoxContainer(this);
    d->toolbox_container->setFixedWidth(300);
    d->toolbox_container->addToolBox(sourceSelectorToolBox);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d->toolbox_container);
    layout->addWidget(stack);

    connect(d->view, SIGNAL(patientClicked(int)), d->preview, SLOT(onPatientClicked(int)));
    connect(d->view, SIGNAL(studyClicked(int)), d->preview, SLOT(onStudyClicked(int)));
    connect(d->view, SIGNAL(seriesClicked(int)), d->preview, SLOT(onSeriesClicked(int)));
    connect(d->view, SIGNAL(imageClicked(int)), d->preview, SLOT(onImageClicked(int)));

    // connect(d->view, SIGNAL(patientDoubleClicked(int)), this, SLOT(onPatientDoubleClicked(int)));
    // connect(d->view, SIGNAL(studyDoubleClicked(int)), this, SLOT(onStudyDoubleClicked(int)));
    // connect(d->view, SIGNAL(seriesDoubleClicked(int)), this, SLOT(onSeriesDoubleClicked(int)));
    // connect(d->view, SIGNAL(imageDoubleClicked(int)), this, SLOT(onImageDoubleClicked(int)));
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
    d->status->addPermanentWidget(d->progress);

    d->progress->show();
}

void medBrowserArea::setdw(QStatusBar *status)
{
    d->status = status;
    d->status->removeWidget(d->progress);

    d->progress->hide();
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

    if(info.isDir())
        medDatabaseController::instance()->import(info.absoluteFilePath());
    else
        d->status->showMessage("Import only support dicom directories so far.", 10000);
}

void medBrowserArea::onFileSystemExportClicked(void)
{

}
