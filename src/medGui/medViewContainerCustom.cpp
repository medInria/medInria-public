/* medViewContainerCustom.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Mar 17 11:01:46 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Mar 17 18:13:32 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 42
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medViewContainer_p.h"
#include "medViewContainerCustom.h"

#include <dtkCore/dtkAbstractView.h>

medViewContainer::Type medViewContainerCustom::type(void)
{
    return medViewContainer::Custom;
}

void medViewContainerCustom::split(int rows, int cols)
{
    if (d->layout->count())
        return;

    for(int i = 0 ; i < rows ; i++)
        for(int j = 0 ; j < cols ; j++)
            d->layout->addWidget(new medViewContainerCustom(this), i, j);

    s_current = 0;
}

void medViewContainerCustom::setView(dtkAbstractView *view)
{
    // if (d->layout->count() && (view->widget() == d->layout->itemAtPosition(0, 0)->widget()))
    //     return;

    if (d->layout->count())
        d->layout->removeItem(d->layout->itemAt(0));
    
    d->layout->setContentsMargins(1, 1, 1, 1);    
    d->layout->addWidget(view->widget(), 0, 0);
    d->view = view;
}

void medViewContainerCustom::dragEnterEvent(QDragEnterEvent *event)
{
    this->setAttribute(Qt::WA_UpdatesDisabled, true);

    medViewContainer::dragEnterEvent(event);
}

void medViewContainerCustom::dragMoveEvent(QDragMoveEvent *event)
{
    medViewContainer::dragMoveEvent(event);
}

void medViewContainerCustom::dragLeaveEvent(QDragLeaveEvent *event)
{
    this->setAttribute(Qt::WA_UpdatesDisabled, false);

    medViewContainer::dragLeaveEvent(event);
}

void medViewContainerCustom::dropEvent(QDropEvent *event)
{
    s_current = this;

    this->setAttribute(Qt::WA_UpdatesDisabled, false);

    medViewContainer::dropEvent(event);
}

void medViewContainerCustom::focusInEvent(QFocusEvent *event)
{
    medViewContainer::focusInEvent(event);
}

void medViewContainerCustom::focusOutEvent(QFocusEvent *event)
{
    medViewContainer::focusOutEvent(event);
}
