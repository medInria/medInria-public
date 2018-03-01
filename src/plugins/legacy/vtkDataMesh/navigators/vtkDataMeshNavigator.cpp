/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkDataMeshNavigator.h"

#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

#include <medVtkViewBackend.h>

#include <medViewFactory.h>
#include <medAbstractImageView.h>
#include <medBoolParameterL.h>

#include <QLabel>

/*=========================================================================

    D pointer

=========================================================================*/


class vtkDataMeshNavigatorPrivate
{
public:
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;

    medAbstractImageView *imageView;

    QList <medAbstractParameterL*> parameters;

    medBoolParameterL *depthPeelingParameter;

};


/*=========================================================================

    public

=========================================================================*/


vtkDataMeshNavigator::vtkDataMeshNavigator(medAbstractView* parent):
    medAbstractNavigator(parent), d(new vtkDataMeshNavigatorPrivate)
{
    d->imageView =dynamic_cast<medAbstractImageView*>(parent);

    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;

    d->depthPeelingParameter = new medBoolParameterL("DepthPeeling", this);
    connect(d->depthPeelingParameter, SIGNAL(valueChanged(bool)), this, SLOT(enableDepthPeeling(bool)));

    d->parameters.append(d->depthPeelingParameter);
}

vtkDataMeshNavigator::~vtkDataMeshNavigator()
{
    delete d;
}

QString  vtkDataMeshNavigator::identifier() const
{
    return vtkDataMeshNavigator::s_identifier();
}

QString  vtkDataMeshNavigator::s_identifier()
{
    return "vtkDataMeshNavigator";
}

bool vtkDataMeshNavigator::registered()
{
    medViewFactory * factory = medViewFactory::instance();
    return factory->registerAdditionalNavigator<vtkDataMeshNavigator>(vtkDataMeshNavigator::s_identifier(),
                                                           QStringList() << "medVtkView");
}

QStringList vtkDataMeshNavigator::handled(void) const
{
    return QStringList() << "medVtkView";
}

QString vtkDataMeshNavigator::description() const
{
    return "Navigator to interact with meshes in a medVtkView";
}

QList<medAbstractParameterL*> vtkDataMeshNavigator::linkableParameters()
{
    return d->parameters;
}

QList<medBoolParameterL*> vtkDataMeshNavigator::mouseInteractionParameters()
{
    // no parameters related to mouse interactions
    return QList<medBoolParameterL*>();
}

void vtkDataMeshNavigator::updateWidgets()
{
    if(d->imageView->orientation() == medImageView::VIEW_ORIENTATION_3D &&
        (d->imageView->contains("vtkDataMesh")
         || d->imageView->contains("vtkDataMesh4D")
         ))
    {
        foreach(medAbstractParameterL *parameter, d->parameters)
            parameter->show();

    }
    else
        foreach(medAbstractParameterL *parameter, d->parameters)
            parameter->hide();
}

void vtkDataMeshNavigator::enableDepthPeeling(bool enabled)
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


QWidget *  vtkDataMeshNavigator::buildToolBoxWidget()
{
    QWidget *toolBoxWidget = new QWidget;
    QFormLayout *layout = new QFormLayout(toolBoxWidget);
    foreach(medAbstractParameterL *parameter, d->parameters)
        layout->addRow(parameter->getLabel(), parameter->getWidget());
    return toolBoxWidget;
}

QWidget *vtkDataMeshNavigator::buildToolBarWidget()
{
    QWidget *toolBarWidget = new QWidget;
    return toolBarWidget;
}





