#include "vtkDataMeshNavigator.h"

#include <vtkImageView2D.h>
#include <vtkImageView3D.h>

#include <medVtkViewBackend.h>

#include <medViewFactory.h>
#include <medAbstractImageView.h>
#include <medBoolParameter.h>


/*=========================================================================

    D pointer

=========================================================================*/


class vtkDataMeshNavigatorPrivate
{
public:
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;
    vtkRenderWindow *renderWindow;

    medAbstractImageView *imageView;

    QList <medAbstractParameter*> parameters;

    medBoolParameter *depthPeelingParameter;

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
    d->renderWindow = backend->renWin;

    d->depthPeelingParameter = new medBoolParameter("DepthPeeling", this);
    connect(d->depthPeelingParameter, SIGNAL(valueChanged(bool)), this, SLOT(enableDepthPeeling(bool)));

    d->parameters.append(d->depthPeelingParameter);


    // Necessary options for depth-peeling
    d->renderWindow->SetAlphaBitPlanes(1);
    d->renderWindow->SetMultiSamples(0);
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

QList<medAbstractParameter*> vtkDataMeshNavigator::parameters()
{
    return d->parameters;
}

void vtkDataMeshNavigator::updateWidgets()
{
    if(d->imageView->orientation() == medImageView::VIEW_ORIENTATION_3D &&
        (d->imageView->contains("vtkDataMesh") || d->imageView->contains("vtkDataMesh4D")))
    {
        if(this->toolBoxWidget())
            this->toolBoxWidget()->show();
    }
    else
        if(this->toolBoxWidget())
            this->toolBoxWidget()->hide();
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

    d->renderWindow->Render();
}


/*=========================================================================

    protected

=========================================================================*/


QWidget *  vtkDataMeshNavigator::buildToolBoxWidget()
{
    QWidget *toolBoxWidget = new QWidget;
    QFormLayout *layout = new QFormLayout(toolBoxWidget);
    foreach(medAbstractParameter *parameter, d->parameters)
        layout->addRow(parameter->getLabel(), parameter->getWidget());
    toolBoxWidget->hide();
    return toolBoxWidget;
}

QWidget *vtkDataMeshNavigator::buildToolBarWidget()
{
    QWidget *toolBarWidget = new QWidget;
    toolBarWidget->hide();
    return toolBarWidget;
}





