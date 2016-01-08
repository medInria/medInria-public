/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtGui>
#include <medCoreExport.h>


class medSlider : public QSlider{
    Q_OBJECT

public slots:
    void addTick(int position)
    {
        if(!ticksList.contains(position))
            ticksList.append(position);
    }
    void removeTick(int position)
    {
        ticksList.removeAll(position);
    }

protected:
    void paintEvent(QPaintEvent *ev) {

        QSlider::paintEvent(ev);

        QPainter painter(this);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        for(unsigned int i = 0; i<ticksList.size(); i++)
        {
            int position = QStyle::sliderPositionFromValue(minimum(),
                                                           maximum(),
                                                           ticksList.at(i),
                                                           width());

            painter.drawLine(position, 0, position, height());
        }

    }
private:
    QList<int> ticksList;
};
