/* medViewContainer.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Oct 26 21:54:57 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Jun 15 16:26:36 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 425
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medViewContainer.h"
#include "medViewContainer_p.h"

#include <QtGui>

#include <dtkCore/dtkAbstractView.h>
#include <medCore/medDataIndex.h>

medViewContainer::medViewContainer(QWidget *parent) : QWidget(parent), d(new medViewContainerPrivate)
{
    d->layout = new QGridLayout(this);
    d->layout->setContentsMargins(0, 0, 0, 0);
    d->layout->setSpacing(2);

    d->view = NULL;

    d->current = this;

    if(medViewContainer *container = dynamic_cast<medViewContainer *>(parent)) {
        connect(this, SIGNAL(dropped(const medDataIndex&)), container, SIGNAL(dropped(const medDataIndex&)));
        connect(this, SIGNAL(focused(dtkAbstractView*)), container, SIGNAL(focused(dtkAbstractView*)));
    }

    this->setAcceptDrops(true);
    this->setFocusPolicy(Qt::ClickFocus);
    this->setMouseTracking(true);
}

medViewContainer::~medViewContainer(void)
{
    delete d;

    d = NULL;
}

medViewContainer::Type medViewContainer::type(void)
{
    return Default;
}

medViewContainer *medViewContainer::current(void)
{
    return d->current;
}

void medViewContainer::split(int rows, int cols)
{
    Q_UNUSED(rows);
    Q_UNUSED(cols);
}

dtkAbstractView *medViewContainer::view(void)
{
    return d->view;
}

void medViewContainer::setView(dtkAbstractView *view)
{
    if (!view)
        return;
}

void medViewContainer::setCurrent(medViewContainer *container)
{
    if(medViewContainer *parent = dynamic_cast<medViewContainer *>(this->parentWidget()))
        parent->setCurrent(container);
    else
        d->current = container;
}

void medViewContainer::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void medViewContainer::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void medViewContainer::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void medViewContainer::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasFormat("med/index")) {
        QStringList ids = QString(mimeData->data("med/index")).split(":");
        int patientId = ids.at(0).toInt();
        int   studyId = ids.at(1).toInt();
        int  seriesId = ids.at(2).toInt();
        int   imageId = ids.at(3).toInt();
        
        emit dropped(medDataIndex(patientId, studyId, seriesId, imageId));
    }

    event->acceptProposedAction();
}

void medViewContainer::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event);

    medViewContainer *former = this->current();

    this->setCurrent(this);

    if(dtkAbstractView *view = this->view())
        emit focused(view);

    former->update();
    this->update();
}

void medViewContainer::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
}

void medViewContainer::paintEvent(QPaintEvent *event)
{
    if(this->layout()->count() > 1)
        return;

    QWidget::paintEvent(event);

    QPainter painter;
    painter.begin(this);
    if (d->current == this)
        painter.setPen(QColor(0x9a, 0xb3, 0xd5));
    else
        painter.setPen(Qt::darkGray);
    painter.setBrush(QColor(0x38, 0x38, 0x38));
    painter.drawRect(this->rect().adjusted(0, 0, -1, -1));
    painter.end();
}

