#include "v3dViewCircleAnnotation.h"

#include <QDebug>
#include <QVector3D>

#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsView>

#include <vtkRenderer.h>

#include <vtkImageView2D.h>

#include <dtkCore/dtkSmartPointer.h>

#include "v3dView.h"
#include "v3dViewGraphicsScene.h"

v3dViewCircleAnnotationData::v3dViewCircleAnnotationData()
{
    radiusWorld = 1.0;
    centerWorld = QVector3D( 0.0, 0.0, 0.0 );
    orientation = "Axial";
    slice       = 0;
    color       = QColor( "#b2b8b2" );
    color.setAlphaF( 0.67 );
}

v3dViewCircleAnnotationData::~v3dViewCircleAnnotationData()
{

}

void v3dViewCircleAnnotationData::setCenter( const QVector3D & center )
{
    centerWorld = center;
    emit dataModified();
}

void v3dViewCircleAnnotationData::setRadius( qreal r )
{
    radiusWorld = r;
    emit dataModified();
}

void v3dViewCircleAnnotationData::setSlice( QString orientation, int slice )
{
    orientation = orientation;
    slice       = slice;
    emit dataModified();
}




class v3dViewCircleAnnotationPrivate
{
public:
    dtkSmartPointer<v3dViewCircleAnnotationData> dataObject;
    qreal     radiusScene;
    QPointF   centerScene;
    bool      isOnSlice;
};

v3dViewCircleAnnotation::v3dViewCircleAnnotation( v3dViewCircleAnnotationData * data,
                                                  QGraphicsItem * parent )
    : BaseClass( parent )
    , d( new v3dViewCircleAnnotationPrivate )
{
    d->dataObject = data;

    this->setVisible( false );

    d->radiusScene = 1.0;
    d->centerScene = QPointF( 0.0, 0.0 );
    d->isOnSlice   = false;

    connect( data, SIGNAL( dataModified() ), this, SLOT( updateSceneCoords() ) );
}

v3dViewCircleAnnotation::~v3dViewCircleAnnotation()
{
    if ( d != NULL )
        delete d;
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

    painter->setPen( d->dataObject->color );
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

    d->isOnSlice = ( d->dataObject->orientation == viewOrientation    &&
                     d->dataObject->slice       == view2d->GetSlice() );
    this->setVisible( d->isOnSlice );

    if ( !d->isOnSlice )
        return;

    d->centerScene = this->worldToScene( d->dataObject->centerWorld );

    QVector3D vup(this->viewUp()) ;
    QVector3D v =  d->dataObject->centerWorld + d->dataObject->radiusWorld *
        vup.normalized();

    QPointF edgePoint( this->worldToScene( v ) );

    d->radiusScene = QVector2D( d->centerScene - edgePoint ).length();

    this->prepareGeometryChange();
    this->setPos( d->centerScene );
}
