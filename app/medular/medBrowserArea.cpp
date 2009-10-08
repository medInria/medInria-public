/* medBrowserArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 25 12:23:43 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Oct  8 09:43:47 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 50
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medBrowserArea.h"

#include <QtGui>

#include <medGui/medImageFlow.h>
#include <medGui/medImagePreview.h>

#include <medSql/medDatabaseController.h>
#include <medSql/medDatabaseModel.h>
#include <medSql/medDatabaseView.h>

// /////////////////////////////////////////////////////////////////
// medBrowserAreaPreview
// /////////////////////////////////////////////////////////////////

class medBrowserAreaPreviewPrivate
{
public:
    QStackedWidget *stack;

    medImageFlow *flow;
    medImagePreview *preview;
};

class medBrowserAreaPreview : public QWidget
{
public:
     medBrowserAreaPreview(QWidget *parent = 0);
    ~medBrowserAreaPreview(void);

private:
    medBrowserAreaPreviewPrivate *d;
};

medBrowserAreaPreview::medBrowserAreaPreview(QWidget *parent) : QWidget(parent), d(new medBrowserAreaPreviewPrivate)
{
    d->stack = new QStackedWidget(this);
    d->stack->setStyleSheet("background: yellow;");

    d->flow = new medImageFlow(this);
    d->preview = new medImagePreview(this);

    this->setFixedHeight(200);
}

medBrowserAreaPreview::~medBrowserAreaPreview(void)
{
    delete d->stack;
    delete d->flow;
    delete d->preview;
    delete d;

    d = NULL;
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

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d->view);
    layout->addWidget(d->preview);
}

medBrowserArea::~medBrowserArea(void)
{
    delete d->preview;
    delete d->model;
    delete d->view;
    delete d;

    d = NULL;
}
