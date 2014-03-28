#include "medVtkViewItkDataImageNavigator.h"

#include <vtkImageView2D.h>
#include <vtkImageView3D.h>

#include <medVtkViewBackend.h>

#include <medImageViewFactory.h>
#include <medStringListParameter.h>
#include <medBoolParameter.h>

class medVtkViewItkDataImageNavigatorPrivate
{
public:
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;

    vtkRenderWindow *render;

    medAbstractImageView *medVtkView;

    QList <medAbstractParameter*> parameters;

    QWidget *toolbox;

    medStringListParameter *mode3DParameter;
    medStringListParameter *renderer3DParameter;
    medBoolParameter *croppingParameter;

};

medVtkViewItkDataImageNavigator::medVtkViewItkDataImageNavigator(medAbstractView* parent):
    medAbstractNavigator(parent), d(new medVtkViewItkDataImageNavigatorPrivate)
{
    d->medVtkView = dynamic_cast<medAbstractImageView*>(parent);
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

    d->parameters.append(d->mode3DParameter);
    d->parameters.append(d->renderer3DParameter);
    d->parameters.append(d->croppingParameter);

    connect(d->medVtkView, SIGNAL(orientationChanged()), this, SLOT(updateVisibility()));

    d->toolbox = NULL;
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
    medImageViewFactory * factory = medImageViewFactory::instance();
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

QWidget* medVtkViewItkDataImageNavigator::widgetForToolBox() const
{
    if(!d->toolbox)
    {
        d->toolbox = new QWidget;
        QFormLayout *layout = new QFormLayout(d->toolbox);
        foreach(medAbstractParameter *parameter, d->parameters)
            layout->addRow(parameter->getLabel(), parameter->getWidget());
        d->toolbox->hide();
    }
    return d->toolbox;
}

QWidget* medVtkViewItkDataImageNavigator::widgetForToolBar() const
{
    return new QWidget();
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
    if( d->medVtkView->orientation() == medImageView::VIEW_ORIENTATION_3D &&
            ( d->medVtkView->contains("itkDataImageChar3"  ) ||
              d->medVtkView->contains("itkDataImageUChar3" ) ||
              d->medVtkView->contains("itkDataImageShort3" ) ||
              d->medVtkView->contains("itkDataImageUShort3") ||
              d->medVtkView->contains("itkDataImageInt3"   ) ||
              d->medVtkView->contains("itkDataImageLong3"  ) ||
              d->medVtkView->contains("itkDataImageULong3" ) ||
              d->medVtkView->contains("itkDataImageFloat3" ) ||
              d->medVtkView->contains("itkDataImageDouble3") ||
              d->medVtkView->contains("itkDataImageRGB3"   ) ||
              d->medVtkView->contains("itkDataImageRGBA3"  ) ) )
    {
        if(d->toolbox)
            d->toolbox->show();
    }
    else
        if(d->toolbox)
            d->toolbox->hide();
}
