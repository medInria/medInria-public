/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medDatabaseCompactWidget.h"

#include <medDatabaseView.h>
#include <medDatabasePreview.h>
#include <medDatabaseSearchPanel.h>

#include <medDataIndex.h>


#include <QVBoxLayout>


class medDatabaseCompactWidgetPrivate
{
public:
    medDatabaseSearchPanel *panel;
    medDatabasePreview *preview;
    medDatabaseView *view;
    QVBoxLayout *layout;
    medToolBox* databaseToolBox;
};


medDatabaseCompactWidget::medDatabaseCompactWidget(QWidget *parent): d(new medDatabaseCompactWidgetPrivate)
{
    this->setContentsMargins(0,0,0,0);

    createLayout();

    d->databaseToolBox = nullptr;
}


medDatabaseCompactWidget::~medDatabaseCompactWidget()
{
    delete d;
    d = nullptr;
}


void medDatabaseCompactWidget::resizeEvent(QResizeEvent *event)
{
    delete d->layout;

    createLayout();

    placeDatabaseWidgets();

    QWidget::resizeEvent(event);
}

void medDatabaseCompactWidget::setSearchPanelViewAndPreview(medDatabaseSearchPanel *panel, medDatabaseView *view,
                                                            medDatabasePreview *preview)
{
    d->panel = panel;
    d->view = view;
    connect(d->view, SIGNAL(open(const medDataIndex&)),
            this, SIGNAL(open(const medDataIndex&))
            );

    d->preview = preview;

    createDatabaseToolBox();

    placeDatabaseWidgets();
}

void medDatabaseCompactWidget::createLayout()
{
    d->layout = new QVBoxLayout(this);
    d->layout->setContentsMargins(0,0,0,0);
    d->layout->setSpacing(0);

    this->setLayout(d->layout);
}

void medDatabaseCompactWidget::createDatabaseToolBox()
{
    d->databaseToolBox = new medToolBox();
    d->databaseToolBox->setTitle("Database");
    d->databaseToolBox->addWidget(d->view);
}

void medDatabaseCompactWidget::placeDatabaseWidgets()
{
    d->layout->addWidget(d->panel, 0);
    d->layout->addWidget(d->databaseToolBox, 1);
    d->layout->addWidget(d->preview, 0, Qt::AlignCenter);
    d->layout->addWidget(d->preview->label(), 0);

    int newH = this->height() - d->preview->height() - d->preview->label()->height() - d->panel->height() - 20;
    d->view->setFixedHeight(newH);
}
