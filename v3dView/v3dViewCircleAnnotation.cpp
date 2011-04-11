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
    v3dView * view;

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
    : BaseClass( parent )
    , d( new v3dViewCircleAnnotationPrivate )
{
    this->setFlag( QGraphicsItem::ItemIsSelectable, false );
    this->setFlag( QGraphicsItem::ItemIsFocusable,  false );
    this->setFlag( QGraphicsItem::ItemIsMovable,    false );

    this->setVisible( false );

    d->view        = view;

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
    vtkImageView2D * view2d = d->view->view2d();
    if ( view2d == NULL ||
         view2d != dynamic_cast< vtkImageView2D * >( d->view->currentView() ) )
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
    vtkImageView2D       * view2d = d->view->view2d();
    v3dViewGraphicsScene * scene  =
        dynamic_cast< v3dViewGraphicsScene * >( this->scene() );
    if ( view2d == NULL || view2d != d->view->currentView() )
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

    qreal posWorld[3];
    posWorld[0] = d->centerWorld.x();
    posWorld[1] = d->centerWorld.y();
    posWorld[2] = d->centerWorld.z();
    // Convert current point in world coordinates to display
    // coordinates (VTK).  Setting the world point is only necessary
    // to perform the coordinate conversion - it has no side effects
    // (hopefully!)
    vtkRenderer * ren = d->view->currentView()->GetRenderer();
    ren->SetWorldPoint( const_cast< qreal * >( posWorld ) );
    ren->WorldToDisplay();
    qreal posDisplay[3];
    ren->GetDisplayPoint( posDisplay );

    // Convert VTK display coordinates to Qt (flipped in Y)
    qreal ysize = scene->sceneRect().height();
    QPointF p( posDisplay[0], ysize - 1.0 - posDisplay[1] );

    d->centerScene = p;

    qreal viewUp[3];
    d->view->cameraUp( viewUp );
    QVector3D v =  d->centerWorld + d->radiusWorld *
        QVector3D( viewUp[0], viewUp[1], viewUp[2] ).normalized();

    posWorld[0] = v.x();
    posWorld[1] = v.y();
    posWorld[2] = v.z();
    // Convert current point in world coordinates to display
    // coordinates (VTK).  Setting the world point is only necessary
    // to perform the coordinate conversion - it has no side effects
    // (hopefully!)
    ren->SetWorldPoint( const_cast< qreal * >( posWorld ) );
    ren->WorldToDisplay();
    ren->GetDisplayPoint( posDisplay );

    // Convert VTK display coordinates to Qt (flipped in Y)
    p = QPointF( posDisplay[0], ysize - 1.0 - posDisplay[1] );


    d->radiusScene = QVector2D( d->centerScene - p ).length();

    this->prepareGeometryChange();
    this->setPos( d->centerScene );
}
