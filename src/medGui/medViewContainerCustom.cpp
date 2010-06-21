/* medViewContainerCustom.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Mar 17 11:01:46 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Jun 15 16:28:13 2010 (+0200)
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
#include "medViewContainerCustom.h"
#include "medViewPool.h"

#include <dtkCore/dtkAbstractView.h>

medViewContainerCustom::medViewContainerCustom (QWidget *parent) : medViewContainer(parent)
{
}

medViewContainerCustom::~medViewContainerCustom()
{
}

medViewContainer::Type medViewContainerCustom::type(void)
{
    return medViewContainer::Custom;
}

void medViewContainerCustom::split(int rows, int cols)
{
    if (d->layout->count())
        return;

    for(int i = 0 ; i < rows ; i++)
        for(int j = 0 ; j < cols ; j++) {
	    medViewContainerCustom *container = new medViewContainerCustom(this);
            d->layout->addWidget(container, i, j);
	}

    this->setCurrent(NULL);
}

void medViewContainerCustom::setPreset(int preset)
{
    if (d->layout->count())
        return;

    medViewContainerCustom *custom1;
    medViewContainerCustom *custom2;

    switch(preset) {
    case A:
        d->layout->addWidget(new medViewContainerCustom(this), 0, 0);
        d->layout->addWidget(new medViewContainerCustom(this), 0, 1);
        break;
    case B:
        d->layout->addWidget(new medViewContainerCustom(this), 0, 0);
        d->layout->addWidget(new medViewContainerCustom(this), 1, 0);
        break;
    case C:
        custom1 = new medViewContainerCustom(this);
        custom1->split(2, 1);
        custom2 = new medViewContainerCustom(this);
        d->layout->addWidget(custom1, 0, 0);
        d->layout->addWidget(custom2, 0, 1);
        d->layout->setColumnStretch(0, 1);
        d->layout->setColumnStretch(1, 2);
        break;
    case D:
        custom1 = new medViewContainerCustom(this);
        custom1->split(3, 1);
        custom2 = new medViewContainerCustom(this);
        d->layout->addWidget(custom1, 0, 0);
        d->layout->addWidget(custom2, 0, 1);
        d->layout->setColumnStretch(0, 1);
        d->layout->setColumnStretch(1, 2);
        break;
    case E:
        d->layout->addWidget(new medViewContainerCustom(this), 0, 0);
        d->layout->addWidget(new medViewContainerCustom(this), 0, 1);
        d->layout->addWidget(new medViewContainerCustom(this), 1, 0);
        d->layout->addWidget(new medViewContainerCustom(this), 1, 1);
        break;
    };

    this->setCurrent(NULL);
}

void medViewContainerCustom::setView(dtkAbstractView *view)
{
    if (d->layout->count())
        d->layout->removeItem(d->layout->itemAt(0));
    
    d->layout->setContentsMargins(1, 1, 1, 1);    
    d->layout->addWidget(view->widget(), 0, 0);
    d->view = view;

    this->synchronize_2 (view);
}

void medViewContainerCustom::synchronize_2 (dtkAbstractView *view)
{
  if (medViewContainerCustom *parent = dynamic_cast<medViewContainerCustom*>(this->parent())) {
      parent->synchronize_2(view);
  }
  else { // top level medViewContainerCustom
      d->pool->appendView (view);
  }
}

void medViewContainerCustom::synchronize (void)
{
  if (medViewContainerCustom *parent = dynamic_cast<medViewContainerCustom*>(this->parent())) {
      parent->synchronize();
  }
  else { // top level medViewContainerCustom      
      d->pool->synchronize();
  }
}

void medViewContainerCustom::desynchronize (void)
{
  if (medViewContainerCustom *parent = dynamic_cast<medViewContainerCustom*>(this->parent())) {
      parent->desynchronize();
  }
  else { // top level medViewContainerCustom
      d->pool->desynchronize();
  }
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
    this->setCurrent(this);

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
