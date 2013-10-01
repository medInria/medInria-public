/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

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
    if (!d->png)  d->png = new QPixmap(":icons/information.png");
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

    painter.drawText(rect, Qt::AlignCenter, d->title);
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
