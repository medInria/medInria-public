/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>
#include <QtGui>


class medSlider : public QSlider{
    Q_OBJECT

public:
    medSlider();
    ~medSlider();

public slots:
    void addTick(int position);
    void removeTick(int position);
    void removeAllTicks();

protected:
    void paintEvent(QPaintEvent *ev);

private:
    QList<int> ticksList;
};
