
#include "medAbstractViewScene.h"

#include "medAbstractViewAnnotation.h"

#include <QSet>
#include <QWidget>

class medAbstractViewScene::medAbstractViewScenePrivate 
{
    friend class medAbstractViewScene;

    typedef QSet< medAbstractViewAnnotation * > AnnotationsContainerType;
    AnnotationsContainerType annotations;

    typedef QList< medViewEventHandler * > EventHandlerContainerType;
    EventHandlerContainerType handlers;
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

void medAbstractViewScene::addAnnotation( medAbstractViewAnnotation * annotation )
{
    medAbstractViewScenePrivate::AnnotationsContainerType::const_iterator it( d->annotations.insert( annotation ) );
    if ( *it == annotation ){
        this->addItem( annotation );
    }
}

void medAbstractViewScene::removeAnnotation( medAbstractViewAnnotation * annotation )
{
    bool removed =  d->annotations.remove( annotation );
    if ( removed ){
        this->removeItem( annotation );
    }
}
