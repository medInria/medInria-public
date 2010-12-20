/* medToolBoxTab.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 10:13:39 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Dec 20 16:37:38 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 28
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medToolBoxTab.h"

class medToolBoxTabPrivate
{
public:
    Qt::Orientation orientation;
};

medToolBoxTab::medToolBoxTab(QWidget *parent) : QTabWidget(parent), d(new medToolBoxTabPrivate)
{
    d->orientation = Qt::Vertical;

    this->setTabPosition(North);

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

medToolBoxTab::~medToolBoxTab(void)
{
    delete d;

    d = NULL;
}

QSize medToolBoxTab::sizeHint(void) const
{
    if(d->orientation == Qt::Vertical)
        return QSize(50*this->count(), 32);
    else
        return QSize(32, 50*this->count());
}

void medToolBoxTab::paintEvent(QPaintEvent *event)
{
    int size = (d->orientation == Qt::Vertical) ? 33 : 50;

    QLinearGradient gradient;
    gradient.setStart(0, 0);
    gradient.setFinalStop((d->orientation == Qt::Vertical) ? 0 : size, (d->orientation == Qt::Vertical) ? size : 0);
    gradient.setColorAt(0, QColor("#3b3b3c"));
    gradient.setColorAt(1, QColor("#2d2d2f"));

    QPainter painter(this);
    painter.setPen(QColor("#2c2c2e"));
    painter.setBrush(gradient);
    if (d->orientation == Qt::Vertical)
        painter.drawRect(QRect(0, 0, this->width(), size));
    else
        painter.drawRect(QRect(0, 0, size , this->height()));
    painter.end();

    QTabWidget::paintEvent(event);
}

void medToolBoxTab::setOrientation(Qt::Orientation orientation)
{
    if (orientation == d->orientation)
        return;

    d->orientation = orientation;

    this->setObjectName((d->orientation == Qt::Vertical) ? "" : "horizontal");

    if (d->orientation == Qt::Vertical) {
        this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        this->setTabPosition(North);
    } else {
        this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        setTabPosition(West);
    }
}
