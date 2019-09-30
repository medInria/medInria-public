/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreLegacyExport.h>

#include <QSlider>
#include <QtGui>

class MEDCORELEGACY_EXPORT medSliderL : public QSlider
{
    Q_OBJECT

public slots:
    void addTick(int position);
    int tickCount();
    void removeTick(int position);
    void removeAllTicks();

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

    /**
     * @brief getTickColor returns the color of the slider tick according to the application theme
     * @return  QColor
     */
    QColor getTickColor();

private:
    QList<int> ticksList;
};
