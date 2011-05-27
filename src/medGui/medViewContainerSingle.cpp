/* medViewContainerSingle.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Mar 17 11:01:46 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Dec 20 11:25:48 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 71
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
    connect (view, SIGNAL (changeDaddy(bool)),
             this, SLOT (onDaddyChanged(bool)));

    this->recomputeStyleSheet();
    emit viewAdded (view);
}

dtkAbstractView *medViewContainerSingle::view (void) const
{
    return d->view;
}

bool medViewContainerSingle::isLeaf(void) const
{
    return true;
}

void medViewContainerSingle::onViewClosing (void)
{
    if (d->view) {
        d->layout->removeWidget (d->view->widget());
        disconnect (d->view, SIGNAL (closing()), this, SLOT (onViewClosing()));
        disconnect (d->view, SIGNAL (changeDaddy(bool)),
                    this,    SLOT (onDaddyChanged(bool)));
        if (medAbstractView *medView = dynamic_cast<medAbstractView*> (d->view))
            d->pool->removeView (medView);

	emit viewRemoved (d->view);
	
        d->view->close();

        d->view = NULL;

        this->recomputeStyleSheet();
    }

    // qDebug() << this << __func__;
    // qDebug() << "isRoot:    " << this->isRoot();
    // qDebug() << "isLeaf:    " << this->isLeaf();
    // qDebug() << "isEmpty:   " << this->isEmpty();
    // qDebug() << "isCurrent: " << this->isCurrent();
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
