/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medViewContainer_p.h"
#include "medSingleViewContainer.h"
#include "medViewPool.h"

#include <dtkCore/dtkAbstractView.h>

#include <medAbstractView.h>
#include <medViewManager.h>


void medSingleViewContainer::split(int rows, int cols)
{
    Q_UNUSED(rows);
    Q_UNUSED(cols);

    // Don't split a single view container

    return;
}

void medSingleViewContainer::setView(dtkAbstractView *view)
{
    if (view==d->view)
        return;

    if (d->view) { // cleanup the previous view first
        this->onViewClosing();
    }

    medViewContainer::setView (view);

    //d->view = view; // already called in medViewContainer::setView()

    if (d->view) {
        d->layout->setContentsMargins(0, 0, 0, 0);
        d->layout->addWidget(view->widget(), 0, 0);
        // BEGIN FIXME
        view->widget()->show();
        // END FIXME

        // set the view properties
        if (medAbstractView *medView = qobject_cast<medAbstractView*> (view)){
            d->pool->appendView (medView);
        }

        connect (view, SIGNAL (closing()), this, SLOT (onViewClosing()));
        connect (view, SIGNAL (changeDaddy(bool)),
                 this, SLOT (onDaddyChanged(bool)));

        this->recomputeStyleSheet();
        emit viewAdded (view);
    }
}

dtkAbstractView *medSingleViewContainer::view() const
{
    return d->view;
}

bool medSingleViewContainer::isLeaf(void) const
{
    return true;
}

void medSingleViewContainer::onViewClosing()
{
    if (d->view) {
        d->layout->removeWidget (d->view->widget());
        disconnect (d->view, SIGNAL (closing()), this, SLOT (onViewClosing()));
        disconnect (d->view, SIGNAL (changeDaddy(bool)),
                    this,    SLOT (onDaddyChanged(bool)));
        if (medAbstractView *medView = qobject_cast<medAbstractView*> (d->view))
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

void medSingleViewContainer::dragEnterEvent(QDragEnterEvent *event)
{
    this->setAttribute(Qt::WA_UpdatesDisabled, true);

    medViewContainer::dragEnterEvent(event);
}

void medSingleViewContainer::dragMoveEvent(QDragMoveEvent *event)
{
    medViewContainer::dragMoveEvent(event);
}

void medSingleViewContainer::dragLeaveEvent(QDragLeaveEvent *event)
{
    this->setAttribute(Qt::WA_UpdatesDisabled, false);

    medViewContainer::dragLeaveEvent(event);
}

void medSingleViewContainer::dropEvent(QDropEvent *event)
{
    this->setCurrent(this);
    this->setAttribute(Qt::WA_UpdatesDisabled, false);

    medViewContainer::dropEvent(event);
}
