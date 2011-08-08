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
};

medToolBoxTab::medToolBoxTab(QWidget *parent) : QTabWidget(parent), d(new medToolBoxTabPrivate)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

medToolBoxTab::~medToolBoxTab(void)
{
    delete d;

    d = NULL;
}

void medToolBoxTab::paintEvent(QPaintEvent *event)
{
    int size =  33;

    QLinearGradient gradient;
    gradient.setStart(0, 0);
    gradient.setColorAt(0, QColor("#3b3b3c"));
    gradient.setColorAt(1, QColor("#2d2d2f"));

    QPainter painter(this);
    painter.setPen(QColor("#2c2c2e"));
    painter.setBrush(gradient);
    painter.drawRect(QRect(0, 0, this->width(), size));
    painter.end();

    QTabWidget::paintEvent(event);
}

