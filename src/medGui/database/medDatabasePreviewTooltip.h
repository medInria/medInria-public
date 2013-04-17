/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore>
#include <QtGui>

class medDatabasePreviewTooltip : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

    Q_PROPERTY(float opacity READ opacity WRITE setOpacity)

public:
    medDatabasePreviewTooltip(const QPixmap& pixmap, QGraphicsItem *parent = 0);
};


