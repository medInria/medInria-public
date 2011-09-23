#include "medAnnotationGraphicsObject.h"

#include <medCore/medAbstractView.h>

#include "medAbstractViewScene.h"
#include "medAnnotationData.h"

#include <dtkCore/dtkSmartPointer.h>

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>
#include <QVector2D>
#include <QVector3D>


class medAnnotationGraphicsObject::medAbstractViewAnnotationPrivate
{
public:
    dtkSmartPointer<const medAnnotationData> m_data;
};

medAnnotationGraphicsObject::medAnnotationGraphicsObject( QGraphicsItem * parent )
    : BaseClass( parent ), 
    d( new medAbstractViewAnnotationPrivate )
{
    this->setFlag( QGraphicsItem::ItemIsSelectable, false );
    this->setFlag( QGraphicsItem::ItemIsFocusable,  false );
    this->setFlag( QGraphicsItem::ItemIsMovable,    false );

    this->setVisible( true );
}

medAnnotationGraphicsObject::~medAnnotationGraphicsObject()
{
    delete d;
    d = NULL;
}

bool medAnnotationGraphicsObject::showIn2dView() const
{
    return true;
}

bool medAnnotationGraphicsObject::showIn3dView() const
{
    return true;
}

QPointF medAnnotationGraphicsObject::worldToScene( const QVector3D & worldVec ) const
{
    medAbstractViewScene * scene = qobject_cast< medAbstractViewScene *>( this->scene() );
    return scene->worldToScene( worldVec );
}

QVector3D medAnnotationGraphicsObject::sceneToWorld( const QPointF & sceneVec ) const
{
    medAbstractViewScene * scene = qobject_cast< medAbstractViewScene *>( this->scene() );
    return scene->sceneToWorld( sceneVec );
}

QVector3D medAnnotationGraphicsObject::viewUp() const
{
    medAbstractViewScene * scene = qobject_cast< medAbstractViewScene *>( this->scene() );
    return scene->viewUp( );
}

bool medAnnotationGraphicsObject::isInSlice( const QVector3D & slicePoint, const QVector3D & sliceNormal, qreal thickness ) const
{
    return true;
}

bool medAnnotationGraphicsObject::isPointInSlice( const QVector3D & testPoint, const QVector3D & slicePoint, const QVector3D & sliceNormal, qreal thickness ) const
{
    qreal distanceToPlane = QVector3D::dotProduct( (testPoint - slicePoint), sliceNormal );

    distanceToPlane = distanceToPlane / sliceNormal.length();

    return ( distanceToPlane >= -thickness ) && ( distanceToPlane < thickness );
}

bool medAnnotationGraphicsObject::isPointInCurrentSlice( const QVector3D & testPoint ) const
{
    medAbstractViewScene * scene = qobject_cast< medAbstractViewScene *>( this->scene() );
    return this->isPointInSlice( testPoint, scene->viewCenter(), scene->viewPlaneNormal(), 0.5*scene->sliceThickness() );
}

QList<medAbstractView *> medAnnotationGraphicsObject::views() const
{
    QList<medAbstractView *> views;
    QGraphicsScene * scene = this->scene();
    if  (!scene) 
        return views;

    QList<QGraphicsView *> genericViews = scene->views();
#if ( QT_VERSION >= QT_VERSION_CHECK( 4, 7, 0 ) )
    views.reserve( genericViews.size() );
#endif
    foreach( QGraphicsView * view,  genericViews ) {
        if ( medAbstractView * mview = qobject_cast< medAbstractView * >(view) ) 
            views.push_back( mview );
    }
    return views;
}

medAbstractView * medAnnotationGraphicsObject::view() const
{
   QList<medAbstractView *> allMViews = views();
   if ( allMViews.isEmpty() ) {
       return NULL;
   } else {
       Q_ASSERT( allMViews.size() == 1 );
       return allMViews.at(0);
   }
}

void medAnnotationGraphicsObject::setAnnotationData( const medAnnotationData * annotationData )
{
    if ( d->m_data ) {
        disconnect( d->m_data, SIGNAL(dataModified()), this, SLOT(onDataModified()) );
    }
    d->m_data = annotationData;
    if ( d->m_data ) {
        connect( d->m_data, SIGNAL(dataModified()), this, SLOT(onDataModified()) );
    }
}

const medAnnotationData * medAnnotationGraphicsObject::annotationData() const
{
    return d->m_data;
}

QVariant medAnnotationGraphicsObject::itemChange( GraphicsItemChange change, const QVariant & value )
{
    switch(change) {
    case QGraphicsItem::ItemSceneHasChanged : 
        {
            QGraphicsScene * scenePtr = value.value<QGraphicsScene *>();
            this->onSceneChanged( scenePtr );
        }
        break;
    default:
        break;
    }
    // The base class does nothing, returning value.
    return QGraphicsItem::itemChange(change,value);
}

QVariant medAnnotationGraphicsObject::annotationItemChange( AnnotationGraphicsItemChange change, const QVariant & value )
{
    switch(change) {
    case medAnnotationGraphicsObject::SceneCameraChanged : 
        {
            this->onSceneCameraChanged();
        }
        break;
    case medAnnotationGraphicsObject::SceneOrientationChanged : 
        {
            this->onSceneOrientationChanged();
        }
        break;
    default:
        break;
    }
    return value;
}

void medAnnotationGraphicsObject::onSceneChanged( QGraphicsScene * scene )
{

}

void medAnnotationGraphicsObject::onSceneCameraChanged()
{

}

void medAnnotationGraphicsObject::onSceneOrientationChanged()
{

}

void medAnnotationGraphicsObject::onDataModified()
{
    this->scene()->update( this->boundingRect() );
}



