/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medVtkViewItkDataImageNavigator.h"

#include <vtkImageView2D.h>
#include <vtkImageView3D.h>

#include <medVtkViewBackend.h>

#include <medViewFactory.h>
#include <medAbstractImageView.h>
#include <medStringListParameterL.h>
#include <medBoolParameterL.h>

#include <QtWidgets>


/*=========================================================================

    D pointer

=========================================================================*/


class medVtkViewItkDataImageNavigatorPrivate
{
public:
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;

    medAbstractImageView *imageView;

    QList <medAbstractParameterL*> parameters;

    medStringListParameterL *mode3DParameter;
    medStringListParameterL *renderer3DParameter;
    medBoolParameterL *croppingParameter;

    // Handle hide / show planes in MSR
    medBoolParameterL *actorXMSRParameter;
    medBoolParameterL *actorYMSRParameter;
    medBoolParameterL *actorZMSRParameter;
};


/*=========================================================================

    public

=========================================================================*/

medVtkViewItkDataImageNavigator::medVtkViewItkDataImageNavigator(medAbstractView* parent):
    medAbstractNavigator(parent), d(new medVtkViewItkDataImageNavigatorPrivate)
{
    d->imageView =dynamic_cast<medAbstractImageView*>(parent);

    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;

    d->mode3DParameter = new medStringListParameterL("3D Mode", this);
    d->mode3DParameter->addItem("VR");
    d->mode3DParameter->addItem("MIP - Maximum");
    d->mode3DParameter->addItem("MIP - Minimum");
    d->mode3DParameter->addItem("MSR");
    d->mode3DParameter->addItem("Off");
    d->mode3DParameter->setValue("MSR");
    connect(d->mode3DParameter, SIGNAL(valueChanged(QString)), this, SLOT(setMode3D(QString)));

    d->renderer3DParameter = new medStringListParameterL("Renderer", this);
    d->renderer3DParameter->addItem("GPU");
#ifdef MED_USE_OSPRAY_4_VR_BY_CPU
    d->renderer3DParameter->addItem("OSPRay / CPU");
#endif //MED_USE_OSPRAY_4_VR_BY_CPU
    d->renderer3DParameter->addItem("Ray Cast");
    d->renderer3DParameter->addItem("Default");
    connect(d->renderer3DParameter, SIGNAL(valueChanged(QString)), this, SLOT(setRenderer(QString)));

    d->croppingParameter = new medBoolParameterL("Cropping", this);
    connect(d->croppingParameter, SIGNAL(valueChanged(bool)), this, SLOT(enableCropping(bool)));

    d->actorXMSRParameter = new medBoolParameterL("Show X plane", this);
    d->actorXMSRParameter->setValue(true);
    connect(d->actorXMSRParameter, SIGNAL(valueChanged(bool)), this, SLOT(toggleMSRXPlane(bool)));

    d->actorYMSRParameter = new medBoolParameterL("Show Y plane", this);
    d->actorYMSRParameter->setValue(true);
    connect(d->actorYMSRParameter, SIGNAL(valueChanged(bool)), this, SLOT(toggleMSRYPlane(bool)));

    d->actorZMSRParameter = new medBoolParameterL("Show Z plane", this);
    d->actorZMSRParameter->setValue(true);
    connect(d->actorZMSRParameter, SIGNAL(valueChanged(bool)), this, SLOT(toggleMSRZPlane(bool)));

    this->setMode3D("MSR");

    d->parameters.append(d->mode3DParameter);
    d->parameters.append(d->actorXMSRParameter);
    d->parameters.append(d->actorYMSRParameter);
    d->parameters.append(d->actorZMSRParameter);
    d->parameters.append(d->renderer3DParameter);
    d->parameters.append(d->croppingParameter);
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

QList<medAbstractParameterL*> medVtkViewItkDataImageNavigator::linkableParameters()
{
    return d->parameters;
}

QList<medBoolParameterL*> medVtkViewItkDataImageNavigator::mouseInteractionParameters()
{
    // no parameters related to mouse interactions
    return QList<medBoolParameterL*>();
}

void medVtkViewItkDataImageNavigator::updateWidgets()
{
    if(d->imageView->orientation() == medImageView::VIEW_ORIENTATION_3D &&
        (d->imageView->contains("itkDataImageChar3")     ||
         d->imageView->contains("itkDataImageUChar3")    ||
         d->imageView->contains("itkDataImageShort3")    ||
         d->imageView->contains("itkDataImageUShort3")   ||
         d->imageView->contains("itkDataImageInt3")      ||
         d->imageView->contains("itkDataImageLong3")     ||
         d->imageView->contains("itkDataImageULong3")    ||
         d->imageView->contains("itkDataImageFloat3")    ||
         d->imageView->contains("itkDataImageDouble3")   ||
         d->imageView->contains("itkDataImageRGB3")      ||
         d->imageView->contains("itkDataImageRGBA3")     ||

         d->imageView->contains("itkDataImageChar4")     ||
         d->imageView->contains("itkDataImageUChar4")    ||
         d->imageView->contains("itkDataImageShort4")    ||
         d->imageView->contains("itkDataImageUShort4")   ||
         d->imageView->contains("itkDataImageInt4")      ||
         d->imageView->contains("itkDataImageLong4")     ||
         d->imageView->contains("itkDataImageULong4")    ||
         d->imageView->contains("itkDataImageFloat4")    ||
         d->imageView->contains("itkDataImageDouble4")))
    {
        if(this->toolBoxWidget())
            this->toolBoxWidget()->show();

    }
    else
        if(this->toolBoxWidget())
            this->toolBoxWidget()->hide();
}

void medVtkViewItkDataImageNavigator::setMode3D(QString mode)
{
    if ( mode == "VR" )
    {
        d->view3d->SetRenderingModeToVR();
        d->view3d->SetVolumeRayCastFunctionToComposite();

        d->actorXMSRParameter->hide();
        d->actorYMSRParameter->hide();
        d->actorZMSRParameter->hide();

        d->renderer3DParameter->show();
        d->croppingParameter->show();
        this->enableCropping(d->croppingParameter->value());
    }
    else if ( mode == "MSR" )
    {
        d->view3d->SetRenderingModeToPlanar();

        this->toggleMSRXPlane(d->actorXMSRParameter->value());
        this->toggleMSRYPlane(d->actorYMSRParameter->value());
        this->toggleMSRZPlane(d->actorZMSRParameter->value());

        d->actorXMSRParameter->show();
        d->actorYMSRParameter->show();
        d->actorZMSRParameter->show();

        d->renderer3DParameter->hide();
        d->croppingParameter->hide();
        this->enableCropping(false);
    }
    else if ( mode == "MIP - Maximum" )
    {
        d->view3d->SetRenderingModeToVR();
        d->view3d->SetVolumeRayCastFunctionToMaximumIntensityProjection();

        d->actorXMSRParameter->hide();
        d->actorYMSRParameter->hide();
        d->actorZMSRParameter->hide();

        d->renderer3DParameter->show();
        d->croppingParameter->show();
        this->enableCropping(d->croppingParameter->value());
    }
    else if ( mode == "MIP - Minimum" )
    {
        d->view3d->SetRenderingModeToVR();
        d->view3d->SetVolumeRayCastFunctionToMinimumIntensityProjection();

        d->actorXMSRParameter->hide();
        d->actorYMSRParameter->hide();
        d->actorZMSRParameter->hide();

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

        d->actorXMSRParameter->hide();
        d->actorYMSRParameter->hide();
        d->actorZMSRParameter->hide();

        d->renderer3DParameter->hide();
        d->croppingParameter->hide();
        this->enableCropping(false);
    }

    d->imageView->render();
}

void medVtkViewItkDataImageNavigator::toggleMSRXPlane(bool toggleValue)
{
    if (toggleValue)
        d->view3d->ShowActorXOn();
    else
        d->view3d->ShowActorXOff();

    d->imageView->render();
}

void medVtkViewItkDataImageNavigator::toggleMSRYPlane(bool toggleValue)
{
    if (toggleValue)
        d->view3d->ShowActorYOn();
    else
        d->view3d->ShowActorYOff();

    d->imageView->render();
}

void medVtkViewItkDataImageNavigator::toggleMSRZPlane(bool toggleValue)
{
    if (toggleValue)
        d->view3d->ShowActorZOn();
    else
        d->view3d->ShowActorZOff();

    d->imageView->render();
}

void medVtkViewItkDataImageNavigator::setRenderer(QString renderer)
{
    if ( renderer=="GPU" )
        d->view3d->SetVolumeMapperToGPU();
#ifdef MED_USE_OSPRAY_4_VR_BY_CPU
    else if ( renderer=="OSPRay / CPU" )
        d->view3d->SetVolumeMapperToOSPRayRenderMode();
#endif
    else if ( renderer=="Ray Cast" )
        d->view3d->SetVolumeMapperToRayCast();

    else if ( renderer=="Default" )
        d->view3d->SetVolumeMapperToDefault();

    d->imageView->render();
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
            d->view3d->SetCroppingModeToOff();
            d->view3d->SetShowBoxWidget ( 0 );
        }
    }

    d->imageView->render();
    //TODO: Shouldn't we be able to save the cropping after?
}

/*=========================================================================

    protected

=========================================================================*/

QWidget *medVtkViewItkDataImageNavigator::buildToolBoxWidget()
{
    QWidget *toolBoxWidget = new QWidget;
    QFormLayout *layout = new QFormLayout(toolBoxWidget);
    for(medAbstractParameterL *parameter : d->parameters)
    {
        layout->addRow(parameter->getLabel(), parameter->getWidget());
    }
    toolBoxWidget->hide();
    return toolBoxWidget;
}

QWidget *medVtkViewItkDataImageNavigator::buildToolBarWidget()
{
    QWidget *toolBarWidget = new QWidget;
    toolBarWidget->hide();
    return toolBarWidget;
}
