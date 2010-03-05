/* medViewContainer.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Oct 26 21:54:57 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Mar  5 17:00:35 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 44
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
#include <medCore/medDataIndex.h>

class medViewContainerPrivate
{
public:
    QGridLayout *layout;

    dtkAbstractView *view;
};

medViewContainer::medViewContainer(QWidget *parent) : QWidget(parent), d(new medViewContainerPrivate)
{
    d->layout = new QGridLayout(this);
    d->layout->setContentsMargins(0, 0, 0, 0);
    d->layout->setSpacing(2);

    d->view = NULL;

    s_current = this;

    if(medViewContainer *container = dynamic_cast<medViewContainer *>(parent)) {
        connect(this, SIGNAL(dropped(const medDataIndex&)), container, SIGNAL(dropped(const medDataIndex&)));
        connect(this, SIGNAL(focused(dtkAbstractView*)), container, SIGNAL(focused(dtkAbstractView*)));
    }

    this->setAcceptDrops(true);
    this->setFocusPolicy(Qt::ClickFocus);
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
    if (d->layout->count())
        return;

    for(int i = 0 ; i < rows ; i++)
        for(int j = 0 ; j < cols ; j++)
            d->layout->addWidget(new medViewContainer(this), i, j);

    s_current = 0;
}

dtkAbstractView *medViewContainer::view(void)
{
    return d->view;
}

void medViewContainer::setView(dtkAbstractView *view)
{
    if (!view)
        return;

    if (d->layout->count())
        return;

    if(QWidget *widget = view->widget()) {
        d->layout->setContentsMargins(1, 1, 1, 1);
        d->layout->addWidget(widget, 0, 0);
        d->view = view;
    }
}

void medViewContainer::dragEnterEvent(QDragEnterEvent *event)
{
    setBackgroundRole(QPalette::Highlight);

    event->acceptProposedAction();
}

void medViewContainer::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void medViewContainer::dragLeaveEvent(QDragLeaveEvent *event)
{
    setBackgroundRole(QPalette::Base);

    event->accept();
}

void medViewContainer::dropEvent(QDropEvent *event)
{
    s_current = this;

    this->update();

    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasFormat("med/index")) {
        QStringList ids = QString(mimeData->data("med/index")).split(":");
        int patientId = ids.at(0).toInt();
        int   studyId = ids.at(1).toInt();
        int  seriesId = ids.at(2).toInt();
        int   imageId = ids.at(3).toInt();
        
        emit dropped(medDataIndex(patientId, studyId, seriesId, imageId));
    }

    setBackgroundRole(QPalette::Base);

    event->acceptProposedAction();
}

void medViewContainer::focusInEvent(QFocusEvent *event)
{
    s_current = this;

    if(dtkAbstractView *view = this->view())
        emit focused(view);

    this->update();
}

void medViewContainer::focusOutEvent(QFocusEvent *event)
{
    this->update();
}

void medViewContainer::paintEvent(QPaintEvent *event)
{
    if(this->layout()->count() > 1)
        return;

    QWidget::paintEvent(event);

    QPainter painter;
    painter.begin(this);
    if (s_current == this)
        painter.setPen(QColor(0x9a, 0xb3, 0xd5));
    else
        painter.setPen(Qt::darkGray);
    painter.setBrush(QColor(0x38, 0x38, 0x38));
    painter.drawRect(this->rect().adjusted(0, 0, -1, -1));
    painter.end();
}

medViewContainer *medViewContainer::s_current = NULL;
