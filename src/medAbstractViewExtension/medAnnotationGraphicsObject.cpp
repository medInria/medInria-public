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
    dtkSmartPointer<medAnnotationData> m_data;
};

medAnnotationGraphicsObject::medAnnotationGraphicsObject( QGraphicsItem * parent )
    : BaseClass( parent ), 
    d( new medAbstractViewAnnotationPrivate )
{
    this->setFlag( QGraphicsItem::ItemIsSelectable, false );
    this->setFlag( QGraphicsItem::ItemIsFocusable,  false );
    this->setFlag( QGraphicsItem::ItemIsMovable,    false );

    this->setVisible( false );
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

    thickness       = ( thickness       < 0 ? -thickness : thickness );
    return ( distanceToPlane >= -thickness ) && ( distanceToPlane < thickness );
}

QList<medAbstractView *> medAnnotationGraphicsObject::views() const
{
    QList<medAbstractView *> views;
    QGraphicsScene * scene = this->scene();
    if  (!scene) 
        return views;

    QList<QGraphicsView *> genericViews = scene->views();
    views.reserve( genericViews.size() );
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

void medAnnotationGraphicsObject::setAnnotationData( medAnnotationData * annotationData )
{
    d->m_data = annotationData;
}

medAnnotationData * medAnnotationGraphicsObject::annotationData() const
{
    return d->m_data;
}





