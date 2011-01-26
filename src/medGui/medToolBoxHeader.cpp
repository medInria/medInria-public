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
};

medToolBoxHeader::medToolBoxHeader(QWidget *parent) : QFrame(parent), d(new medToolBoxHeaderPrivate)
{
    d->title = "Untitled";

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

void medToolBoxHeader::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);

    QPainter painter(this);
    painter.setPen(QColor("#bbbbbb"));

    QRectF rect = this->rect();

    painter.translate(16, 10);
    
    painter.drawText(rect, Qt::AlignLeft, d->title);
    painter.end();
}
