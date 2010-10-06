#include "v3dViewMeshInteractor.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>

#include <vtkActor.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkImageView.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkPointSet.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include "v3dView.h"
#include <QInputDialog>
#include <QColorDialog>

class v3dViewMeshInteractorPrivate
{
public:
    typedef vtkSmartPointer <vtkActor>  ActorSmartPointer;

    dtkAbstractData  *data;
    v3dView          *view;
    ActorSmartPointer     actor;

};

v3dViewMeshInteractor::v3dViewMeshInteractor(): dtkAbstractViewInteractor(), d(new v3dViewMeshInteractorPrivate)
{
    d->data = NULL;
    d->view = NULL;
    
    // addProperty here
    this->addProperty("Visibility", QStringList() << "true" << "false");
    this->addProperty("ShowEdges", QStringList() << "true" << "false");
    this->addProperty("RenderingMode", QStringList() << "surface" << "wireframe" << "points" );
}

v3dViewMeshInteractor::~v3dViewMeshInteractor()
{
    this->disable();

    delete d;
    d = 0;
}

QString v3dViewMeshInteractor::description(void) const
{
    return "v3dViewMeshInteractor";
}

QStringList v3dViewMeshInteractor::handled(void) const
{
    return QStringList () << "v3dView";
}

bool v3dViewMeshInteractor::registered(void)
{
    return dtkAbstractViewFactory::instance()->registerViewInteractorType("v3dViewMeshInteractor", QStringList() << "v3dView", createV3dViewMeshInteractor);
}

void v3dViewMeshInteractor::setData(dtkAbstractData *data)
{
    if (vtkPointSet *pointSet = dynamic_cast<vtkPointSet *>((vtkDataObject *)(data->data()))) {

        d->data = data;
    }
}

void v3dViewMeshInteractor::setView(dtkAbstractView *view)
{
    if (v3dView *v3dview = dynamic_cast<v3dView*>(view) ) {
        d->view = v3dview;
    }
}

void v3dViewMeshInteractor::enable(void)
{
    if (this->enabled())
        return;

    if (d->view) {

        vtkSmartPointer < vtkActor > actor;
        if ( vtkPolyData *polydata = dynamic_cast<vtkPolyData *>((vtkObject *)(d->data->data())) ) {

            vtkSmartPointer < vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New ();
            mapper->SetInput ( polydata );
            actor = vtkSmartPointer < vtkActor>::New ();
            actor->SetMapper ( mapper );

        } else if (vtkDataSet *dataset = dynamic_cast<vtkDataSet *>((vtkObject *)(d->data->data())) ) {

            vtkSmartPointer < vtkDataSetSurfaceFilter > surfaceFilter = vtkSmartPointer<vtkDataSetSurfaceFilter>::New ();
            surfaceFilter->SetInput ( dataset );

            vtkSmartPointer < vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New ();
            mapper->SetInputConnection ( surfaceFilter->GetOutputPort () );
            actor = vtkSmartPointer < vtkActor>::New ();
            actor->SetMapper ( mapper );

        }

        if ( actor.GetPointer () ) {

            d->actor = actor;
            d->view->renderer3D()->AddViewProp ( actor );
        }
    }
    dtkAbstractViewInteractor::enable();
}

void v3dViewMeshInteractor::disable(void)
{
    if (!this->enabled())
        return;

    if (d->view) {

        d->view->renderer3D()->RemoveViewProp ( d->actor );
    }
    dtkAbstractViewInteractor::disable();
}

void v3dViewMeshInteractor::onPropertySet(QString key, QString value)
{
    if (key=="Visibility")
        this->onVisibilityPropertySet (value);
    
    if (key=="ShowEdges")
        this->onEdgeVisibilityPropertySet (value);

    if (key=="RenderingMode")
        this->onRenderingModePropertySet (value);
        
    if (d->view)
        d->view->update();
}

void v3dViewMeshInteractor::onVisibilityPropertySet (QString value)
{
    if (value=="true")
        d->actor->SetVisibility(1);
    else
        d->actor->SetVisibility(0);
}

void v3dViewMeshInteractor::onEdgeVisibilityPropertySet (QString value)
{
    if (value=="true")
        d->actor->GetProperty ()->SetEdgeVisibility (1);
    else
        d->actor->GetProperty ()->SetEdgeVisibility (0);
}

void v3dViewMeshInteractor::onRenderingModePropertySet (QString value)
{
    if (value=="wireframe") {
        d->actor->GetProperty ()->SetRepresentationToWireframe ();
    } else if (value=="surface") {
        d->actor->GetProperty ()->SetRepresentationToSurface ();
    } else if (value=="points") {
        d->actor->GetProperty ()->SetRepresentationToPoints ();
    }
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractViewInteractor *createV3dViewMeshInteractor(void)
{
    return new v3dViewMeshInteractor;
}
