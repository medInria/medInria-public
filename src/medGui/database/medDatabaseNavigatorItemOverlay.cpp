/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDatabaseNavigatorItemOverlay.h>

// /////////////////////////////////////////////////////////////////
// medDatabaseNavigatorItemOverlay
// /////////////////////////////////////////////////////////////////

class medDatabaseNavigatorItemOverlayPrivate
{
};

medDatabaseNavigatorItemOverlay::medDatabaseNavigatorItemOverlay(QGraphicsItem *parent)
    : QObject(), QGraphicsPixmapItem(parent), d(new medDatabaseNavigatorItemOverlayPrivate)
{
}

medDatabaseNavigatorItemOverlay::~medDatabaseNavigatorItemOverlay(void)
{
    delete d;
    
    d = NULL;
}

void medDatabaseNavigatorItemOverlay::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit clicked();
}
