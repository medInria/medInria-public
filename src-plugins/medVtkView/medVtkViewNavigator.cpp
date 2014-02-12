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
#include <vtkCamera.h>

#include <QVTKWidget2.h>

#include <medVtkViewBackend.h>
#include <medImageViewFactory.h>
#include <medBoolGroupParameter.h>
#include <medBoolParameter.h>
#include <medDoubleParameter.h>
#include <medVector2DParameter.h>
#include <medVector3DParameter.h>
#include <medDoubleParameter.h>

class medVtkViewNavigatorPrivate
{
    public:
    medAbstractImageView *medVtkView;
    vtkRenderWindow *render;
    QVTKWidget2 *qvtkWidget;
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;
    vtkImageView *currentView;


    vtkRenderer *renderer2d;
    vtkRenderer *renderer3d;
    vtkRenderWindow *renWin;
    vtkImageViewCollection *collection;

    medImageView::Orientation orientation;

    QWidget *widgetForToolBox;
    QWidget *widgetForToolBar;

    medBoolGroupParameter *orientationParameter;
    medBoolParameter *oAxialParameter;
    medBoolParameter *oSagittalParameter;
    medBoolParameter *oCoronalParameter;
    medBoolParameter *o3dParameter;

    medDoubleParameter *zoomParameter;

    medVector2DParameter *panParameter;

    medVector3DParameter *cameraPositionParameter;
    medVector3DParameter *cameraViewUpParameter;
    medVector3DParameter *cameraFocalParameter;
    medDoubleParameter   *cameraParallelScaleParameter;

};

medVtkViewNavigator::medVtkViewNavigator(medAbstractImageView* parent) :
    medAbstractImageViewNavigator(parent), d(new medVtkViewNavigatorPrivate)
{
    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;
    d->renWin = backend->renWin;

    d->renderer2d = d->view2d->GetRenderer();
    d->renderer3d = d->view3d->GetRenderer();

    d->collection = vtkImageViewCollection::New();
    d->collection->AddItem(d->view2d);
    d->collection->AddItem(d->view3d);
    d->render = backend->renWin;
    d->qvtkWidget = backend->qvtkWidget;

    d->currentView = NULL;

    d->orientationParameter = new medBoolGroupParameter("Orientation", parent);
    d->orientationParameter->setPushButtonDirection(QBoxLayout::LeftToRight);
    d->orientationParameter->getLabel()->hide();


    d->oAxialParameter = new medBoolParameter("axial", parent);
    d->oAxialParameter->setIcon(QIcon(":/icons/AxialIcon.png"));
    d->oAxialParameter->setIconSize(QSize(64,64));
    connect(d->oAxialParameter, SIGNAL(valueChanged(bool)),
            this, SLOT(setAxial(bool)));

    d->oCoronalParameter = new medBoolParameter("coronal", parent);
    d->oCoronalParameter->setIcon(QIcon(":/icons/CoronalIcon.png"));
    d->oCoronalParameter->setIconSize(QSize(64,64));
    connect(d->oCoronalParameter, SIGNAL(valueChanged(bool)),
            this, SLOT(setCoronal(bool)));

    d->oSagittalParameter = new medBoolParameter("sagittal", parent);
    d->oSagittalParameter->setIcon(QIcon(":/icons/SagittalIcon.png"));
    d->oSagittalParameter->setIconSize(QSize(64,64));
    connect(d->oSagittalParameter, SIGNAL(valueChanged(bool)),
            this, SLOT(setSagittal(bool)));

    d->o3dParameter = new medBoolParameter("3d", parent);
    d->o3dParameter->setIcon(QIcon(":/icons/3DIcon.png"));
    d->o3dParameter->setIconSize(QSize(64,64));
    connect(d->o3dParameter, SIGNAL(valueChanged(bool)),
            this, SLOT(set3d(bool)));

    d->orientationParameter->addParameter(d->oAxialParameter);
    d->orientationParameter->addParameter(d->oCoronalParameter);
    d->orientationParameter->addParameter(d->oSagittalParameter);
    d->orientationParameter->addParameter(d->o3dParameter);
    d->oAxialParameter->setValue(true);

    d->zoomParameter = new medDoubleParameter("zoom");
    connect(d->zoomParameter, SIGNAL(valueChanged(double)),this, SLOT(setZoom(double)));
    connect(parent, SIGNAL(zoomChanged(double)), d->zoomParameter, SLOT(setValue(double)));
    d->zoomParameter->setValue(1);

    d->panParameter = new medVector2DParameter("pan");
    connect(d->panParameter, SIGNAL(valueChanged(QVector2D)),this, SLOT(setPan(QVector2D)));
    connect(parent, SIGNAL(panChanged(QVector2D)), d->panParameter, SLOT(setValue(QVector2D)));

    d->cameraPositionParameter = new medVector3DParameter("cameraPosition");
    d->cameraViewUpParameter = new medVector3DParameter("cameraViewUp");
    d->cameraFocalParameter = new medVector3DParameter("cameraFocal");
    d->cameraParallelScaleParameter = new medDoubleParameter("cameraParallelScale");

    connect(d->cameraPositionParameter, SIGNAL(valueChanged(QVector3D)), this, SLOT(setCameraPosition(QVector3D)));
    connect(d->cameraViewUpParameter, SIGNAL(valueChanged(QVector3D)), this, SLOT(setCameraUp(QVector3D)));
    connect(d->cameraFocalParameter, SIGNAL(valueChanged(QVector3D)), this, SLOT(setCameraFocalPoint(QVector3D)));
    connect(d->cameraParallelScaleParameter, SIGNAL(valueChanged(double)), this, SLOT(setCameraParallelScale(double)));
    connect(parent, SIGNAL(cameraChanged(QVector3D,QVector3D,QVector3D,double)),
            this,SLOT(updateCameraParam(QVector3D,QVector3D,QVector3D,double)));

    d->widgetForToolBar = NULL;
    d->widgetForToolBox = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(d->widgetForToolBox);
    layout->addWidget(d->orientationParameter->getLabel());
    layout->addWidget(d->orientationParameter->getPushButtonGroup());

    //TODO GPR-RDE: better solution?
    connect(this, SIGNAL(orientationChanged()), parent, SIGNAL(orientationChanged()));

}

medVtkViewNavigator::~medVtkViewNavigator()
{
    delete d;
}

QList<medAbstractParameter*> medVtkViewNavigator::parameters()
{
    QList<medAbstractParameter*> params;
    params.append(d->orientationParameter);
    params.append(d->zoomParameter);
    params.append(d->panParameter);
    params.append(d->cameraPositionParameter);
    params.append(d->cameraViewUpParameter);
    params.append(d->cameraFocalParameter);
    params.append(d->cameraParallelScaleParameter);

    return params;
}

QString  medVtkViewNavigator::identifier() const
{
    return medVtkViewNavigator::s_identifier();
}

QString  medVtkViewNavigator::s_identifier()
{
    return "medVtkViewNavigator";
}

QStringList medVtkViewNavigator::handled(void) const
{
    return QStringList() << "medVtkView";
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

QWidget* medVtkViewNavigator::widgetForToolBox() const
{
    return d->widgetForToolBox;
}

QWidget* medVtkViewNavigator::widgetForToolBar() const
{
    return d->widgetForToolBar;
}

QVector3D medVtkViewNavigator::positionBeingViewed() const
{
    return QVector3D(0.0,0.0,0.0);
}

double medVtkViewNavigator::zoom() const
{
    return  d->currentView->GetZoom();
}

void medVtkViewNavigator::setZoom(double zoom)
{
    d->currentView->SetZoom(zoom);

    //TODO GPR: find a better way to update the view
    d->currentView->Render();
}

QVector2D medVtkViewNavigator::pan() const
{
    const double* pan = d->view2d->GetPan();

    QVector2D qpan(pan[0], pan[1]);
    return qpan;
}

void medVtkViewNavigator::setPan(const QVector2D &pan)
{
    double stdpan[2] = {pan.x(), pan.y()};
    d->view2d->SetPan(stdpan);

    d->view2d->Render();
}

void medVtkViewNavigator::camera(QVector3D &position,
                    QVector3D &viewup,
                    QVector3D &focal,
                    double &parallelScale) const
{

}

void medVtkViewNavigator::setCamera(const QVector3D &position,
                       const QVector3D &viewup,
                       const QVector3D &focal,
                       double parallelScale)
{
    setCameraPosition(position);
    setCameraUp(viewup);
    setCameraFocalPoint(focal);
    setCameraParallelScale(parallelScale);
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

void medVtkViewNavigator::setCameraUp(const QVector3D& viewup)
{
    double vup[3];

    vup[0] = viewup.x();
    vup[1] = viewup.y();
    vup[2] = viewup.z();

    d->renderer3d->GetActiveCamera()->SetViewUp ( vup );
    d->qvtkWidget->update();
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

void medVtkViewNavigator::setCameraPosition(double x, double y, double z)
{
    if (d->orientation != medImageView::VIEW_ORIENTATION_3D)
        return;

    vtkCamera *camera = d->renderer3d->GetActiveCamera();
    camera->SetPosition (x, y, z);
    d->renderer3d->ResetCameraClippingRange();
}

void medVtkViewNavigator::setCameraPosition(const QVector3D& position)
{
    setCameraPosition(position.x(), position.y(), position.z());
    d->qvtkWidget->update();
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

void medVtkViewNavigator::setCameraFocalPoint(const QVector3D& focal)
{
    double foc[3];

    foc[0] = focal.x();
    foc[1] = focal.y();
    foc[2] = focal.z();

    d->renderer3d->GetActiveCamera()->SetFocalPoint ( foc );
    d->qvtkWidget->update();
}

void medVtkViewNavigator::setCameraParallelScale(double parallelScale)
{
    d->renderer3d->GetActiveCamera()->SetParallelScale ( parallelScale );
    d->qvtkWidget->update();
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

void medVtkViewNavigator::updateCameraParam(const QVector3D& postion,const QVector3D& viewUp,const QVector3D& focal,double parallelScale)
{
    d->cameraPositionParameter->setValue(postion);
    d->cameraViewUpParameter->setValue(viewUp);
    d->cameraFocalParameter->setValue(focal);
    d->cameraParallelScaleParameter->setValue(parallelScale);
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

void medVtkViewNavigator::setOrientation(medImageView::Orientation orientation)
{
    if (orientation == d->orientation)
        return;

    switch(orientation)
    {
    case medImageView::VIEW_ORIENTATION_3D:
        d->o3dParameter->setValue(true);
        break;
    case medImageView::VIEW_ORIENTATION_AXIAL:
        d->oAxialParameter->setValue(true);
        break;
    case medImageView::VIEW_ORIENTATION_CORONAL:
        d->oCoronalParameter->setValue(true);
        break;
    case medImageView::VIEW_ORIENTATION_SAGITTAL:
        d->oSagittalParameter->setValue(true);
        break;
    }

    d->orientation = orientation;
}

medImageView::Orientation medVtkViewNavigator::orientation() const
{
    return d->orientation;
}

void medVtkViewNavigator::setAxial(bool axial)
{
    if(axial)
        this->_prvt_setOrientation(medImageView::VIEW_ORIENTATION_AXIAL);
}

void medVtkViewNavigator::setCoronal(bool coronal)
{
    if(coronal)
        this->_prvt_setOrientation(medImageView::VIEW_ORIENTATION_CORONAL);
}

void medVtkViewNavigator::setSagittal(bool sagittal)
{
    if(sagittal)
        this->_prvt_setOrientation(medImageView::VIEW_ORIENTATION_SAGITTAL);
}

void medVtkViewNavigator::set3d(bool o3d)
{
    if(o3d)
        this->_prvt_setOrientation(medImageView::VIEW_ORIENTATION_3D);
}

void medVtkViewNavigator::_prvt_setOrientation(medImageView::Orientation orientation)
{
    double pos[3];
    int timeIndex = 0;
    if(d->currentView)
    {
        d->currentView->GetCurrentPoint(pos);
        timeIndex = d->currentView->GetTimeIndex();
        d->currentView->UnInstallInteractor();
        d->currentView->SetRenderWindow(NULL);
        d->renWin->RemoveRenderer(d->currentView->GetRenderer());
    }

    switch(orientation)
    {
    case medImageView::VIEW_ORIENTATION_3D:
        d->currentView = d->view3d;
        break;
    case medImageView::VIEW_ORIENTATION_AXIAL:
        d->view2d->SetViewOrientation (vtkImageView2D::VIEW_ORIENTATION_AXIAL);
        d->currentView = d->view2d;
        break;
    case medImageView::VIEW_ORIENTATION_CORONAL:
        d->view2d->SetViewOrientation (vtkImageView2D::VIEW_ORIENTATION_CORONAL);
        d->currentView = d->view2d;
        break;
    case medImageView::VIEW_ORIENTATION_SAGITTAL:
        d->view2d->SetViewOrientation (vtkImageView2D::VIEW_ORIENTATION_SAGITTAL);
        d->currentView = d->view2d;
        break;
    }

    d->currentView->SetRenderWindow(d->renWin);
    d->currentView->SetCurrentPoint(pos);
    d->currentView->SetTimeIndex(timeIndex);

    d->orientation = orientation;

    emit orientationChanged();
}
