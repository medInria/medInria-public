/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSettingsManager.h>
#include <medSliderL.h>

#include <QStyle>

void medSliderL::addTick(int position)
{
    if( (ticksList.isEmpty()) || !ticksList.contains(position))
    {
        ticksList.append(position);
    }
}

int medSliderL::tickCount()
{
    if (!ticksList.isEmpty())
    {
        return ticksList.count();
    }

    return 0;
}

void medSliderL::removeTick(int position)
{
    if (!ticksList.isEmpty())
    {
        ticksList.removeAll(position);
    }
}

void medSliderL::removeAllTicks()
{
    if (!ticksList.isEmpty())
    {
        ticksList.clear();
    }
}

void medSliderL::paintEvent(QPaintEvent *event)
{
    QSlider::paintEvent(event);

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

/**
 * @brief Return the color of the small lines displayed on view sliders in some toolboxes,
 * according to the application theme.
 * 
 * @return QColor 
 */
QColor medSliderL::getTickColor()
{
    QVariant themeChosen = medSettingsManager::instance().value("startup","theme");
    int themeIndex = themeChosen.toInt();

    switch (themeIndex)
    {
    case 0:
    default:
        return QColor(239, 109, 59, 255);
        break;
    case 1:
    case 2:
        return QColor("blue");
        break;
    }
}

void medSliderL::mousePressEvent(QMouseEvent *event)
{
    QSlider::mousePressEvent(event);
    setValue(QStyle::sliderValueFromPosition(minimum(), maximum(), event->x(), width()));
    event->accept();
}
