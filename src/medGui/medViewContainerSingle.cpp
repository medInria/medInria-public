/* medViewContainerSingle.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Mar 17 11:01:46 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Jun 15 16:30:34 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 69
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medViewContainer_p.h"
#include "medViewContainerSingle.h"
#include "medViewPool.h"

#include <dtkCore/dtkAbstractView.h>

#include <medCore/medAbstractView.h>
#include <medCore/medViewManager.h>

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
    if (view==d->view)
        return;

    medViewContainer::setView (view);
    
    // if (d->layout->count())
    //     d->layout->takeAt(0)->widget()->hide();
    
    d->layout->setContentsMargins(1, 1, 1, 1);    
    d->layout->addWidget(view->widget(), 0, 0);
    d->view = view;
	
    // d->view->reset();

    // set the view properties

    if (medAbstractView *medView = dynamic_cast<medAbstractView*> (view))
        d->pool->appendView (medView);
    connect (view, SIGNAL (closing()), this, SLOT (onViewClosing()));
}

void medViewContainerSingle::onViewClosing (void)
{
    if (d->view) {
        d->layout->removeWidget (d->view->widget());
        disconnect (d->view, SIGNAL (closing()), this, SLOT (onViewClosing()));
        if (medAbstractView *medView = dynamic_cast<medAbstractView*> (d->view))
            d->pool->removeView (medView);
        d->view->close();
        d->view = NULL;
    }
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
    this->setCurrent(this);
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
