/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
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
#include <medStringListParameter.h>
#include <medBoolParameter.h>


/*=========================================================================

    D pointer

=========================================================================*/


class medVtkViewItkDataImageNavigatorPrivate
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


medVtkViewItkDataImageNavigator::medVtkViewItkDataImageNavigator(medAbstractView* parent):
    medAbstractNavigator(parent), d(new medVtkViewItkDataImageNavigatorPrivate)
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

QList<medAbstractParameter*> medVtkViewItkDataImageNavigator::linkableParameters()
{
    return d->parameters;
}

QList<medBoolParameter*> medVtkViewItkDataImageNavigator::mouseInteractionParameters()
{
    // no parameters related to mouse interactions
    return QList<medBoolParameter*>();
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
         d->imageView->contains("itkDataImageDouble4")   ||
         d->imageView->contains("itkDataImageRGB4")))
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





