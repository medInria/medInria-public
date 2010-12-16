/* medToolBoxTab.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 10:13:39 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 10:15:15 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 3
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
    setTabPosition(North);
}

medToolBoxTab::~medToolBoxTab(void)
{
    delete d;

    d = NULL;
}

void medToolBoxTab::paintEvent(QPaintEvent *event)
{
    int height = 33;

    QLinearGradient gradient;
    gradient.setStart(0, 0);
    if (d->orientation == Qt::Vertical)
    {
        gradient.setFinalStop(0, height);
    }
    else
    {
        gradient.setFinalStop(height, 0);
        height = 40;
    }
    gradient.setColorAt(0, QColor("#3b3b3c"));
    gradient.setColorAt(1, QColor("#2d2d2f"));

    QPainter painter(this);
    painter.setPen(QColor("#2c2c2e"));
    painter.setBrush(gradient);
    if (d->orientation == Qt::Vertical)
        painter.drawRect(QRect(0, 0, this->width(), height));
    else
        painter.drawRect(QRect(0, 0,height , this->parentWidget()->height()));
    painter.end();

    QTabWidget::paintEvent(event);
}

void medToolBoxTab::setOrientation(Qt::Orientation orient)
{
    if (orient == d->orientation)
        return;
    if (orient == Qt::Vertical)
    {
        setTabPosition(North);
    }
    else
    {
        setTabPosition(West);
    }
}
