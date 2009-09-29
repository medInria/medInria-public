/* medBrowserArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 25 12:23:43 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Fri Sep 25 14:15:52 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 19
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

class medBrowserAreaPrivate
{
public:
    medDatabaseModel *model;
    medDatabaseView *view;
};

medBrowserArea::medBrowserArea(QWidget *parent) : QWidget(parent), d(new medBrowserAreaPrivate)
{
    d->model = new medDatabaseModel;

    d->view = new medDatabaseView(this);
    // d->view->setFrameStyle(QFrame::NoFrame);
    // d->view->setAttribute(Qt::WA_MacShowFocusRect, false);
    // d->view->setUniformRowHeights(true);
    // d->view->setAnimated(true);
    // d->view->setAlternatingRowColors(true);
    // d->view->setSortingEnabled(true);
    d->view->setModel(d->model);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d->view);
}

medBrowserArea::~medBrowserArea(void)
{
    delete d->view;
    delete d;

    d = NULL;
}
