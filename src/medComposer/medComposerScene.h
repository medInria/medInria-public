/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#ifndef MEDCOMPOSERSCENE_H
#define MEDCOMPOSERSCENE_H

#include <dtkComposer/dtkComposerScene.h>

class medComposerScene : public dtkComposerScene
{
public:
    medComposerScene();

    void dropEvent(QGraphicsSceneDragDropEvent *event);
};

#endif // MEDCOMPOSERSCENE_H
