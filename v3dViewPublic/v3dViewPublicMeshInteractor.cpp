#include "v3dViewPublicMeshInteractor.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>

#include <vtkActor.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkViewImage.h>
#include <vtkViewImage2D.h>
#include <vtkViewImage3D.h>
#include <vtkPointSet.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include "v3dViewPublic.h"
#include <QInputDialog>
#include <QColorDialog>

class v3dViewPublicMeshInteractorPrivate
{
public:
    typedef vtkSmartPointer <vtkActor>  ActorSmartPointer;
    typedef vtkSmartPointer <vtkProperty>  PropertySmartPointer;

    dtkAbstractData  *data;
    v3dViewPublic          *view;
    ActorSmartPointer     actor2d;
    ActorSmartPointer     actor3d;
    PropertySmartPointer  actorProperty;
};

v3dViewPublicMeshInteractor::v3dViewPublicMeshInteractor(): dtkAbstractViewInteractor(), d(new v3dViewPublicMeshInteractorPrivate)
{
    d->data = NULL;
    d->view = NULL;
    
    // addProperty here
    this->addProperty("Visibility", QStringList() << "true" << "false");
    this->addProperty("ShowEdges", QStringList() << "true" << "false");
    this->addProperty("RenderingMode", QStringList() << "surface" << "wireframe" << "points" );
}

v3dViewPublicMeshInteractor::~v3dViewPublicMeshInteractor()
{
    this->disable();

    delete d;
    d = 0;
}

QString v3dViewPublicMeshInteractor::description(void) const
{
    return "v3dViewMeshInteractor";
}

QStringList v3dViewPublicMeshInteractor::handled(void) const
{
    return QStringList () << "v3dViewPublic";
}

bool v3dViewPublicMeshInteractor::registered(void)
{
    return dtkAbstractViewFactory::instance()->registerViewInteractorType("v3dViewMeshInteractor", QStringList() << "v3dViewPublic", createV3dViewPublicMeshInteractor);
}

void v3dViewPublicMeshInteractor::setData(dtkAbstractData *data)
{
    if (vtkPointSet *pointSet = dynamic_cast<vtkPointSet *>((vtkDataObject *)(data->data()))) {
	Q_UNUSED( pointSet );
        d->data = data;
        updatePipeline ();
    }
}

void v3dViewPublicMeshInteractor::setView(dtkAbstractView *view)
{
    if (v3dViewPublic *v3dview = dynamic_cast<v3dViewPublic*>(view) ) {
        d->view = v3dview;
    }
}

bool v3dViewPublicMeshInteractor::isAutoEnabledWith ( dtkAbstractData * data )
{
    if ( data->description() == "vtkDataMesh" ) {

        this->enable ();
        return true;
    }
    return false;
}

void v3dViewPublicMeshInteractor::enable(void)
{
    if (this->enabled())
        return;

    updatePipeline ();

    dtkAbstractViewInteractor::enable();
}


void v3dViewPublicMeshInteractor::disable(void)
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

void v3dViewPublicMeshInteractor::onPropertySet(const QString& key, const QString& value)
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

void v3dViewPublicMeshInteractor::onVisibilityPropertySet (const QString& value)
{
    if (value=="true") {
        d->actor2d->SetVisibility(1);
        d->actor3d->SetVisibility(1);
    } else {
        d->actor2d->SetVisibility(0);
        d->actor3d->SetVisibility(0);
    }
}

void v3dViewPublicMeshInteractor::onEdgeVisibilityPropertySet (const QString& value)
{
    if (value=="true")
        d->actorProperty->SetEdgeVisibility (1);
    else
        d->actorProperty->SetEdgeVisibility (0);
}

void v3dViewPublicMeshInteractor::onRenderingModePropertySet (const QString& value)
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

dtkAbstractViewInteractor *createV3dViewPublicMeshInteractor(void)
{
    return new v3dViewPublicMeshInteractor;
}

void v3dViewPublicMeshInteractor::updatePipeline (void)
{
    if (d->view && d->data) {

        if(vtkPointSet *pointset = dynamic_cast<vtkPointSet*>((vtkObject *)(d->data->data()))) {
            d->actor2d = d->view->view2D ()->AddDataSet (pointset);
            d->actor3d = d->view->view3D ()->AddDataSet(pointset);

            d->actorProperty = v3dViewPublicMeshInteractorPrivate::PropertySmartPointer::New ();

            d->actorProperty->SetColor ( 1,0,0 );
            if (d->actor2d)
                d->actor2d->SetProperty ( d->actorProperty );
            if (d->actor3d)
                d->actor3d->SetProperty ( d->actorProperty );

            if (d->actor2d)
                d->actor2d->GetMapper ()->ScalarVisibilityOff ();
            if (d->actor3d)
            d->actor3d->GetMapper ()->ScalarVisibilityOff ();
        }
    }
}
