/* medToolBoxHeader.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 10:15:58 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Dec 20 15:58:20 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 156
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include "medToolBoxHeader.h"

#include <dtkCore/dtkGlobal.h>

class medToolBoxHeaderPrivate
{
public:
    QString title;
    QPoint titleOffset;
};

medToolBoxHeader::medToolBoxHeader(QWidget *parent) : QFrame(parent), d(new medToolBoxHeaderPrivate)
{
    d->title = "Untitled";
    d->titleOffset = QPoint( 0, 0 );

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

medToolBoxHeader::~medToolBoxHeader(void)
{
    delete d;

    d = NULL;
}

QSize medToolBoxHeader::sizeHint(void) const
{
    return QSize(100, 32);
}

void medToolBoxHeader::setTitle(const QString& title)
{
    d->title = title;
}

QString medToolBoxHeader::title() const
{
    return d->title;
}


void medToolBoxHeader::setTitleOffset(const QPoint & titleOffset)
{
    d->titleOffset = titleOffset;
}

void medToolBoxHeader::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);

    QRectF rect = this->contentsRect();

    QPainter painter(this);
    painter.translate( d->titleOffset );

    painter.drawText(rect, Qt::AlignLeft, d->title);
    painter.end();
}

void medToolBoxHeader::mousePressEvent( QMouseEvent *event )
{
    emit triggered();
}
