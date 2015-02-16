/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medVtkViewItkScalarImageNavigator.h"

#include <vtkImageView2D.h>
#include <vtkImageView3D.h>

#include <medVtkViewBackend.h>

#include <medViewFactory.h>
#include <medAbstractImageView.h>
#include <medStringListParameter.h>
#include <medBoolParameter.h>


/*=========================================================================

    D pointer

=========================================================================*/


class medVtkViewItkScalarImageNavigatorPrivate
{
public:
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;

    medAbstractImageView *imageView;

    QList <medAbstractParameter*> parameters;

    medStringListParameter *mode3DParameter;
    medStringListParameter *renderer3DParameter;
    medBoolParameter *croppingParameter;

};


/*=========================================================================

    public

=========================================================================*/


medVtkViewItkScalarImageNavigator::medVtkViewItkScalarImageNavigator(medAbstractView* parent):
    medAbstractNavigator(parent), d(new medVtkViewItkScalarImageNavigatorPrivate)
{
    d->imageView =dynamic_cast<medAbstractImageView*>(parent);

    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;

    d->mode3DParameter = new medStringListParameter("3D Mode", this);
    d->mode3DParameter->addItem("VR");
    d->mode3DParameter->addItem("MIP - Maximum");
    d->mode3DParameter->addItem("MIP - Minimum");
    d->mode3DParameter->addItem("MSR");
    d->mode3DParameter->addItem("Off");
    d->mode3DParameter->setValue("MSR");
    connect(d->mode3DParameter, SIGNAL(valueChanged(QString)), this, SLOT(setMode3D(QString)));

    d->renderer3DParameter = new medStringListParameter("Renderer", this);
    d->renderer3DParameter->addItem("GPU");
    d->renderer3DParameter->addItem("Ray Cast / Texture");
    d->renderer3DParameter->addItem("Ray Cast");
    d->renderer3DParameter->addItem("Texture");
    d->renderer3DParameter->addItem("Default");
    connect(d->renderer3DParameter, SIGNAL(valueChanged(QString)), this, SLOT(setRenderer(QString)));

    d->croppingParameter = new medBoolParameter("Cropping", this);
    connect(d->croppingParameter, SIGNAL(valueChanged(bool)), this, SLOT(enableCropping(bool)));

    this->setMode3D("MSR");

    d->parameters.append(d->mode3DParameter);
    d->parameters.append(d->renderer3DParameter);
    d->parameters.append(d->croppingParameter);
}

medVtkViewItkScalarImageNavigator::~medVtkViewItkScalarImageNavigator()
{
    delete d;
}

QString  medVtkViewItkScalarImageNavigator::identifier() const
{
    return medVtkViewItkScalarImageNavigator::s_identifier();
}

QString  medVtkViewItkScalarImageNavigator::s_identifier()
{
    return "medVtkViewItkScalarImageNavigator";
}

bool medVtkViewItkScalarImageNavigator::registered()
{
    medViewFactory * factory = medViewFactory::instance();
    return factory->registerAdditionalNavigator<medVtkViewItkScalarImageNavigator>(medVtkViewItkScalarImageNavigator::s_identifier(),
                                                           QStringList() << "medVtkView");
}

QStringList medVtkViewItkScalarImageNavigator::handled(void) const
{
    return QStringList() << "medVtkView";
}

QString medVtkViewItkScalarImageNavigator::description() const
{
    return "Navigator to interact with itk images in a medVtkView";
}

QList<medAbstractParameter*> medVtkViewItkScalarImageNavigator::linkableParameters()
{
    return d->parameters;
}

QList<medBoolParameter*> medVtkViewItkScalarImageNavigator::mouseInteractionParameters()
{
    // no parameters related to mouse interactions
    return QList<medBoolParameter*>();
}

void medVtkViewItkScalarImageNavigator::updateWidgets()
{
    if(d->imageView->orientation() == medImageView::VIEW_ORIENTATION_3D &&
        (d->imageView->contains("medItkChar3ImageData")     ||
         d->imageView->contains("medItkUChar3ImageData")    ||
         d->imageView->contains("medItkShort3ImageData")    ||
         d->imageView->contains("medItkUShort3ImageData")   ||
         d->imageView->contains("medItkInt3ImageData")      ||
         d->imageView->contains("medItkLong3ImageData")     ||
         d->imageView->contains("medItkULong3ImageData")    ||
         d->imageView->contains("medItkFloat3ImageData")    ||
         d->imageView->contains("medItkDouble3ImageData")   ||
         d->imageView->contains("medItkRgb3ImageData")      ||
         d->imageView->contains("medItkRgba3ImageData")     ||

         d->imageView->contains("medItkChar4ImageData")     ||
         d->imageView->contains("medItkUChar4ImageData")    ||
         d->imageView->contains("medItkShort4ImageData")    ||
         d->imageView->contains("medItkUShort4ImageData")   ||
         d->imageView->contains("medItkInt4ImageData")      ||
         d->imageView->contains("medItkLong4ImageData")     ||
         d->imageView->contains("medItkULong4ImageData")    ||
         d->imageView->contains("medItkFloat4ImageData")    ||
         d->imageView->contains("medItkDouble4ImageData")))
    {
        if(this->toolBoxWidget())
            this->toolBoxWidget()->show();

    }
    else
        if(this->toolBoxWidget())
            this->toolBoxWidget()->hide();
}

void medVtkViewItkScalarImageNavigator::setMode3D(QString mode)
{
    if ( mode == "VR" )
    {
        d->view3d->SetRenderingModeToVR();
        d->view3d->SetVolumeRayCastFunctionToComposite();
        d->renderer3DParameter->show();
        d->croppingParameter->show();
        this->enableCropping(d->croppingParameter->value());
    }
    else if ( mode == "MSR" )
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

    d->imageView->render();
}

void medVtkViewItkScalarImageNavigator::setRenderer(QString renderer)
{
    if ( renderer=="GPU" )
        d->view3d->SetVolumeMapperToGPU();

    else if ( renderer=="Ray Cast / Texture" )
        d->view3d->SetVolumeMapperToRayCastAndTexture();

    else if ( renderer=="Ray Cast" )
        d->view3d->SetVolumeMapperToRayCast();

    else if ( renderer=="Default" )
        d->view3d->SetVolumeMapperToDefault();

    d->imageView->render();
}

void medVtkViewItkScalarImageNavigator::enableCropping(bool enabled)
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

    d->imageView->render();
    //TODO: Shouldn't we be able to save the cropping after?
}


/*=========================================================================

    protected

=========================================================================*/


QWidget *  medVtkViewItkScalarImageNavigator::buildToolBoxWidget()
{
    QWidget *toolBoxWidget = new QWidget;
    QFormLayout *layout = new QFormLayout(toolBoxWidget);
    foreach(medAbstractParameter *parameter, d->parameters)
        layout->addRow(parameter->getLabel(), parameter->getWidget());
    toolBoxWidget->hide();
    return toolBoxWidget;
}

QWidget *medVtkViewItkScalarImageNavigator::buildToolBarWidget()
{
    QWidget *toolBarWidget = new QWidget;
    toolBarWidget->hide();
    return toolBarWidget;
}





