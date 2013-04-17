/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

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

