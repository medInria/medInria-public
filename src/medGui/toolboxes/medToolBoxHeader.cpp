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
#include <medButton.h>

#include <dtkCore/dtkGlobal.h>

class medToolBoxHeaderPrivate
{
public:
    QString title;
    QPoint titleOffset;
    medButton* about;
    static QPixmap* png;
    static const QString tooltip;
};


const QString medToolBoxHeaderPrivate::tooltip = QObject::tr("About this plugin");
QPixmap * medToolBoxHeaderPrivate::png = NULL;

medToolBoxHeader::medToolBoxHeader(QWidget *parent) : QFrame(parent), d(new medToolBoxHeaderPrivate)
{
    d->png = new QPixmap(":icons/information.png");
    d->title = "Untitled";
    d->titleOffset = QPoint( 0, 0 );
//    d->about = NULL;
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight,this);
    layout->setMargin(0);

    d->about = new medButton(this,*(d->png),
                             medToolBoxHeaderPrivate::tooltip);
    layout->addStretch();
    layout->addWidget(d->about);
    d->about->hide();

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

void medToolBoxHeader::setAboutButtonVisibility(bool visible)
{
    d->about->setVisible(visible);
}

void medToolBoxHeader::mousePressEvent( QMouseEvent *event )
{
    emit triggered();
}

medButton * medToolBoxHeader::aboutButton()
{
    return d->about;
}
