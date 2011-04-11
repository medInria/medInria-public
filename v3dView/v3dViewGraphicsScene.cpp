#include "v3dViewGraphicsScene.h"

#include <QVector2D>
#include <QVector3D>

#include <dtkCore/dtkAbstractDataImage.h>

#include <vtkRenderer.h>

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
  : QGraphicsScene( parent )
  , d( new v3dViewGraphicsScenePrivate )
{
    // view
    d->view = view;
    d->itemsVisible = ( d->view->currentView() == d->view->view2d() );

    // connect( d->view, SIGNAL( sliceChanged( int ) ),
    //          this,    SLOT( onSliceChanged( int ) ) );
    connect( d->view, SIGNAL( positionChanged( const QVector3D & ) ),
             this,    SLOT( onPositionChanged( const QVector3D & ) ) );
    connect( d->view, SIGNAL( zoomChanged( double ) ),
             this,    SLOT( onZoomChanged( double ) ) );
    connect( d->view, SIGNAL( panChanged( const QVector2D &) ),
             this,    SLOT( onPanChanged( const QVector2D &)) );
    connect( d->view, SIGNAL( cameraChanged( const QVector3D &,
                                             const QVector3D &,
                                             const QVector3D &, double ) ),
             this,    SLOT( onCameraChanged( const QVector3D &,
                                             const QVector3D &,
                                             const QVector3D &, double ) ) );

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

void v3dViewGraphicsScene::onSliceChanged( int slice )
{
    Q_UNUSED( slice );
    d->circAnn->updateSceneCoords();
}

void v3dViewGraphicsScene::onPositionChanged( const QVector3D &position )
{
    Q_UNUSED( position );
    d->circAnn->updateSceneCoords();
}

void v3dViewGraphicsScene::onZoomChanged( double zoom )
{
    Q_UNUSED( zoom );
    d->circAnn->updateSceneCoords();
}

void v3dViewGraphicsScene::onPanChanged( const QVector2D &pan )
{
    Q_UNUSED( pan );
    d->circAnn->updateSceneCoords();
}

void v3dViewGraphicsScene::onCameraChanged( const QVector3D & position,
                                            const QVector3D & viewup,
                                            const QVector3D & focal,
                                            double parallelScale )
{
    Q_UNUSED( position );
    Q_UNUSED( viewup );
    Q_UNUSED( focal );
    Q_UNUSED( parallelScale );

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
