#include "v3dViewCircleAnnotation.h"

#include <QDebug>
#include <QVector3D>

#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsView>

#include <vtkRenderer.h>

#include <vtkImageView2D.h>

#include "v3dView.h"
#include "v3dViewGraphicsScene.h"

class v3dViewCircleAnnotationPrivate
{
public:
    qreal     radiusWorld;
    QVector3D centerWorld;

    QString   orientation;
    int       slice;

    qreal     radiusScene;
    QPointF   centerScene;
    bool      isOnSlice;

    QColor    color;
};

v3dViewCircleAnnotation::v3dViewCircleAnnotation( v3dView       * view,
                                                  QGraphicsItem * parent )
    : BaseClass( view, parent )
    , d( new v3dViewCircleAnnotationPrivate )
{
    this->setFlag( QGraphicsItem::ItemIsSelectable, false );
    this->setFlag( QGraphicsItem::ItemIsFocusable,  false );
    this->setFlag( QGraphicsItem::ItemIsMovable,    false );

    this->setVisible( false );

    d->radiusWorld = 1.0;
    d->centerWorld = QVector3D( 0.0, 0.0, 0.0 );
    d->radiusScene = 1.0;
    d->centerScene = QPointF( 0.0, 0.0 );
    d->orientation = "Axial";
    d->slice       = 0;
    d->isOnSlice   = false;

    d->color       = QColor( "#b2b8b2" );
    d->color.setAlphaF( 0.67 );
}

v3dViewCircleAnnotation::~v3dViewCircleAnnotation()
{
    if ( d != NULL )
        delete d;
}

void v3dViewCircleAnnotation::setCenter( const QVector3D & center )
{
    d->centerWorld = center;
    this->updateSceneCoords();
}

void v3dViewCircleAnnotation::setRadius( qreal r )
{
    d->radiusWorld = r;
    this->updateSceneCoords();
}

void v3dViewCircleAnnotation::setSlice( QString orientation,
                                        int slice )
{
    v3dView * view = qobject_cast< v3dView * >( this->view() );
    vtkImageView2D * view2d = view->view2d();
    if ( view2d == NULL ||
         view2d != dynamic_cast< vtkImageView2D * >( view->currentView() ) )
        return;

    bool wasOnSlice = d->isOnSlice;

    QString viewOrientation;
    switch ( view2d->GetViewOrientation() )
    {
    case vtkImageView2D::VIEW_ORIENTATION_AXIAL:
        viewOrientation = "Axial";
        break;
    case vtkImageView2D::VIEW_ORIENTATION_CORONAL:
        viewOrientation = "Coronal";
        break;
    case vtkImageView2D::VIEW_ORIENTATION_SAGITTAL:
        viewOrientation = "Sagittal";
        break;
    default: break;
    }

    d->orientation = orientation;
    d->slice       = slice;
    d->isOnSlice = ( d->orientation == viewOrientation    &&
                     d->slice       == view2d->GetSlice() );
    this->setVisible( d->isOnSlice );

    if ( !wasOnSlice && d->isOnSlice )
        this->updateSceneCoords();

}

QRectF v3dViewCircleAnnotation::boundingRect() const
{
    QRectF circleRect( -d->radiusScene,    -d->radiusScene,
                       2 * d->radiusScene, 2 * d->radiusScene );

    QRectF sceneRect = this->scene()->views()[0]->geometry();
    // the view rect is four pixels larger than the scene rect
    sceneRect.adjust( 0.0, 0.0, -4.0, -4.0 );

    // qDebug() << "boundingRect: " << rect;
    return circleRect & sceneRect;
}

void v3dViewCircleAnnotation::paint( QPainter * painter,
                                     const QStyleOptionGraphicsItem * option,
                                     QWidget * widget )
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->save();

    painter->setPen( d->color );
    painter->setRenderHints( QPainter::Antialiasing          |
                             QPainter::SmoothPixmapTransform |
                             QPainter::TextAntialiasing      );

    painter->drawEllipse( QPointF( 0.0, 0.0 ), d->radiusScene, d->radiusScene );

    painter->restore();
}

void v3dViewCircleAnnotation::updateSceneCoords()
{
    v3dView * view = qobject_cast< v3dView * >( this->view() );
    vtkImageView2D       * view2d = view->view2d();
    v3dViewGraphicsScene * scene  =
        dynamic_cast< v3dViewGraphicsScene * >( this->scene() );
    if ( view2d == NULL || view2d != view->currentView() )
        return;

    QString viewOrientation;
    switch ( view2d->GetViewOrientation() )
    {
    case vtkImageView2D::VIEW_ORIENTATION_AXIAL:
        viewOrientation = "Axial";
        break;
    case vtkImageView2D::VIEW_ORIENTATION_CORONAL:
        viewOrientation = "Coronal";
        break;
    case vtkImageView2D::VIEW_ORIENTATION_SAGITTAL:
        viewOrientation = "Sagittal";
        break;
    default: break;
    }

    d->isOnSlice = ( d->orientation == viewOrientation    &&
                     d->slice       == view2d->GetSlice() );
    this->setVisible( d->isOnSlice );

    if ( !d->isOnSlice )
        return;

    d->centerScene = this->worldToScene( d->centerWorld );

    QVector3D vup(this->viewUp()) ;
    QVector3D v =  d->centerWorld + d->radiusWorld *
        vup.normalized();

    QPointF edgePoint( this->worldToScene( v ) );

    d->radiusScene = QVector2D( d->centerScene - edgePoint ).length();

    this->prepareGeometryChange();
    this->setPos( d->centerScene );
}
