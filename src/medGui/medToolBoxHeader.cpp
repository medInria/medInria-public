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

    // // QFontDatabase fdb;
    // // qDebug() << fdb.families();
    // QFont font = this->font();
    // font.setFamily( "CM Sans" );
    // // font.setLetterSpacing( QFont::AbsoluteSpacing, -5.0 );
    // this->setFont( font );
}

medToolBoxHeader::~medToolBoxHeader(void)
{
    delete d;

    d = NULL;
}

void medToolBoxHeader::setTitle(const QString& title)
{
    d->title = title;
}

void medToolBoxHeader::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);

    QRectF rect = this->rect();

    QPainter painter(this);

    // QColor c = painter.pen().color();
    // c.setAlpha( 200 );
    // painter.setPen( QPen( c ) );

    painter.translate(7, 3);
    
    painter.drawText(rect, Qt::AlignLeft, d->title);
    painter.end();
}

void medToolBoxHeader::mousePressEvent( QMouseEvent *event )
{
    emit triggered();
}
