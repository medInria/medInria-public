/* medBrowserArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 25 12:23:43 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Fri Oct  9 15:56:40 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 139
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

#include <medGui/medPatientPreview.h>
#include <medGui/medStudyPreview.h>
#include <medGui/medSeriesPreview.h>
#include <medGui/medImagePreview.h>

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
    qDebug() << __func__ << id;

    d->stack->setCurrentWidget(d->study_preview);
}

void medBrowserAreaPreview::onSeriesClicked(int id)
{
    qDebug() << __func__ << id;

    d->stack->setCurrentWidget(d->series_preview);
}

void medBrowserAreaPreview::onImageClicked(int id)
{
    qDebug() << __func__ << id;

    d->stack->setCurrentWidget(d->image_preview);
}

// /////////////////////////////////////////////////////////////////
// medBrowserArea
// /////////////////////////////////////////////////////////////////

class medBrowserAreaPrivate
{
public:
    medBrowserAreaPreview *preview;

    medDatabaseModel *model;
    medDatabaseView *view;
};

medBrowserArea::medBrowserArea(QWidget *parent) : QWidget(parent), d(new medBrowserAreaPrivate)
{
    d->preview = new medBrowserAreaPreview(this);

    d->model = new medDatabaseModel;

    d->view = new medDatabaseView(this);
    d->view->setModel(d->model);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d->view);
    layout->addWidget(d->preview);

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

medDatabaseView *medBrowserArea::databaseView (void)
{
    return d->view;
}

medDatabaseModel *medBrowserArea::databaseModel (void)
{
    return d->model;
}
