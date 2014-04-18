#include "medVtkViewItkDataImageNavigator.h"

#include <vtkImageView2D.h>
#include <vtkImageView3D.h>

#include <medVtkViewBackend.h>

#include <medViewFactory.h>
#include <medAbstractImageView.h>
#include <medStringListParameter.h>
#include <medBoolParameter.h>

class medVtkViewItkDataImageNavigatorPrivate
{
public:
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;
    vtkRenderWindow *render;

    QList <medAbstractParameter*> parameters;

    medStringListParameter *mode3DParameter;
    medStringListParameter *renderer3DParameter;
    medBoolParameter *croppingParameter;

};

medVtkViewItkDataImageNavigator::medVtkViewItkDataImageNavigator(medAbstractView* parent):
    medAbstractNavigator(parent), d(new medVtkViewItkDataImageNavigatorPrivate)
{
    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;

    d->render = backend->renWin;

    d->mode3DParameter = new medStringListParameter("3D Mode", this);
    QStringList modes = QStringList() << "VR" << "MIP - Maximum" << "MIP - Minimum" << "MPR" << "Off";
    d->mode3DParameter->addItems(modes);
    d->mode3DParameter->setValue("MPR");
    connect(d->mode3DParameter, SIGNAL(valueChanged(QString)), this, SLOT(set3DMode(QString)));

    d->renderer3DParameter = new medStringListParameter("Renderer", this);
    QStringList renderers = QStringList() << "GPU" << "Ray Cast / Texture" << "Ray Cast" << "Texture" << "Default";
    d->renderer3DParameter->addItems(renderers);
    connect(d->renderer3DParameter, SIGNAL(valueChanged(QString)), this, SLOT(setRenderer(QString)));

    d->croppingParameter = new medBoolParameter("Cropping", this);
    connect(d->croppingParameter, SIGNAL(valueChanged(bool)), this, SLOT(enableCropping(bool)));

    this->set3DMode("MPR");

    d->parameters.append(d->mode3DParameter);
    d->parameters.append(d->renderer3DParameter);
    d->parameters.append(d->croppingParameter);

    connect(dynamic_cast<medAbstractImageView*>(this->view()), SIGNAL(orientationChanged()),
            this, SLOT(updateVisibility()));

}

medVtkViewItkDataImageNavigator::~medVtkViewItkDataImageNavigator()
{
    delete d;
}

QString  medVtkViewItkDataImageNavigator::identifier() const
{
    return medVtkViewItkDataImageNavigator::s_identifier();
}

QString  medVtkViewItkDataImageNavigator::s_identifier()
{
    return "medVtkViewItkDataImageNavigator";
}

bool medVtkViewItkDataImageNavigator::registered()
{
    medViewFactory * factory = medViewFactory::instance();
    return factory->registerAdditionalNavigator<medVtkViewItkDataImageNavigator>(medVtkViewItkDataImageNavigator::s_identifier(),
                                                           QStringList() << "medVtkView");

}

QStringList medVtkViewItkDataImageNavigator::handled(void) const
{
    return QStringList() << "medVtkView";
}

QString medVtkViewItkDataImageNavigator::description() const
{
    return "Navigator to interact with itk images in a medVtkView";
}

QWidget *  medVtkViewItkDataImageNavigator::buildToolBoxWidget()
{
    QWidget *toolBoxWidget = new QWidget;
    QFormLayout *layout = new QFormLayout(toolBoxWidget);
    foreach(medAbstractParameter *parameter, d->parameters)
        layout->addRow(parameter->getLabel(), parameter->getWidget());
    toolBoxWidget->hide();
    return toolBoxWidget;
}

QWidget *medVtkViewItkDataImageNavigator::buildToolBarWidget()
{
    QWidget *toolBarWidget = new QWidget;
    toolBarWidget->hide();
    return toolBarWidget;
}

QList<medAbstractParameter*> medVtkViewItkDataImageNavigator::parameters()
{
    return d->parameters;
}

void medVtkViewItkDataImageNavigator::set3DMode(QString mode)
{
    if ( mode == "VR" )
    {
        d->view3d->SetRenderingModeToVR();
        d->view3d->SetVolumeRayCastFunctionToComposite();
        d->renderer3DParameter->show();
        d->croppingParameter->show();
        this->enableCropping(d->croppingParameter->value());
    }
    else if ( mode == "MPR" )
    {
        d->view3d->SetRenderingModeToPlanar();
        d->view3d->ShowActorXOn();
        d->view3d->ShowActorYOn();
        d->view3d->ShowActorZOn();
        d->renderer3DParameter->hide();
        d->croppingParameter->hide();
        this->enableCropping(false);
    }
    else if ( mode == "MIP - Maximum" )
    {
        d->view3d->SetRenderingModeToVR();
        d->view3d->SetVolumeRayCastFunctionToMaximumIntensityProjection();
        d->renderer3DParameter->show();
        d->croppingParameter->show();
        this->enableCropping(d->croppingParameter->value());
    }
    else if ( mode == "MIP - Minimum" )
    {
        d->view3d->SetRenderingModeToVR();
        d->view3d->SetVolumeRayCastFunctionToMinimumIntensityProjection();
        d->renderer3DParameter->show();
        d->croppingParameter->show();
        this->enableCropping(d->croppingParameter->value());
    }
    else if ( mode == "Off" )
    {
        d->view3d->SetRenderingModeToPlanar();
        d->view3d->ShowActorXOff();
        d->view3d->ShowActorYOff();
        d->view3d->ShowActorZOff();
        d->renderer3DParameter->hide();
        d->croppingParameter->hide();
        this->enableCropping(false);
    }

    d->render->Render();
}

void medVtkViewItkDataImageNavigator::setRenderer(QString renderer)
{
    if ( renderer=="GPU" )
        d->view3d->SetVolumeMapperToGPU();

    else if ( renderer=="Ray Cast / Texture" )
        d->view3d->SetVolumeMapperToRayCastAndTexture();

    else if ( renderer=="Ray Cast" )
        d->view3d->SetVolumeMapperToRayCast();

    else if ( renderer=="Default" )
        d->view3d->SetVolumeMapperToDefault();

    d->render->Render();
}

void medVtkViewItkDataImageNavigator::enableCropping(bool enabled)
{
    if ( enabled )
    {
        if ( d->view3d->GetBoxWidget()->GetInteractor() ) // avoid VTK warnings
        {
            d->view3d->SetCroppingModeToOutside();
            d->view3d->SetShowBoxWidget ( 1 );
        }
    }
    else
    {
        if ( d->view3d->GetBoxWidget()->GetInteractor() )
        {
            // d->view3D->SetCroppingModeToOff ();
            d->view3d->SetShowBoxWidget ( 0 );
        }
    }

    d->render->Render();
    //TODO: Shouldn't we be able to save the cropping after?
}

void medVtkViewItkDataImageNavigator::updateVisibility()
{
    medAbstractImageView *imageView = dynamic_cast<medAbstractImageView*>(this->view());
    if(!imageView)
        return;

    if( imageView->orientation() == medImageView::VIEW_ORIENTATION_3D &&
            ( imageView->contains("itkDataImageChar3"  ) ||
              imageView->contains("itkDataImageUChar3" ) ||
              imageView->contains("itkDataImageShort3" ) ||
              imageView->contains("itkDataImageUShort3") ||
              imageView->contains("itkDataImageInt3"   ) ||
              imageView->contains("itkDataImageLong3"  ) ||
              imageView->contains("itkDataImageULong3" ) ||
              imageView->contains("itkDataImageFloat3" ) ||
              imageView->contains("itkDataImageDouble3") ||
              imageView->contains("itkDataImageRGB3"   ) ||
              imageView->contains("itkDataImageRGBA3"  )))
    {
        if(this->toolBoxWidget())
            this->toolBoxWidget()->show();
    }
    else
        if(this->toolBoxWidget())
            this->toolBoxWidget()->hide();
}
