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
    typedef vtkSmartPointer <vtkProperty>  PropertySmartPointer;

    dtkAbstractData  *data;
    v3dView          *view;
    ActorSmartPointer     actor2d;
    ActorSmartPointer     actor3d;
    PropertySmartPointer  actorProperty;


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
	Q_UNUSED( pointSet );
        d->data = data;
        updatePipeline ();
    }
}

void v3dViewMeshInteractor::setView(dtkAbstractView *view)
{
    if (v3dView *v3dview = dynamic_cast<v3dView*>(view) ) {
        d->view = v3dview;
    }
}

bool v3dViewMeshInteractor::isAutoEnabledWith ( dtkAbstractData * data )
{
    if ( data->description() == "vtkDataMesh" ) {

        this->enable ();
        return true;
    }
    return false;
}

void v3dViewMeshInteractor::enable(void)
{
    if (this->enabled())
        return;

    updatePipeline ();

    dtkAbstractViewInteractor::enable();
}


void v3dViewMeshInteractor::disable(void)
{
    if (!this->enabled())
        return;

    if (d->view) {

// TODO        d->view->view3D ()->RemoveDataset ();
// TODO        d->view->view3D ()->RemoveDataset ();
// TODO        d->view->view3D ()->RemoveDataset ();
// TODO        d->view->view3D ()->RemoveDataset ();
    }
    dtkAbstractViewInteractor::disable();
}

void v3dViewMeshInteractor::onPropertySet(const QString& key, const QString& value)
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

void v3dViewMeshInteractor::onVisibilityPropertySet (const QString& value)
{
    if (value=="true") {
        d->actor2d->SetVisibility(1);
        d->actor3d->SetVisibility(1);
    } else {
        d->actor2d->SetVisibility(0);
        d->actor3d->SetVisibility(0);
    }
}

void v3dViewMeshInteractor::onEdgeVisibilityPropertySet (const QString& value)
{
    if (value=="true")
        d->actorProperty->SetEdgeVisibility (1);
    else
        d->actorProperty->SetEdgeVisibility (0);
}

void v3dViewMeshInteractor::onRenderingModePropertySet (const QString& value)
{
    if (value=="wireframe") {
        d->actorProperty->SetRepresentationToWireframe ();
    } else if (value=="surface") {
        d->actorProperty->SetRepresentationToSurface ();
    } else if (value=="points") {
        d->actorProperty->SetRepresentationToPoints ();
    }
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractViewInteractor *createV3dViewMeshInteractor(void)
{
    return new v3dViewMeshInteractor;
}

void v3dViewMeshInteractor::updatePipeline (void)
{
    if (d->view && d->data) {

        if(vtkPointSet *pointset = dynamic_cast<vtkPointSet*>((vtkObject *)(d->data->data()))) {
            d->actor2d = d->view->view2d ()->AddDataSet (pointset);
            d->actor3d = d->view->view3d ()->AddDataSet(pointset);

            d->actorProperty = v3dViewMeshInteractorPrivate::PropertySmartPointer::New ();

            d->actorProperty->SetColor ( 1,0,0 );
            d->actor2d->SetProperty ( d->actorProperty );
            d->actor3d->SetProperty ( d->actorProperty );

            d->actor2d->GetMapper ()->ScalarVisibilityOff ();
            d->actor3d->GetMapper ()->ScalarVisibilityOff ();
        }
    }
}
