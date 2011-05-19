#include "medAbstractViewAnnotation.h"

#include <medCore/medAbstractView.h>

#include "medAbstractViewScene.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>
#include <QVector2D>
#include <QVector3D>


class medAbstractViewAnnotation::medAbstractViewAnnotationPrivate
{
public:
};

medAbstractViewAnnotation::medAbstractViewAnnotation( medAbstractView       * view,
                                                  QGraphicsItem * parent )
    : BaseClass( parent ),
    m_view(view), 
    d( new medAbstractViewAnnotationPrivate )
{
    this->setFlag( QGraphicsItem::ItemIsSelectable, false );
    this->setFlag( QGraphicsItem::ItemIsFocusable,  false );
    this->setFlag( QGraphicsItem::ItemIsMovable,    false );

    this->setVisible( false );
}

medAbstractViewAnnotation::~medAbstractViewAnnotation()
{
    delete d;
    d = NULL;
}

bool medAbstractViewAnnotation::showIn2dView() const
{
    return true;
}

bool medAbstractViewAnnotation::showIn3dView() const
{
    return true;
}

QPointF medAbstractViewAnnotation::worldToScene( const QVector3D & worldVec ) const
{
    medAbstractViewScene * scene = qobject_cast< medAbstractViewScene *>( this->scene() );
    return scene->worldToScene( worldVec );
}

QVector3D medAbstractViewAnnotation::sceneToWorld( const QPointF & sceneVec ) const
{
    medAbstractViewScene * scene = qobject_cast< medAbstractViewScene *>( this->scene() );
    return scene->sceneToWorld( sceneVec );
}

QVector3D medAbstractViewAnnotation::viewUp() const
{
    medAbstractViewScene * scene = qobject_cast< medAbstractViewScene *>( this->scene() );
    return scene->viewUp( );
}

bool medAbstractViewAnnotation::isInSlice( const QVector3D & slicePoint, const QVector3D & sliceNormal, qreal thickness ) const
{
    return true;
}

bool medAbstractViewAnnotation::isPointInSlice( const QVector3D & testPoint, const QVector3D & slicePoint, const QVector3D & sliceNormal, qreal thickness ) const
{
    qreal distanceToPlane = QVector3D::dotProduct( (testPoint - slicePoint), sliceNormal );

    distanceToPlane = distanceToPlane / sliceNormal.length();

    thickness       = ( thickness       < 0 ? -thickness : thickness );
    return ( distanceToPlane >= -thickness ) && ( distanceToPlane < thickness );
}











