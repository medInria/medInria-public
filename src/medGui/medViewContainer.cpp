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
#include "medViewPool.h"

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
    
    d->pool = new medViewPool;
    
    if(medViewContainer *container = dynamic_cast<medViewContainer *>(parent)) {
        connect(this, SIGNAL(dropped(const medDataIndex&)), container, SIGNAL(dropped(const medDataIndex&)));
        connect(this, SIGNAL(focused(dtkAbstractView*)), container, SIGNAL(focused(dtkAbstractView*)));
    }

    this->setAcceptDrops(true);
    this->setFocusPolicy(Qt::ClickFocus);
    this->setMouseTracking(true);
    this->setSizePolicy (QSizePolicy::Expanding,QSizePolicy::Expanding);
}

medViewContainer::~medViewContainer(void)
{    
    d->pool->deleteLater();
    
    if (d->view)
        d->view->close();
    
    delete d;

    d = NULL;
}


medViewContainer *medViewContainer::current(void)
{
    medViewContainer * parent =
        dynamic_cast<medViewContainer *>( this->parentWidget() );
    if ( parent != NULL )
        return parent->current();

    return d->current;
}

void medViewContainer::split(int rows, int cols)
{
    Q_UNUSED(rows);
    Q_UNUSED(cols);
}

dtkAbstractView *medViewContainer::view(void) const
{
    return d->view;
}

QList<dtkAbstractView *> medViewContainer::views(void) const
{
    QList<dtkAbstractView *> views;
    if (d->view)
        views << d->view;
  
    return views;
}

medViewPool *medViewContainer::pool (void)
{
    return d->pool;
}

void medViewContainer::setView(dtkAbstractView *view)
{
    if (view==d->view)
        return;

    d->view = view;

    // pass properties to the view
    if (d->view) {
        QHash<QString,QString>::iterator it = d->viewProperties.begin();
        while (it!=d->viewProperties.end()) {
            view->setProperty (it.key(), it.value());
            ++it;
        }
    }
}

void medViewContainer::setCurrent(medViewContainer *container)
{
    medViewContainer * parent =
        dynamic_cast<medViewContainer *>( this->parentWidget() );
    if ( parent != NULL )
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

    medViewContainer * former = this->current();

    this->setCurrent(this);

    if (dtkAbstractView *view = this->view())
        emit focused(view);

    if (former)
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

    if ( this->current() == this )
        painter.setPen(QColor(0x9a, 0xb3, 0xd5));
    else
        painter.setPen(Qt::darkGray);
    
    painter.setBrush(QColor(0x38, 0x38, 0x38));
    painter.drawRect(this->rect().adjusted(0, 0, -1, -1));

    if ( this->view() != NULL &&
         this->view()->property( "Daddy" ) == "true" ) {
        // painter.setPen( Qt::red );
        painter.setPen( QPen( QColor( 255, 0, 0, 127 ), 1, Qt::DotLine ) );
        // painter.setPen( QColor( 255, 0, 0, 32 ) );
        painter.setBrush( Qt::transparent );
        painter.drawRect( this->rect().adjusted( 0, 0, -1, -1 ) );
    }

    if (!this->view() && (d->viewProperties.count() ||
                          !d->viewInfo.isEmpty()) ) 
    {
        painter.setPen(Qt::white);
        QFont font = painter.font();
        font.setPointSize (18);
        painter.setFont (font);
        QString text;
        
        //Add View Info:
        if (!d->viewInfo.isEmpty())
            //    Debug()<< "viewInfo" << d->viewInfo;
            text += d->viewInfo + "\n";
        
        QList<QString> keys = d->viewProperties.keys();
        foreach (QString key, keys)
            text += d->viewProperties[key] + "\n";
        painter.drawText (event->rect(), Qt::AlignCenter, text);
    }
    
    painter.end();
}

void medViewContainer::setViewProperty (const QString &key, const QString &value)
{
    d->viewProperties[key] = value;
}

QString medViewContainer::viewProperty (const QString &key) const
{
    if (d->viewProperties.contains (key))
        return d->viewProperties[key];

    return QString();
}

void medViewContainer::onViewFullScreen (bool value)
{
    Q_UNUSED (value);
}

void medViewContainer::setInfo(const QString& info)
{
    d->viewInfo = info;
}

QString medViewContainer::info()
{
    return d->viewInfo;
}
