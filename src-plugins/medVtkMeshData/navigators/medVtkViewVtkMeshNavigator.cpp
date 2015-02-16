/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#include "medVtkViewVtkMeshNavigator.h"

#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

#include <medVtkViewBackend.h>

#include <medViewFactory.h>
#include <medAbstractImageView.h>
#include <medBoolParameter.h>


/*=========================================================================

    D pointer

=========================================================================*/


class medVtkViewVtkMeshNavigatorPrivate
{
public:
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;

    medAbstractImageView *imageView;

    QList <medAbstractParameter*> parameters;

    medBoolParameter *depthPeelingParameter;

};


/*=========================================================================

    public

=========================================================================*/


medVtkViewVtkMeshNavigator::medVtkViewVtkMeshNavigator(medAbstractView* parent):
    medAbstractNavigator(parent), d(new medVtkViewVtkMeshNavigatorPrivate)
{
    d->imageView =dynamic_cast<medAbstractImageView*>(parent);

    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;

    d->depthPeelingParameter = new medBoolParameter("DepthPeeling", this);
    connect(d->depthPeelingParameter, SIGNAL(valueChanged(bool)), this, SLOT(enableDepthPeeling(bool)));

    d->parameters.append(d->depthPeelingParameter);
}

medVtkViewVtkMeshNavigator::~medVtkViewVtkMeshNavigator()
{
    delete d;
}

QString  medVtkViewVtkMeshNavigator::identifier() const
{
    return medVtkViewVtkMeshNavigator::s_identifier();
}

QString  medVtkViewVtkMeshNavigator::s_identifier()
{
    return "medVtkViewVtkMeshNavigator";
}

bool medVtkViewVtkMeshNavigator::registered()
{
    medViewFactory * factory = medViewFactory::instance();
    return factory->registerAdditionalNavigator<medVtkViewVtkMeshNavigator>(medVtkViewVtkMeshNavigator::s_identifier(),
                                                           QStringList() << "medVtkView");
}

QStringList medVtkViewVtkMeshNavigator::handled(void) const
{
    return QStringList() << "medVtkView";
}

QString medVtkViewVtkMeshNavigator::description() const
{
    return "Navigator to interact with meshes in a medVtkView";
}

QList<medAbstractParameter*> medVtkViewVtkMeshNavigator::linkableParameters()
{
    return d->parameters;
}

QList<medBoolParameter*> medVtkViewVtkMeshNavigator::mouseInteractionParameters()
{
    // no parameters related to mouse interactions
    return QList<medBoolParameter*>();
}

void medVtkViewVtkMeshNavigator::updateWidgets()
{
    if(d->imageView->orientation() == medImageView::VIEW_ORIENTATION_3D &&
        (d->imageView->contains("medVtkMeshData")
         || d->imageView->contains("medVtk4DMeshData")
         ))
    {
        foreach(medAbstractParameter *parameter, d->parameters)
            parameter->show();

    }
    else
        foreach(medAbstractParameter *parameter, d->parameters)
            parameter->hide();
}

void medVtkViewVtkMeshNavigator::enableDepthPeeling(bool enabled)
{
    if ( enabled )
    {
        // Activate depth-peeling to have a proper opacity rendering
        d->view3d->GetRenderer()->SetUseDepthPeeling(1);
        d->view3d->GetRenderer()->SetMaximumNumberOfPeels(100);
        d->view3d->GetRenderer()->SetOcclusionRatio(0.01);

    }
    else d->view3d->GetRenderer()->SetUseDepthPeeling(0);

    d->imageView->render();
}


/*=========================================================================

    protected

=========================================================================*/


QWidget *  medVtkViewVtkMeshNavigator::buildToolBoxWidget()
{
    QWidget *toolBoxWidget = new QWidget;
    QFormLayout *layout = new QFormLayout(toolBoxWidget);
    foreach(medAbstractParameter *parameter, d->parameters)
        layout->addRow(parameter->getLabel(), parameter->getWidget());
    return toolBoxWidget;
}

QWidget *medVtkViewVtkMeshNavigator::buildToolBarWidget()
{
    QWidget *toolBarWidget = new QWidget;
    return toolBarWidget;
}





