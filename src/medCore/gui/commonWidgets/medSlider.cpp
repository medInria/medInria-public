/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medSlider.h"

void medSlider::addTick(int position)
{
    if( (tickCount() == 0) || !ticksList.contains(position))
    {
        ticksList.append(position);
    }
}

int medSlider::tickCount()
{
    if (!ticksList.toVector().isEmpty())
    {
        return ticksList.count();
    }
    return 0;
}

void medSlider::removeTick(int position)
{
    if (tickCount() > 0)
    {
        ticksList.removeAll(position);
    }
}

void medSlider::removeAllTicks()
{
    if (tickCount() > 0)
    {
        ticksList.clear();
    }
}

void medSlider::paintEvent(QPaintEvent *ev)
{

    QSlider::paintEvent(ev);

    QPainter painter(this);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    for(int i = 0; i<ticksList.size(); i++)
    {
        int position = QStyle::sliderPositionFromValue(minimum(),
                                                       maximum(),
                                                       ticksList.at(i),
                                                       width());

        painter.drawLine(position, 0, position, height());
    }
}

void medSlider::mousePressEvent(QMouseEvent *event)
{
    QSlider::mousePressEvent(event);
    setValue(QStyle::sliderValueFromPosition(minimum(), maximum(), event->x(), width()));
    event->accept();
}
