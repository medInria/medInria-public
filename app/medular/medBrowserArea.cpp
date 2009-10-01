/* medBrowserArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 25 12:23:43 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Sep 29 13:50:15 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 24
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
    d->view->setModel(d->model);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d->view);
}

medBrowserArea::~medBrowserArea(void)
{
    delete d->model;
    delete d->view;
    delete d;

    d = NULL;
}
