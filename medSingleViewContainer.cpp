/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medViewContainer_p.h>
#include <medSingleViewContainer.h>

#include <medAbstractView.h>

#include <medAbstractView.h>
#include <medViewManager.h>


void medSingleViewContainer::split(int rows, int cols)
{
    Q_UNUSED(rows);
    Q_UNUSED(cols);

    // Don't split a single view container

    return;
}

void medSingleViewContainer::setView(medAbstractView *view)
{
    if (view == d->view)
        return;

    // cleanup the previous view first
    if (d->view)
        delete d->view;

    medViewContainer::setView(view);
}

medAbstractView *medSingleViewContainer::view() const
{
    return d->view;
}

bool medSingleViewContainer::isLeaf(void) const
{
    return true;
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
