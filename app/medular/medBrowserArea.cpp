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
#include <medGui/medStatusPanel.h>
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

class medBrowserAreaPrivate
{
public:
    medToolBoxContainer *toolbox_container;

    medBrowserAreaPreview *preview;
    medDatabaseModel *model;
    medDatabaseView *view;
    medStatusPanel *status;

    QProgressBar *progress;
};

medBrowserArea::medBrowserArea(QWidget *parent) : QWidget(parent), d(new medBrowserAreaPrivate)
{
    d->preview = new medBrowserAreaPreview(this);

    d->model = new medDatabaseModel;

    d->view = new medDatabaseView(this);
    d->view->setModel(d->model);

    QFormLayout *dataInfoLayout = new QFormLayout;
    dataInfoLayout->setContentsMargins(0, 5, 0, 5);
    dataInfoLayout->setSpacing(0);
    dataInfoLayout->setFormAlignment(Qt::AlignHCenter | Qt::AlignTop);
    dataInfoLayout->setLabelAlignment(Qt::AlignRight);
    dataInfoLayout->addRow("Database location:", new QLabel(QString("%1").arg(medDatabaseController::instance()->dataLocation())));
    dataInfoLayout->addRow("Database size:", new QLabel(QString("%1 bytes").arg(QFileInfo(medDatabaseController::instance()->dataLocation()).size())));

    QWidget *databaseInfo = new QWidget(this);
    databaseInfo->setLayout(dataInfoLayout);

    d->progress = new QProgressBar(this);
    d->progress->setRange (0, 100);

    d->status = new medStatusPanel(this);
    d->status->addWidget(new QLabel("Current user: Unknown"));
    d->status->addWidget(d->progress);
    d->status->addWidget(databaseInfo);

    // /////////////////////////////////////////////////////////////////

    QWidget *central = new QWidget(this);
    
    QVBoxLayout *central_layout = new QVBoxLayout(central);
    central_layout->setContentsMargins(0, 0, 0, 0);
    central_layout->setSpacing(10);
    central_layout->addWidget(d->view);
    central_layout->addWidget(d->preview);

    // /////////////////////////////////////////////////////////////////

    medToolBox *databaseSelectorToolBox = new medToolBox(this);
    databaseSelectorToolBox->setTitle("Database selector");

    medToolBox *localDatabaseToolBox = new medToolBox(this);
    localDatabaseToolBox->setTitle("Local database");

    medToolBox *distantDatabaseToolBox = new medToolBox(this);
    distantDatabaseToolBox->setTitle("Distant database");

    d->toolbox_container = new medToolBoxContainer(this);
    d->toolbox_container->setFixedWidth(300);
    d->toolbox_container->addToolBox(databaseSelectorToolBox);
    d->toolbox_container->addToolBox(localDatabaseToolBox);
    d->toolbox_container->addToolBox(distantDatabaseToolBox);

    QWidget *main = new QWidget(this);

    QHBoxLayout *main_layout = new QHBoxLayout(main);
    main_layout->setContentsMargins(0, 0, 0, 0);
    main_layout->setSpacing(10);
    main_layout->addWidget(d->toolbox_container);
    main_layout->addWidget(central);

    // /////////////////////////////////////////////////////////////////

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(10);
    // layout->addWidget(d->view);
    // layout->addWidget(d->preview);
    layout->addWidget(main);
    layout->addWidget(d->status);

    // /////////////////////////////////////////////////////////////////

    connect(d->view, SIGNAL(patientClicked(int)), d->preview, SLOT(onPatientClicked(int)));
    connect(d->view, SIGNAL(studyClicked(int)), d->preview, SLOT(onStudyClicked(int)));
    connect(d->view, SIGNAL(seriesClicked(int)), d->preview, SLOT(onSeriesClicked(int)));
    connect(d->view, SIGNAL(imageClicked(int)), d->preview, SLOT(onImageClicked(int)));

    connect (medDatabaseController::instance(), SIGNAL (importCompleted (int)), d->progress, SLOT (setValue (int)));
	
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
    delete d->status;
    delete d;

    d = NULL;
}

medDatabaseView *medBrowserArea::view(void)
{
    return d->view;
}

medDatabaseModel *medBrowserArea::model(void)
{
    return d->model;
}
