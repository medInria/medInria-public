/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medSlider.h"


medSlider::medSlider() : QSlider()
{
}

medSlider::~medSlider()
{
}

void medSlider::addTick(int position)
{
    if(!ticksList.contains(position))
        ticksList.append(position);
}
void medSlider::removeTick(int position)
{
    ticksList.removeAll(position);
}
void medSlider::removeAllTicks()
{
    ticksList.clear();
}

void medSlider::paintEvent(QPaintEvent *ev) {

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

