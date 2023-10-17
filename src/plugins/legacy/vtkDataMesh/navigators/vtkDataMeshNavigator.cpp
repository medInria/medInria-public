/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.

 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.

=========================================================================*/

#include "vtkDataMeshNavigator.h"

#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

#include <medAbstractImageView.h>
#include <medBoolParameterL.h>
#include <medStringListParameterL.h>
#include <medViewFactory.h>
#include <medVtkViewBackend.h>

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
    medStringListParameterL *colorBackgroundParam;
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

    // Depth Peeling
    d->depthPeelingParameter = new medBoolParameterL("DepthPeeling", this);
    connect(d->depthPeelingParameter, SIGNAL(valueChanged(bool)), this, SLOT(enableDepthPeeling(bool)));

    d->parameters.append(d->depthPeelingParameter);

    // Background
    QStringList colors = createColorList();

    d->colorBackgroundParam = new medStringListParameterL("Background Color", this);

    for(QString color : colors)
    {
        d->colorBackgroundParam->addItem(color, medStringListParameterL::createIconFromColor(color));
    }

    connect(d->colorBackgroundParam, SIGNAL(valueChanged(QString)), this, SLOT(setBackgroundColor(QString)));

    d->colorBackgroundParam->setValue("#000000");

    d->parameters.append(d->colorBackgroundParam);
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
        for(medAbstractParameterL *parameter : d->parameters)
        {
            parameter->show();
        }
    }
    else
    {
        for(medAbstractParameterL *parameter : d->parameters)
        {
            parameter->hide();
        }
    }
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

QStringList vtkDataMeshNavigator::createColorList()
{
    QStringList colors;
    colors << "#FFFFFF";
    colors << "#808080";
    colors << "#800000";
    colors << "#804040";
    colors << "#FF8080";
    colors << "#FF0000";
    colors << "#FFFF80";
    colors << "#FFFF00";
    colors << "#FF8040";
    colors << "#FF8000";
    colors << "#80FF80";
    colors << "#80FF00";
    colors << "#00FF00";
    colors << "#80FFFF";
    colors << "#00FFFF";
    colors << "#004080";
    colors << "#0000FF";
    colors << "#0080FF";
    colors << "#0080C0";
    colors << "#000000";
    return colors;
}

void vtkDataMeshNavigator::setBackgroundColor(QColor color)
{
    if( !color.isValid() )
    {
        color.setRgb(0, 0, 0);
    }

    double r,g,b;
    color.getRgbF(&r, &g, &b);

    d->view3d->SetBackground(r, g, b);

    d->imageView->render();
}

void vtkDataMeshNavigator::setBackgroundColor(const QString &color)
{
    d->colorBackgroundParam->setValue(color);
    setBackgroundColor(QColor(color));
}

QString vtkDataMeshNavigator::name() const
{
    return "vtkDataMeshNavigator";
}

/*=========================================================================

    protected

=========================================================================*/


QWidget *  vtkDataMeshNavigator::buildToolBoxWidget()
{
    QWidget *toolBoxWidget = new QWidget;
    QFormLayout *layout = new QFormLayout(toolBoxWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setLabelAlignment(Qt::AlignLeft);

    for(medAbstractParameterL *parameter : d->parameters)
    {
        layout->addRow(parameter->getLabel(), parameter->getWidget());
    }
    return toolBoxWidget;
}

QWidget *vtkDataMeshNavigator::buildToolBarWidget()
{
    QWidget *toolBarWidget = new QWidget;
    return toolBarWidget;
}





