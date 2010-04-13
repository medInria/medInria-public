/* medViewContainerSingle.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Mar 17 11:01:46 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Mar 17 18:49:32 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 67
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medViewContainer_p.h"
#include "medViewContainerSingle.h"

#include <dtkCore/dtkAbstractView.h>

medViewContainer::Type medViewContainerSingle::type(void)
{
    return medViewContainer::Single;
}

void medViewContainerSingle::split(int rows, int cols)
{
    Q_UNUSED(rows);
    Q_UNUSED(cols);

    // Don't split a single view container

    return;
}

void medViewContainerSingle::setView(dtkAbstractView *view)
{
    // if (d->layout->count())
    //     d->layout->takeAt(0)->widget()->hide();
    
    d->layout->setContentsMargins(1, 1, 1, 1);    
    d->layout->addWidget(view->widget(), 0, 0);
    d->view = view;
    connect(d->view,SIGNAL(closed()),this,SLOT(unsetView()));
}

void medViewContainerSingle::dragEnterEvent(QDragEnterEvent *event)
{
    this->setAttribute(Qt::WA_UpdatesDisabled, true);

    medViewContainer::dragEnterEvent(event);
}

void medViewContainerSingle::dragMoveEvent(QDragMoveEvent *event)
{
    medViewContainer::dragMoveEvent(event);
}

void medViewContainerSingle::dragLeaveEvent(QDragLeaveEvent *event)
{
    this->setAttribute(Qt::WA_UpdatesDisabled, false);

    medViewContainer::dragLeaveEvent(event);
}

void medViewContainerSingle::dropEvent(QDropEvent *event)
{
    s_current = this;

    this->setAttribute(Qt::WA_UpdatesDisabled, false);

    medViewContainer::dropEvent(event);
}

void medViewContainerSingle::focusInEvent(QFocusEvent *event)
{
    medViewContainer::focusInEvent(event);
}

void medViewContainerSingle::focusOutEvent(QFocusEvent *event)
{
    medViewContainer::focusOutEvent(event);
}

void medViewContainerSingle::unsetView(void)
{
    d->layout->removeWidget(d->view->widget());
    delete d->view;
    d->view= NULL;
}
