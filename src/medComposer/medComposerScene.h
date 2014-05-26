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
