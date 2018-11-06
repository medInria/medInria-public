/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medSlider.h"
#include "medSettingsManager.h"

void medSlider::addTick(int position)
{
    if( (ticksList.isEmpty()) || !ticksList.contains(position))
    {
        ticksList.append(position);
    }
}

int medSlider::tickCount()
{
    if (!ticksList.isEmpty())
    {
        return ticksList.count();
    }

    return 0;
}

void medSlider::removeTick(int position)
{
    if (!ticksList.isEmpty())
    {
        ticksList.removeAll(position);
    }
}

void medSlider::removeAllTicks()
{
    if (!ticksList.isEmpty())
    {
        ticksList.clear();
    }
}

void medSlider::paintEvent(QPaintEvent *ev)
{

    QSlider::paintEvent(ev);

    QPainter painter(this);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    for(int i = 0; i<tickCount(); i++)
    {
        int position = QStyle::sliderPositionFromValue(minimum(),
                                                       maximum(),
                                                       ticksList.at(i),
                                                       width());
        painter.setPen(this->getTickColor());
        painter.drawLine(position, 0, position, height());
    }
}

QColor medSlider::getTickColor()
{
    // CSS
    QVariant themeChosen = medSettingsManager::instance()->value("startup","theme");
    int themeIndex = themeChosen.toInt();

    switch (themeIndex)
    {
    case 0:
    default:
        return QColor("cyan"); // Dark Theme
        break;
    case 1:
        return QColor("blue"); // Light Theme
        break;
    }
}

void medSlider::mousePressEvent(QMouseEvent *event)
{
    QSlider::mousePressEvent(event);
    setValue(QStyle::sliderValueFromPosition(minimum(), maximum(), event->x(), width()));
    event->accept();
}
