#include "v3dViewGraphicsScene.h"

#include <QVector2D>
#include <QVector3D>

#include <dtkCore/dtkAbstractDataImage.h>

#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkWindow.h>

#include <vtkImageView.h>
#include <vtkImageView2D.h>

#include "QVtkGraphicsView.h"
#include "v3dView.h"
#include "v3dViewCircleAnnotation.h"


class v3dViewGraphicsScenePrivate
{
    friend class v3dViewGraphicsScene;

    v3dView * view;
    bool itemsVisible;
    v3dViewCircleAnnotation * circAnn;
};

v3dViewGraphicsScene::v3dViewGraphicsScene( v3dView * view, QWidget * parent )
  : BaseClass( view, parent )
  , d( new v3dViewGraphicsScenePrivate )
{
    // view
    d->view = view;
    d->itemsVisible = ( d->view->currentView() == d->view->view2d() );

    connect( d->view, SIGNAL( sliceChanged( int, bool ) ),
             this,    SLOT( onSliceChanged( int, bool ) ) );
    connect( d->view, SIGNAL( positionChanged( const QVector3D &, bool ) ),
             this,    SLOT( onPositionChanged( const QVector3D &, bool ) ) );
    connect( d->view, SIGNAL( zoomChanged( double, bool ) ),
             this,    SLOT( onZoomChanged( double, bool ) ) );
    connect( d->view, SIGNAL( panChanged( const QVector2D &, bool ) ),
             this,    SLOT( onPanChanged( const QVector2D &, bool ) ) );
    connect( d->view, SIGNAL( cameraChanged( const QVector3D &,
                                             const QVector3D &,
                                             const QVector3D &,
                                             double, bool ) ),
             this,    SLOT( onCameraChanged( const QVector3D &,
                                             const QVector3D &,
                                             const QVector3D &,
                                             double, bool ) ) );

    QVtkGraphicsView * qView =
        dynamic_cast< QVtkGraphicsView *>( view->receiverWidget() );
    connect( qView, SIGNAL( sizeChanged (int, int) ),
             this,  SLOT( onSizeChanged (int, int) ) );

    connect( this, SIGNAL( sceneRectChanged( const QRectF & ) ),
             this, SLOT( onSceneRectChanged( const QRectF & ) ) );
    

    // user annotations
    d->circAnn = new v3dViewCircleAnnotation( d->view );
    d->circAnn->setCenter( QVector3D( -7.0, 118.0, 1356.0 ) );
    d->circAnn->setRadius( 6.23 );
    d->circAnn->setSlice( "Axial", 123 );
    this->addItem( d->circAnn );

    // this->setItemsVisible( d->itemsVisible );
}

v3dViewGraphicsScene::~v3dViewGraphicsScene()
{
    // do we need to delete the members?
    delete d;
}

void v3dViewGraphicsScene::onImageDataChanged(
    dtkAbstractDataImage * imageData )
{
    Q_UNUSED( imageData );
    d->circAnn->updateSceneCoords();
}

void v3dViewGraphicsScene::onSceneRectChanged( const QRectF & rect )
{
    Q_UNUSED( rect );
    d->circAnn->updateSceneCoords();
}

void v3dViewGraphicsScene::onSliceChanged( int slice, bool propagate )
{
    Q_UNUSED( slice );
    Q_UNUSED( propagate );
    d->circAnn->updateSceneCoords();
}

void v3dViewGraphicsScene::onPositionChanged( const QVector3D & position,
                                              bool propagate )
{
    Q_UNUSED( position );
    Q_UNUSED( propagate );
    d->circAnn->updateSceneCoords();
}

void v3dViewGraphicsScene::onZoomChanged( double zoom, bool propagate )
{
    Q_UNUSED( zoom );
    Q_UNUSED( propagate );
    d->circAnn->updateSceneCoords();
}

void v3dViewGraphicsScene::onPanChanged( const QVector2D & pan, bool propagate )
{
    Q_UNUSED( pan );
    Q_UNUSED( propagate );
    d->circAnn->updateSceneCoords();
}

void v3dViewGraphicsScene::onCameraChanged( const QVector3D & position,
                                            const QVector3D & viewup,
                                            const QVector3D & focal,
                                            double parallelScale,
                                            bool propagate )
{
    Q_UNUSED( position );
    Q_UNUSED( viewup );
    Q_UNUSED( focal );
    Q_UNUSED( parallelScale );
    Q_UNUSED( propagate );

    d->circAnn->updateSceneCoords();
}

void v3dViewGraphicsScene::onSizeChanged( int width, int height)
{
    Q_UNUSED( width );
    Q_UNUSED( height );
    d->circAnn->updateSceneCoords();
}

void v3dViewGraphicsScene::onOrientationChanged( const QString & name )
{
    this->setItemsVisible( name != "3D" );

    d->circAnn->updateSceneCoords();
}


void v3dViewGraphicsScene::showItems()
{
    this->setItemsVisible( true );
}

void v3dViewGraphicsScene::hideItems()
{
    this->setItemsVisible( false );
}

void v3dViewGraphicsScene::setItemsVisible( bool state )
{
    d->itemsVisible = state;

    d->circAnn->setVisible( d->itemsVisible );
}

QPointF v3dViewGraphicsScene::worldToScene( const QVector3D & worldVec ) const
{
    vtkRenderer * ren = d->view->currentView()->GetRenderer();

    /* get physical window dimensions */
    vtkWindow * win = ren->GetVTKWindow();

    if ( !win )
        return QPointF();

    double wx = worldVec.x();
    double wy = worldVec.y();
    double wz = worldVec.z();

    ren->WorldToView( wx, wy, wz );

    /* get physical window dimensions */
    const int * size = win->GetSize();
    int sizex = size[0];
    int sizey = size[1];

    const double * viewport = ren->GetViewport( );

    double dx = (wx + 1.0) *
        (sizex*(viewport[2]-viewport[0])) / 2.0 +
        sizex*viewport[0];
    double dy = (wy + 1.0) *
        (sizey*(viewport[3]-viewport[1])) / 2.0 +
        sizey*viewport[1];

    // Convert VTK display coordinates to Qt (flipped in Y)
    return QPointF( dx, sizey - 1 - dy );
}

QVector3D v3dViewGraphicsScene::sceneToWorld( const QPointF & sceneVec) const
{
    vtkRenderer * ren = d->view->currentView()->GetRenderer();

    /* get physical window dimensions */
    vtkWindow * win = ren->GetVTKWindow();

    if ( !win )
        return QVector3D();

    const int * size = win->GetSize();
    int sizex = size[0];
    int sizey = size[1];

    const double * viewport = ren->GetViewport( );

    // Convert Qt display coordinates to VTK (flipped in Y)
    const double dx = sceneVec.x();
    const double dy = sizey - 1 - sceneVec.y();

    double vx = 2.0 * (dx - sizex*viewport[0])/
            (sizex*(viewport[2]-viewport[0])) - 1.0;
    double vy = 2.0 * (dy - sizey*viewport[1])/
            (sizey*(viewport[3]-viewport[1])) - 1.0;
    double vz = 0.;

    ren->ViewToWorld(vx,vy,vz);
    return QVector3D( vx, vy, vz );

}

QVector3D v3dViewGraphicsScene::viewPlaneNormal() const
{
    vtkRenderer * ren = d->view->currentView()->GetRenderer();
    double vpn[3];
    ren->GetActiveCamera()->GetViewPlaneNormal( vpn );
    return QVector3D( vpn[0], vpn[1], vpn[2] );
}

QVector3D v3dViewGraphicsScene::viewUp() const
{
    vtkRenderer * ren = d->view->currentView()->GetRenderer();
    double vup[3];
    ren->GetActiveCamera()->GetViewUp( vup );
    return QVector3D( vup[0], vup[1], vup[2] );
}

bool v3dViewGraphicsScene::isScene2D() const
{
    return (d->view->currentView() == d->view->view2d() );
}

QVector3D v3dViewGraphicsScene::sceneToImagePos( const QPointF & point) const
{
    if (! this->isScene2D()) {
        return QVector3D();
    }

    int xy[2];
    xy[0] = point.x();
    xy[1] = point.y();

    double worldPosition[3];
    d->view->view2d()->GetWorldCoordinatesFromDisplayPosition ( xy, worldPosition);
    return QVector3D( worldPosition[0], worldPosition[1], worldPosition[2] );

}
