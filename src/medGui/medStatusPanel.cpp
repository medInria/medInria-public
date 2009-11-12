/* medStatusPanel.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Sun Oct 11 13:02:28 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Nov  3 09:42:50 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 121
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medStatusPanel.h"

#include <QtGui>

// /////////////////////////////////////////////////////////////////
// medStatusPanel
// /////////////////////////////////////////////////////////////////

class medStatusPanelPrivate
{
public:
    QHBoxLayout *layout;

    int spacing;
};

medStatusPanel::medStatusPanel(QWidget *parent) : QFrame(parent), d(new medStatusPanelPrivate)
{
    d->spacing = 12;

    d->layout = new QHBoxLayout(this);
    d->layout->setContentsMargins(5, 0, 5, 0);
    d->layout->setSpacing(d->spacing);

    this->setStyleSheet("font-size: 9px;");
    this->setFixedHeight(32);
}

medStatusPanel::~medStatusPanel(void)
{
    delete d;

    d = NULL;
}

void medStatusPanel::addSpacer(void)
{
    d->layout->addStretch(2);
}

void medStatusPanel::addWidget(QWidget *widget)
{
    d->layout->addWidget(widget);
}

void medStatusPanel::removeWidget(QWidget *widget)
{
    d->layout->removeWidget(widget);
}

static void paintLayout(QPainter *painter, QLayout *layout, int spacing, const QRect& rect)
{
    for (int i = 1; i < layout->count(); ++i) {

        QLayoutItem *item = layout->itemAt(i);
          
        QPoint tl(item->geometry().topLeft().x() - spacing/2, rect.top() + 2);
        QPoint bl(item->geometry().bottomLeft().x() - spacing/2, rect.bottom() - 2);

        painter->save();
        painter->setPen(QColor(0x31, 0x31, 0x31));
        painter->drawLine(tl, bl);
        painter->restore();
    }
}

void medStatusPanel::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    // painter.setPen(Qt::darkGray);
    // painter.setBrush(QColor(0x31, 0x31, 0x31).lighter(150));
    painter.fillRect(event->rect(), QColor(0x31, 0x31, 0x31).lighter(150));
    paintLayout(&painter, this->layout(), d->spacing, event->rect());
    painter.end();
}
