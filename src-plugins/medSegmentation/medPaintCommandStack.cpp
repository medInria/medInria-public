/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medPaintCommandStack.h>

medPaintCommandStack::medPaintCommandStack(QObject *parent): QUndoStack(parent)
{
    connect(this, SIGNAL(indexChanged(int)), this, SLOT(redoAllUntilIndex(int)));

    currentIndex = -1;
}

void medPaintCommandStack::redoAllUntilIndex(int index)
{
    if(index < currentIndex)
    {
        disconnect(this, SIGNAL(indexChanged(int)), this, SLOT(redoAllUntilIndex(int)));

        setIndex(0);
        setIndex(index);

        connect(this, SIGNAL(indexChanged(int)), this, SLOT(redoAllUntilIndex(int)));
    }

    currentIndex = index;
}
