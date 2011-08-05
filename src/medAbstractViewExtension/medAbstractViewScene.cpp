
#include "medAbstractViewScene.h"

#include "medAnnotationGraphicsObject.h"

#include <QGraphicsSceneDragDropEvent>
#include <QSet>
#include <QWidget>

class medAbstractViewScene::medAbstractViewScenePrivate 
{
    friend class medAbstractViewScene;
};

medAbstractViewScene::medAbstractViewScene( medAbstractView * view, QWidget * parent /*= NULL */ )
    : BaseClass( parent ),
    d( new medAbstractViewScenePrivate),
    m_view (view)
{
}

medAbstractViewScene::~medAbstractViewScene()
{
    delete d;
    d = NULL;
}

void medAbstractViewScene::dragEnterEvent( QGraphicsSceneDragDropEvent * event )
{
    event->setAccepted(false);
}


