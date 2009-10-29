/* medViewContainer.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Oct 26 21:54:57 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Thu Oct 29 10:55:38 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 19
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medViewContainer.h"

#include <QtGui>

#include <dtkCore/dtkAbstractView.h>

class medViewContainerPrivate
{
public:
    QGridLayout *layout;
};

medViewContainer::medViewContainer(QWidget *parent) : QWidget(parent), d(new medViewContainerPrivate)
{
    d->layout = new QGridLayout(this);
    d->layout->setContentsMargins(0, 0, 0, 0);
    d->layout->setSpacing(2);

    s_current = this;

    this->setFocusPolicy(Qt::StrongFocus);
}

medViewContainer::~medViewContainer(void)
{
    delete d;
    
    d = NULL;
}

medViewContainer *medViewContainer::current(void)
{
    return s_current;
}

void medViewContainer::split(int rows, int cols)
{
    medViewContainer *current = this->current();

    if (!current)
        return;

    if (current->d->layout->count())
        return;

    for(int i = 0 ; i < rows ; i++)
        for(int j = 0 ; j < cols ; j++)
            current->d->layout->addWidget(new medViewContainer, i, j);

    s_current = 0;
}

void medViewContainer::setView(dtkAbstractView *view)
{
    medViewContainer *current = this->current();

    if (!current)
        return;

    if (current->d->layout->count())
        return;

    if(QWidget *widget = view->widget()) {
        widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        current->d->layout->setContentsMargins(1, 1, 1, 1);
        current->d->layout->addWidget(widget, 0, 0);
    }
}

void medViewContainer::focusInEvent(QFocusEvent *event)
{
    s_current = this;

    QWidget::focusInEvent(event);
}

void medViewContainer::focusOutEvent(QFocusEvent *event)
{
    QWidget::focusOutEvent(event);
}

void medViewContainer::paintEvent(QPaintEvent *event)
{
    if(this->layout()->count() > 1)
        return;

    QWidget::paintEvent(event);

    QPainter painter;
    painter.begin(this);
    if (s_current == this)
        painter.setPen(Qt::red);
    else
        painter.setPen(Qt::darkGray);
    painter.setBrush(QColor(0x38, 0x38, 0x38));
    painter.drawRect(this->rect().adjusted(0, 0, -1, -1));
    painter.end();
}

medViewContainer *medViewContainer::s_current = NULL;
