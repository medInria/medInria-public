/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtkViewNavigator.h>

#include <vtkCamera.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkImageActor.h>
#include <vtkMetaDataSet.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>

#include <medVtkViewBackend.h>
#include <medViewFactory.h>
#include <medViewContainer.h>
#include <medViewContainerSplitter.h>
#include <medAbstractImageView.h>
#include <medBoolGroupParameterL.h>
#include <medBoolParameterL.h>
#include <medDataIndex.h>
#include <medDataManager.h>
#include <medDoubleParameterL.h>
#include <medVector2DParameterL.h>
#include <medVector3DParameterL.h>
#include <medCompositeParameterL.h>
#include <medStringListParameterL.h>
#include <medTimeLineParameterL.h>

#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

/*=========================================================================

    D pointer

=========================================================================*/

class medVtkViewNavigatorPrivate
{
    public:
    medAbstractImageView *parent;

    vtkImageView2D *view2d;
    vtkImageView3D *view3d;
    vtkImageView *currentView;

    vtkRenderer *renderer2d;
    vtkRenderer *renderer3d;

    medImageView::Orientation orientation;

    medBoolGroupParameterL *orientationParameter;
    medBoolParameterL *oAxialParameter;
    medBoolParameterL *oSagittalParameter;
    medBoolParameterL *oCoronalParameter;
    medBoolParameterL *o3dParameter;

    medBoolParameterL *enableZooming;
    medBoolParameterL *enablePanning;
    medBoolParameterL *enableSlicing;
    medBoolParameterL *enableMeasuring;

    medBoolParameterL *showAxesParameter;
    medBoolParameterL *showRulerParameter;
    medBoolParameterL *showAnnotationParameter;
    medBoolParameterL *showScalarBarParameter;
    medBoolParameterL *showAnnotatedCubeParameter;
    
    QPushButton *fourImageSplitterButton;

    QWidget *showOptionsWidget;

    QList<medAbstractParameterL*> parameters;

};

/*=========================================================================

    public

=========================================================================*/

medVtkViewNavigator::medVtkViewNavigator(medAbstractView *parent) :
    medAbstractImageViewNavigator(parent), d(new medVtkViewNavigatorPrivate)
{
    d->parent = dynamic_cast<medAbstractImageView*>(parent);

    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(d->parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;

    d->currentView = nullptr;
    d->showOptionsWidget = nullptr;

    d->renderer2d = d->view2d->GetRenderer();
    d->renderer3d = d->view3d->GetRenderer();

    d->orientationParameter = new medBoolGroupParameterL("Orientation", this);
    d->orientationParameter->setPushButtonDirection(QBoxLayout::LeftToRight);
    d->orientationParameter->getLabel()->hide();

    d->oAxialParameter = new medBoolParameterL("axial", this);
    d->oAxialParameter->setIcon(QIcon(":/pixmaps/orientationAxial.png"));
    d->oAxialParameter->setIconSize(QSize(50,50));
    d->oAxialParameter->setToolTip("Axial");
    d->oAxialParameter->getPushButton()->setMinimumSize(64,64);
    connect(d->oAxialParameter, SIGNAL(valueChanged(bool)),
            this, SLOT(setAxial(bool)));

    d->oCoronalParameter = new medBoolParameterL("coronal", this);
    d->oCoronalParameter->setIcon(QIcon(":/pixmaps/orientationCoronal.png"));
    d->oCoronalParameter->setIconSize(QSize(50,50));
    d->oCoronalParameter->setToolTip("Coronal");
    d->oCoronalParameter->getPushButton()->setMinimumSize(64,64);
    connect(d->oCoronalParameter, SIGNAL(valueChanged(bool)),
            this, SLOT(setCoronal(bool)));

    d->oSagittalParameter = new medBoolParameterL("sagittal", this);
    d->oSagittalParameter->setIcon(QIcon(":/pixmaps/orientationSagittal.png"));
    d->oSagittalParameter->setIconSize(QSize(50,50));
    d->oSagittalParameter->setToolTip("Sagittal");
    d->oSagittalParameter->getPushButton()->setMinimumSize(64,64);
    connect(d->oSagittalParameter, SIGNAL(valueChanged(bool)),
            this, SLOT(setSagittal(bool)));

    d->o3dParameter = new medBoolParameterL("3d", this);
    d->o3dParameter->setIcon(QIcon::fromTheme("orientation_3d"));
    d->o3dParameter->setIconSize(QSize(50,50));
    d->o3dParameter->setToolTip("3D");
    d->o3dParameter->getPushButton()->setMinimumSize(64,64);
    connect(d->o3dParameter, SIGNAL(valueChanged(bool)),
            this, SLOT(set3d(bool)));

    d->orientationParameter->addParameter(d->oAxialParameter);
    d->orientationParameter->addParameter(d->oCoronalParameter);
    d->orientationParameter->addParameter(d->oSagittalParameter);
    d->orientationParameter->addParameter(d->o3dParameter);
    d->oAxialParameter->setValue(true);

    d->showAxesParameter = new medBoolParameterL("Axes", this);
    d->showRulerParameter = new medBoolParameterL("Ruler", this);
    d->showAnnotationParameter = new medBoolParameterL("Annotations", this);
    d->showScalarBarParameter = new medBoolParameterL("Scalar Bar", this);
    d->showAnnotatedCubeParameter = new medBoolParameterL("Annotated Cube", this);

    d->showAxesParameter->setText("Axes");
    d->showRulerParameter->setText("Ruler");
    d->showAnnotationParameter->setText("Annotations");
    d->showScalarBarParameter->setText("Scalar Bar");
    d->showAnnotatedCubeParameter->setText("Annotated Cube");

    connect(d->showAxesParameter, SIGNAL(valueChanged(bool)), this, SLOT(showAxes(bool)));
    connect(d->showRulerParameter, SIGNAL(valueChanged(bool)), this, SLOT(showRuler(bool)));
    connect(d->showAnnotationParameter, SIGNAL(valueChanged(bool)), this, SLOT(showAnnotations(bool)));
    connect(d->showScalarBarParameter, SIGNAL(valueChanged(bool)), this, SLOT(showScalarBar(bool)));
    connect(d->showAnnotatedCubeParameter, SIGNAL(valueChanged(bool)), this, SLOT(showAnnotatedCube(bool)));

    d->showAxesParameter->setValue(false);
    d->showRulerParameter->setValue(true);
    d->showAnnotationParameter->setValue(true);
    d->showScalarBarParameter->setValue(false);
    d->showAnnotatedCubeParameter->setValue(true);

    d->enableZooming = new medBoolParameterL("Zooming", this);
    d->enableZooming->setIcon(QIcon::fromTheme("magnifier"));
    d->enableZooming->setToolTip(tr("Zooming"));
    connect(d->enableZooming, SIGNAL(valueChanged(bool)), this, SLOT(enableZooming(bool)));
    
    d->enablePanning = new medBoolParameterL("Pan",this);
    d->enablePanning->setIcon(QIcon::fromTheme("pan"));
    d->enablePanning->setToolTip(tr("Pan"));
    connect(d->enablePanning, SIGNAL(valueChanged(bool)), this, SLOT(enablePanning(bool)));

    d->enableSlicing = new medBoolParameterL("Slicing", this);
    d->enableSlicing->setIcon(QIcon::fromTheme("stack"));
    d->enableSlicing->setToolTip(tr("Slicing"));
    connect(d->enableSlicing, SIGNAL(valueChanged(bool)), this, SLOT(enableSlicing(bool)));

    d->enableMeasuring = new medBoolParameterL("Measuring", this);
    d->enableMeasuring->setIcon (QIcon::fromTheme("ruler"));
    d->enableMeasuring->setToolTip(tr("Measuring"));
    connect(d->enableMeasuring, SIGNAL(valueChanged(bool)), this, SLOT(enableMeasuring(bool)));

    d->parameters << d->orientationParameter
                    << this->zoomParameter()
                    << this->panParameter()
                    << this->cameraParameter()
                    << d->showAxesParameter
                    << d->showRulerParameter
                    << d->showAnnotationParameter
                    << d->showScalarBarParameter
                    << d->showAnnotatedCubeParameter
                    << this->positionBeingViewedParameter()
                    << this->timeLineParameter();

    connect(this, SIGNAL(orientationChanged()),
            dynamic_cast<medAbstractImageView*>(parent), SIGNAL(orientationChanged()));

    // these parameters are always shown
    d->showScalarBarParameter->show();
    d->showAnnotationParameter->show();
}

medVtkViewNavigator::~medVtkViewNavigator()
{
    delete d;
}

QString  medVtkViewNavigator::identifier() const
{
    return medVtkViewNavigator::s_identifier();
}

bool medVtkViewNavigator::registered()
{
    medViewFactory * factory = medViewFactory::instance();
    return factory->registerNavigator<medVtkViewNavigator>(medVtkViewNavigator::s_identifier(),
                                                           QStringList() << "medVtkView");
}

QString  medVtkViewNavigator::s_identifier()
{
    return "medVtkViewNavigator";
}

QStringList medVtkViewNavigator::handled(void) const
{
    return QStringList() << "medVtkView";
}


QString medVtkViewNavigator::description() const
{
    return "Navigator to interact with a medVtkView";
}

QList<medAbstractParameterL*> medVtkViewNavigator::linkableParameters()
{
    return d->parameters;
}

QList<medBoolParameterL*> medVtkViewNavigator::mouseInteractionParameters()
{
    QList<medBoolParameterL*> params;
    params.append(d->enableZooming);
    params.append(d->enableSlicing);
    params.append(d->enablePanning);
    params.append(d->enableMeasuring);
    return params;
}

medImageView::Orientation medVtkViewNavigator::orientation() const
{
    return d->orientation;
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
    default:
        break;
    }

    d->orientation = orientation;
}

void medVtkViewNavigator::setCamera(const QVector3D &position,
                       const QVector3D &viewup,
                       const QVector3D &focal,
                       double parallelScale)
{
    bool needUpdate = false;

    double pos[3];
    this->cameraPosition(pos);
    QVector3D currentPos(pos[0], pos[1], pos[2]);
    if(position != currentPos)
    {
        setCameraPosition(position);
        needUpdate = true;
    }

    double up[3];
    this->cameraUp(up);
    QVector3D currentUp(up[0], up[1], up[2]);
    if(viewup != currentUp)
    {
        setCameraUp(viewup);
        needUpdate = true;
    }

    double fp[3];
    this->cameraFocalPoint(fp);
    QVector3D currentFp(fp[0], fp[1], fp[2]);
    if(focal != currentFp)
    {
        setCameraFocalPoint(focal);
        needUpdate = true;
    }

    double db;
    this->cameraParallelScale(db);
    if(db != parallelScale)
    {
        setCameraParallelScale(parallelScale);
        needUpdate = true;
    }

    if(needUpdate)
        d->parent->render();
}

void medVtkViewNavigator::setZoom(double zoom)
{
    if(zoom == d->currentView->GetZoom())
        return;

    d->currentView->SetZoom(zoom);
    d->parent->render();

}

void medVtkViewNavigator::setPan(const QVector2D &pan)
{
    if(pan == QVector2D(d->view2d->GetPan()[0], d->view2d->GetPan()[1]))
        return;

    double stdpan[2] = {pan.x(), pan.y()};
    d->view2d->SetPan(stdpan);
    d->view2d->Render();
}

void medVtkViewNavigator::moveToPosition(const QVector3D &position)
{
    double op[3];
    d->view2d->GetCurrentPoint(op);
    QVector3D oldPos(op[0], op[1], op[2]);
    if(position == oldPos)
        return;

    double pos[3];
    pos[0] = position.x();
    pos[1] = position.y();
    pos[2] = position.z();

    d->view2d->SetCurrentPoint(pos);
    d->view2d->UpdateCursorPosition(pos);

    d->view3d->SetCurrentPoint(pos);

    if(d->orientation == medImageView::VIEW_ORIENTATION_3D)
        d->view3d->Render();
    else d->view2d->Render();
}

/*=========================================================================

    protected

=========================================================================*/

QWidget* medVtkViewNavigator::buildToolBoxWidget()
{
    QWidget *toolBoxWidget = new QWidget;

    d->showOptionsWidget = new QWidget;
    QHBoxLayout* showOptionsLayout = new QHBoxLayout(d->showOptionsWidget);
    showOptionsLayout->addWidget(d->showAxesParameter->getCheckBox());
    showOptionsLayout->addWidget(d->showRulerParameter->getCheckBox());
    showOptionsLayout->addWidget(d->showAnnotationParameter->getCheckBox());
    showOptionsLayout->addWidget(d->showScalarBarParameter->getCheckBox());
    showOptionsLayout->addWidget(d->showAnnotatedCubeParameter->getCheckBox());
    showOptionsLayout->setContentsMargins(0, 0, 0, 10);

    QVBoxLayout* layout = new QVBoxLayout(toolBoxWidget);
    layout->addWidget(d->orientationParameter->getLabel());
    layout->addWidget(d->orientationParameter->getPushButtonGroup());
    layout->addWidget(d->showOptionsWidget);
    layout->addWidget(this->timeLineParameter()->getWidget());
    layout->setContentsMargins(0, 0, 0, 0);

    return toolBoxWidget;
}

QWidget* medVtkViewNavigator::buildToolBarWidget()
{
    QWidget *toolBarWidget = new QWidget;
    
    return toolBarWidget;
}

void medVtkViewNavigator::cameraUp (double *coordinates) const
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

void medVtkViewNavigator::cameraPosition(double *coordinates) const
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


void medVtkViewNavigator::cameraFocalPoint(double *coordinates) const
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

void medVtkViewNavigator::cameraParallelScale(double &parallelScale) const
{
    if ( d->orientation != medImageView::VIEW_ORIENTATION_3D )
        parallelScale =  -1;
    else
        parallelScale = d->renderer3d->GetActiveCamera()->GetParallelScale();
}


void medVtkViewNavigator::setCamera(QHash<QString,QVariant> cameraOptions)
{
    if(cameraOptions.count() != 4)
    {
        qWarning() << "Camera options are incorrect.";
        return;
    }

    QVector3D cameraPosition(cameraOptions["Camera Position"].value<QVector3D>());
    QVector3D cameraUp(cameraOptions["Camera Up"].value<QVector3D>());
    QVector3D cameraFocalPoint(cameraOptions["Camera Focal"].value<QVector3D>());
    double parallelScale = cameraOptions["Parallel Scale"].toReal();

    this->setCamera(cameraPosition, cameraUp, cameraFocalPoint, parallelScale);
}


void medVtkViewNavigator::setCameraUp(const QVector3D& viewup)
{
    double vup[3];

    vup[0] = viewup.x();
    vup[1] = viewup.y();
    vup[2] = viewup.z();

    d->view3d->GetInteractorStyle()->HandleObserversOff();
    d->renderer3d->GetActiveCamera()->SetViewUp(vup);
    d->view3d->GetInteractorStyle()->HandleObserversOn();
}

void medVtkViewNavigator::setCameraPosition(const QVector3D& position)
{
    d->view3d->GetInteractorStyle()->HandleObserversOff();
    vtkCamera *camera = d->renderer3d->GetActiveCamera();
    camera->SetPosition (position.x(), position.y(), position.z());
    d->renderer3d->ResetCameraClippingRange();
    d->view3d->GetInteractorStyle()->HandleObserversOn();
}


void medVtkViewNavigator::setCameraFocalPoint(const QVector3D& focal)
{
    double foc[3];

    foc[0] = focal.x();
    foc[1] = focal.y();
    foc[2] = focal.z();

    d->view3d->GetInteractorStyle()->HandleObserversOff();
    d->renderer3d->GetActiveCamera()->SetFocalPoint(foc);
    d->view3d->GetInteractorStyle()->HandleObserversOn();
}


void medVtkViewNavigator::setCameraParallelScale(double parallelScale)
{
    d->view3d->GetInteractorStyle()->HandleObserversOff();
    d->renderer3d->GetActiveCamera()->SetParallelScale(parallelScale);
    d->view3d->GetInteractorStyle()->HandleObserversOn();
}

bool medVtkViewNavigator::setSlice(int slice)
{
    int res = false;

    if ((slice > 0) && 
        (slice < d->view2d->GetSliceMax()))
    {
        d->view2d->SetSlice(slice);
        d->currentView->Render();
        res = true;
    }
    
    return res;
}

bool medVtkViewNavigator::setRotationAngle(double angle)
{
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();

    double *cameraPos = d->view3d->GetCameraFocalPoint();
    double *cameraUp  = d->view3d->GetCameraViewUp(); // camera axis

    // Translation needed for meshes not centered at 0, 0, 0
    transform->Translate(cameraPos[0], cameraPos[1], cameraPos[2]);
    // Rotation in degree
    transform->RotateWXYZ(angle, cameraUp[0], cameraUp[1], cameraUp[2]);
    // Put back at original position
    transform->Translate(-cameraPos[0], -cameraPos[1], -cameraPos[2]);

    foreach(medDataIndex index, d->parent->dataList())
    {
        medAbstractData *data = medDataManager::instance().retrieveData(index);

        // We only apply rotation on meshes
        if (data && data->identifier().contains("vtkDataMesh") )
        {
            vtkMetaDataSet *dataset = reinterpret_cast<vtkMetaDataSet*>(data->data());

            for(unsigned int i = 0; i < dataset->GetNumberOfActors(); i++)
            {
                dataset->GetActor(i)->SetUserTransform(transform);
            }
        }
        else
        {
            return false;
        }
    }

    d->parent->render();
    return true;
}

void medVtkViewNavigator::setAxial(bool axial)
{
    if(axial)
        this->changeOrientation(medImageView::VIEW_ORIENTATION_AXIAL);
}

void medVtkViewNavigator::setCoronal(bool coronal)
{
    if(coronal)
        this->changeOrientation(medImageView::VIEW_ORIENTATION_CORONAL);
}

void medVtkViewNavigator::setSagittal(bool sagittal)
{
    if(sagittal)
        this->changeOrientation(medImageView::VIEW_ORIENTATION_SAGITTAL);
}

void medVtkViewNavigator::set3d(bool o3d)
{
    if(o3d)
        this->changeOrientation(medImageView::VIEW_ORIENTATION_3D);
}

void medVtkViewNavigator::showAxes(bool show)
{
    d->view2d->SetShowImageAxis(show);
    d->view2d->InvokeEvent(vtkImageView2D::CurrentPointChangedEvent);
    d->view2d->Render();
}

void medVtkViewNavigator::showRuler(bool show)
{
    d->view2d->SetShowRulerWidget(show);
    d->view2d->Render();
}

void medVtkViewNavigator::showAnnotations(bool show)
{
    d->view2d->SetShowAnnotations(show);
    d->view3d->SetShowAnnotations(show);
    d->currentView->Render();
}

void medVtkViewNavigator::showScalarBar(bool show)
{
    d->view2d->SetShowScalarBar(show);
    d->view3d->SetShowScalarBar(show);
    d->currentView->Render();
}

void medVtkViewNavigator::showAnnotatedCube(bool show)
{
    d->view3d->SetShowCube(static_cast<int>(show));
    d->currentView->Render();
}

/*=========================================================================

    private

=========================================================================*/

void medVtkViewNavigator::changeOrientation(medImageView::Orientation orientation)
{
    // prevent from vtk event send by the resend by the observer. - RDE
    this->zoomParameter()->blockSignals(true);
    this->cameraParameter()->blockSignals(true);

    double pos[3];
    vtkRenderWindow * renWin = nullptr;
    if(d->currentView)
    {
        d->currentView->GetCurrentPoint(pos);
        d->currentView->UnInstallInteractor();
        renWin = d->currentView->GetRenderWindow();
        d->currentView->SetRenderWindow(nullptr);
    }

    switch(orientation)
    {
    case medImageView::VIEW_ORIENTATION_3D:
    {
        d->currentView = d->view3d;
        double *pos = d->view3d->GetRenderer()->GetActiveCamera()->GetPosition();
        double *vup = d->view3d->GetRenderer()->GetActiveCamera()->GetViewUp();
        double *foc = d->view3d->GetRenderer()->GetActiveCamera()->GetFocalPoint();
        double   ps = d->view3d->GetRenderer()->GetActiveCamera()->GetParallelScale();
        QVector3D position( pos[0], pos[1], pos[2] );
        QVector3D viewup( vup[0], vup[1], vup[2] );
        QVector3D focal( foc[0], foc[1], foc[2] );
        QHash <QString,QVariant> cam;
        cam["Camera Position"] = QVariant(position);
        cam["Camera Up"] = QVariant(viewup);
        cam["Camera Focal"] = QVariant(focal);
        cam["Parallel Scale"] = QVariant(ps);

        if(d->parent->cameraParameter())
            d->parent->cameraParameter()->setValues(cam);
    }
        break;
    case medImageView::VIEW_ORIENTATION_AXIAL:
        d->view2d->SetViewOrientation(vtkImageView2D::VIEW_ORIENTATION_AXIAL);
        d->currentView = d->view2d;
        break;
    case medImageView::VIEW_ORIENTATION_CORONAL:
        d->view2d->SetViewOrientation(vtkImageView2D::VIEW_ORIENTATION_CORONAL);
        d->currentView = d->view2d;
        break;
    case medImageView::VIEW_ORIENTATION_SAGITTAL:
        d->view2d->SetViewOrientation(vtkImageView2D::VIEW_ORIENTATION_SAGITTAL);
        d->currentView = d->view2d;
        break;
    default:
        break;
    }

    // If we have transitioned to 3d view, and do not have any image data,
    //with only meshes for instance, grab it from 2d and make it invisible.
    if (d->currentView == d->view3d && (!d->currentView->GetMedVtkImageInfo() || !d->currentView->GetMedVtkImageInfo()->initialized))
    {
        if (d->view2d->GetMedVtkImageInfo() && d->view2d->GetMedVtkImageInfo()->initialized)
        {
            d->view3d->GetActorX()->SetOpacity(0.0);
            d->view3d->GetActorY()->SetOpacity(0.0);
            d->view3d->GetActorZ()->SetOpacity(0.0);
            d->currentView->SetInput(d->view2d->Get2DDisplayMapperInputAlgorithm(0)->GetInputConnection(0,0), d->view2d->GetOrientationMatrix());
            d->currentView->ResetCamera();
        }
    }

    d->currentView->SetRenderWindow(renWin);
    d->currentView->SetCurrentPoint(pos);
    d->currentView->GlobalWarningDisplayOff();
    d->currentView->Render();

    d->orientation = orientation;

    emit orientationChanged();

    this->zoomParameter()->blockSignals(false);
    this->cameraParameter()->blockSignals(false);
}

void medVtkViewNavigator::updateWidgets()
{
    if(d->orientation == medImageView::VIEW_ORIENTATION_3D)
    {
        d->showAxesParameter->hide();
        d->showRulerParameter->hide();
        d->showAnnotatedCubeParameter->show();
    }
    else
    {
        d->showAxesParameter->show();
        d->showRulerParameter->show();
        d->showAnnotatedCubeParameter->hide();
    }
}

void medVtkViewNavigator::enableZooming(bool enable)
{
    if(enable)
        d->view2d->SetLeftButtonInteractionStyle ( vtkInteractorStyleImageView2D::InteractionTypeZoom );

}

void medVtkViewNavigator::enableSlicing(bool enable)
{
    if(enable)
        d->view2d->SetLeftButtonInteractionStyle ( vtkInteractorStyleImageView2D::InteractionTypeSlice );
}

void medVtkViewNavigator::enablePanning(bool enable)
{
    if(enable)
        d->view2d->SetLeftButtonInteractionStyle ( vtkInteractorStyleImageView2D::InteractionTypePan );
}

void medVtkViewNavigator::enableMeasuring(bool enable)
{
    if(enable)
    {
        d->view2d->ShowDistanceWidgetOn();
    }
    else
    {
        d->view2d->ShowDistanceWidgetOff();
    }
}
