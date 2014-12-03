/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medDatabaseCompactWidget.h"

#include <medDatabaseView.h>
#include <medDatabasePreview.h>

#include <medDataIndex.h>


#include <QVBoxLayout>


class medDatabaseCompactWidgetPrivate
{
public:
    medDatabasePreview *preview;
    medDatabaseView *view;
    QVBoxLayout *layout;
};


medDatabaseCompactWidget::medDatabaseCompactWidget(QWidget *parent): d(new medDatabaseCompactWidgetPrivate)
{
    this->setContentsMargins(0,0,0,0);

    d->layout = new QVBoxLayout(this);
    this->setLayout(d->layout);
    d->layout->setContentsMargins(0,0,0,0);
    d->layout->setSpacing(0);
}


medDatabaseCompactWidget::~medDatabaseCompactWidget()
{
    delete d;
    d = NULL;
}


void medDatabaseCompactWidget::resizeEvent(QResizeEvent *event)
{
    delete d->layout;
    d->layout = new QVBoxLayout(this);
    d->layout->setContentsMargins(0,0,0,0);
    d->layout->setSpacing(0);

    this->setLayout(d->layout);

    d->layout->addWidget(d->view, 0);
    d->layout->addWidget(d->preview, 0, Qt::AlignBottom);
    d->layout->addWidget(d->preview->label(), 0);

    QWidget::resizeEvent(event);
}



void
medDatabaseCompactWidget::setViewAndPreview(medDatabaseView *view, medDatabasePreview *preview)
{
    d->view = view;
    d->preview = preview;
    d->layout->addWidget(d->view, 0);
    d->layout->addWidget(d->preview, 0, Qt::AlignBottom);
    d->layout->addWidget(d->preview->label(), 0);
}
