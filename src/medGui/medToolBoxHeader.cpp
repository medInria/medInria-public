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

    Qt::Orientation orientation;
};

medToolBoxHeader::medToolBoxHeader(QWidget *parent) : QFrame(parent), d(new medToolBoxHeaderPrivate)
{
    d->title = "Untitled";

    d->orientation = Qt::Vertical;

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

medToolBoxHeader::~medToolBoxHeader(void)
{
    delete d;

    d = NULL;
}

QSize medToolBoxHeader::sizeHint(void) const
{
    if (d->orientation == Qt::Vertical)
        return QSize(100, 32);
    else
        return QSize(32, 100);
}

void medToolBoxHeader::setTitle(const QString& title)
{
    d->title = title;
}

void medToolBoxHeader::setOrientation(Qt::Orientation orientation)
{
    d->orientation = orientation;

    this->setObjectName((d->orientation == Qt::Vertical) ? "" : "horizontal");

    if(d->orientation == Qt::Vertical)
        this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    else
        this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
}

void medToolBoxHeader::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);

    QPainter painter(this);
    painter.setPen(QColor("#bbbbbb"));

    QRectF rect = this->rect();

    if(d->orientation == Qt::Horizontal) {
        painter.translate(10, rect.height()-16);
        painter.rotate(-90.0);
        
        QSizeF size = rect.size();
        size.transpose();

        rect = QRectF(rect.topLeft(), size);

        qDebug() << DTK_PRETTY_FUNCTION << rect;
        
    } else {
        painter.translate(16, 10);
    }
    
    painter.drawText(rect, Qt::AlignLeft, d->title);
    painter.end();
}
