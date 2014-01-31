/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include <medVtkViewNavigator.h>

#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkImageViewCollection.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

#include <QVTKWidget2.h>

#include <medVtkViewBackend.h>
#include <medImageViewFactory.h>
#include <medBoolGroupParameter.h>
#include <medBoolParameter.h>

class medVtkViewNavigatorPrivate
{
    public:
    medAbstractImageView *medVtkView;
    vtkRenderWindow *render;
    QVTKWidget2 *qvtkWidget;
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;
    vtkRenderer *renderer2d;
    vtkRenderer *renderer3d;
    vtkImageViewCollection *collection;

    medImageView::Orientation orientation;


};

medVtkViewNavigator::medVtkViewNavigator(medAbstractImageView* parent) :
    medAbstractImageViewNavigator(parent), d(new medVtkViewNavigatorPrivate)
{
    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;

    d->renderer2d = d->view2d->GetRenderer();
    d->renderer3d = d->view3d->GetRenderer();

    d->collection = vtkImageViewCollection::New();
    d->collection->AddItem(d->view2d);
    d->collection->AddItem(d->view3d);
    d->render = backend->renWin;
    d->qvtkWidget = backend->qvtkWidget;

    d->orientation = medImageView::VIEW_ORIENTATION_AXIAL;
}

medVtkViewNavigator::~medVtkViewNavigator()
{
    delete d;
}

QString  medVtkViewNavigator::identifier() const
{
    return medVtkViewNavigator::s_identifier();
}

QString  medVtkViewNavigator::s_identifier()
{
    return "medVtkViewNavigator";
}

bool medVtkViewNavigator::registered()
{
    medImageViewFactory * factory = medImageViewFactory::instance();
    return factory->registerNavigator<medVtkViewNavigator>(medVtkViewNavigator::s_identifier(),
                                                           QStringList() << "medVtkView");
}

QString medVtkViewNavigator::description() const
{
    return "Navigator to interact with a medVtkView";
}

QVector3D medVtkViewNavigator::positionBeingViewed() const
{

}

void medVtkViewNavigator::setOrientation(medImageView::Orientation orientation)
{

}

medImageView::Orientation medVtkViewNavigator::orientaion() const
{

}

void medVtkViewNavigator::setCamera(const QVector3D &position,
                       const QVector3D &viewup,
                       const QVector3D &focal,
                       double parallelScale)
{

}

void medVtkViewNavigator::camera(QVector3D &position,
                    QVector3D &viewup,
                    QVector3D &focal,
                    double &parallelScale) const
{



}

void medVtkViewNavigator::cameraUp ( double *coordinates )
{
    if (d->orientation != medImageView::VIEW_ORIENTATION_3D)
    {
        vtkCamera *camera = d->renderer2d->GetActiveCamera();
        camera->GetViewUp (coordinates);
    }
    else
    {
        vtkCamera *camera = d->renderer3d->GetActiveCamera();
        camera->GetViewUp (coordinates);
    }
}

void medVtkViewNavigator::cameraPosition ( double *coordinates )
{
    if ( d->orientation != medImageView::VIEW_ORIENTATION_3D )
    {
        vtkCamera *camera = d->renderer2d->GetActiveCamera();
        camera->GetPosition(coordinates);
    }
    else
    {
        vtkCamera *camera = d->renderer3d->GetActiveCamera();
        camera->GetPosition(coordinates);
    }
}

void medVtkViewNavigator::cameraFocalPoint ( double *coordinates )
{
    if ( d->orientation != medImageView::VIEW_ORIENTATION_3D )
    {
        vtkCamera *camera = d->renderer2d->GetActiveCamera();
        camera->GetFocalPoint(coordinates);
    }
    else
    {
        vtkCamera *camera = d->renderer3d->GetActiveCamera();
        camera->GetFocalPoint(coordinates);
    }
}


void medVtkViewNavigator::bounds(float& xmin, float& xmax, float& ymin, float& ymax, float& zmin, float& zmax)
{
    double bounds[6];
    switch(d->orientation)
    {
    case medImageView::VIEW_ORIENTATION_3D:
        d->renderer3d->ComputeVisiblePropBounds(bounds);
        xmin = bounds[0];
        xmax = bounds[1];
        ymin = bounds[2];
        ymax = bounds[3];
        zmin = bounds[4];
        zmax = bounds[5];
        break;
    case medImageView::VIEW_ORIENTATION_AXIAL:
        d->renderer2d->ComputeVisiblePropBounds(bounds);
        xmin = bounds[0];
        xmax = bounds[1];
        ymin = bounds[2];
        ymax = bounds[3];
        zmin = bounds[4];
        zmax = bounds[5];
        break;
    case medImageView::VIEW_ORIENTATION_CORONAL:
        d->renderer2d->ComputeVisiblePropBounds(bounds);
        xmin = bounds[0];
        xmax = bounds[1];
        ymin = bounds[2];
        ymax = bounds[3];
        zmin = bounds[4];
        zmax = bounds[5];
        break;
    case medImageView::VIEW_ORIENTATION_SAGITTAL:
        d->renderer2d->ComputeVisiblePropBounds(bounds);
        xmin = bounds[0];
        xmax = bounds[1];
        ymin = bounds[2];
        ymax = bounds[3];
        zmin = bounds[4];
        zmax = bounds[5];
        break;
    }
}

void medVtkViewNavigator::setCameraPosition(double x, double y, double z)
{
    if (d->orientation != medImageView::VIEW_ORIENTATION_3D)
        return;

    vtkCamera *camera = d->renderer3d->GetActiveCamera();
    camera->SetPosition (x, y, z);
    d->renderer3d->ResetCameraClippingRange();
}

void medVtkViewNavigator::setCameraClippingRange(double nearRange, double farRange)
{
    if (d->orientation != medImageView::VIEW_ORIENTATION_3D)
        return;

    vtkCamera *camera = d->renderer3d->GetActiveCamera();
    camera->SetClippingRange (nearRange, farRange);
}

//TODO is that usefull ?
QString medVtkViewNavigator::cameraProjectionMode()
{
    vtkCamera *camera = NULL;

    if (d->orientation != medImageView::VIEW_ORIENTATION_3D)
        camera = d->renderer2d->GetActiveCamera();
    else
        camera = d->renderer3d->GetActiveCamera();

    if ( !camera )
        return QString("None");

    if (camera->GetParallelProjection())
        return QString ("Parallel");
    else
        return QString ("Perspective");
}

double medVtkViewNavigator::cameraViewAngle()
{
    vtkCamera *camera = NULL;

    if (d->orientation != medImageView::VIEW_ORIENTATION_3D)
        camera = d->renderer2d->GetActiveCamera();
    else
        camera = d->renderer3d->GetActiveCamera();

    if ( !camera )
        return 0.0;
    else
        return camera->GetViewAngle();
}

double medVtkViewNavigator::cameraZoom()
{
    vtkImageView *view = NULL;

    if (d->orientation != medImageView::VIEW_ORIENTATION_3D)
        view = d->view2d;

    else  view = d->view3d;

    if ( !view )
        return 1.0;
    else
        return view->GetZoom();
}
