#include "v3dViewAnnotationInteractor.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkLog.h>

#include "medAnnotationData.h"
#include "medSeedPointAnnotationData.h"

#include "v3dView.h"

#include "vtkSeedPointActor2D.h"

#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkSeedWidget.h>
#include <vtkSphereHandleRepresentation.h>
#include <vtkSmartPointer.h>
// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractViewInteractor *createV3dViewAnnotationInteractor(void)
{
    return new v3dViewAnnotationInteractor;
}

// pImpl
class v3dViewAnnotationInteractorPrivate
{
public:
    typedef vtkSmartPointer <vtkProp >  PropSmartPointer;
    typedef vtkSmartPointer <vtkProperty>  PropertySmartPointer;
    enum EAnnotation { SeedPoint };
    struct ActorInfo {
        PropSmartPointer actor2d;
        PropSmartPointer actor3d;
        PropertySmartPointer actorProperty;
        EAnnotation AnnotationId;
    };

    typedef std::map< medAnnotationData*, ActorInfo> ActorMap;
    ActorMap  actors;
};

// Implementation
v3dViewAnnotationInteractor::v3dViewAnnotationInteractor(): medAbstractAnnotationViewInteractor(), d(new v3dViewAnnotationInteractorPrivate)
{
    // addProperty here
//    this->addProperty("Visibility", QStringList() << "true" << "false");
}

v3dViewAnnotationInteractor::~v3dViewAnnotationInteractor()
{
    this->disable();

    delete d;
    d = NULL;
}

QString v3dViewAnnotationInteractor::description(void) const
{
    return tr("Interactor displaying Meshes");
}

QString v3dViewAnnotationInteractor::identifier() const
{
    return s_identifier();
}

QStringList v3dViewAnnotationInteractor::handled(void) const
{
    return QStringList () << v3dView::s_identifier();
}

bool v3dViewAnnotationInteractor::registered(void)
{
    return dtkAbstractViewFactory::instance()->registerViewInteractorType(
        v3dViewAnnotationInteractor::s_identifier(), 
        QStringList() << v3dView::s_identifier(), createV3dViewAnnotationInteractor);
}

bool v3dViewAnnotationInteractor::isAutoEnabledWith ( dtkAbstractData * data )
{
    if ( data ) 
        return true;
    return false;
}

void v3dViewAnnotationInteractor::enable(void)
{
    dtkWarning() << "enabling " << this->identifier() ;

    if (this->enabled())
        return;

    //updatePipeline ();

    dtkAbstractViewInteractor::enable();
}


void v3dViewAnnotationInteractor::disable(void)
{
    if (!this->enabled())
        return;

    if (this->view()) {

        // TODO        d->view->view3D ()->RemoveDataset ();
        // TODO        d->view->view3D ()->RemoveDataset ();
        // TODO        d->view->view3D ()->RemoveDataset ();
        // TODO        d->view->view3D ()->RemoveDataset ();
    }
    dtkAbstractViewInteractor::disable();
}

void v3dViewAnnotationInteractor::onPropertySet(const QString& key, const QString& value)
{
    if (key == ("Visibility"))
        this->onVisibilityPropertySet (value);

    if (this->view())
        this->view()->update();
}

void v3dViewAnnotationInteractor::onVisibilityPropertySet (const QString& value)
{

}


QString v3dViewAnnotationInteractor::s_identifier()
{
    return "v3dViewAnnotationInteractor";
}

v3dView * v3dViewAnnotationInteractor::getV3dView()
{
    return qobject_cast<v3dView *>(this->view());
}

void v3dViewAnnotationInteractor::initialize( medAbstractView * view, medAbstractData* data )
{
    BaseClass::initialize(view,data);
}

void v3dViewAnnotationInteractor::onDataModified( medAnnotationData* data )
{
    typedef v3dViewAnnotationInteractorPrivate::ActorMap ActorMap;
    ActorMap::iterator it = d->actors.find(data);

    if ( it == d->actors.end() )
        return;

    switch( it->second.AnnotationId ) {
    case d->SeedPoint:
        this->seedPointModified(qobject_cast<medSeedPointAnnotationData*>(data));
        break;
    }
}

bool v3dViewAnnotationInteractor::onAddAnnotation( medAnnotationData * annItem )
{
    bool isAdded = false;
    if ( medSeedPointAnnotationData * spad = qobject_cast<medSeedPointAnnotationData*>(annItem) ) {
        isAdded = this->addSeedPointAnnotation( spad );
    }
    return isAdded;
}

void v3dViewAnnotationInteractor::onRemoveAnnotation( medAnnotationData * annData )
{
    typedef v3dViewAnnotationInteractorPrivate::ActorMap ActorMap;
    ActorMap::iterator it = d->actors.find(annData);
    if ( it == d->actors.end() )
        return;

    if ( medSeedPointAnnotationData * spad = qobject_cast<medSeedPointAnnotationData*>(annData) ) {
        this->removeSeedPointAnnotation( spad );
    }

    d->actors.erase(annData);
}

bool v3dViewAnnotationInteractor::addSeedPointAnnotation( medSeedPointAnnotationData * annData )
{
    v3dView * view = this->getV3dView();

    vtkSmartPointer<vtkSeedPointActor2D> seedActor2d = vtkSmartPointer<vtkSeedPointActor2D>::New();
    view->renderer2d()->AddActor2D(seedActor2d);
    seedActor2d->VisibilityOn();

    vtkSmartPointer<vtkSphereHandleRepresentation> seedActor3d = vtkSmartPointer<vtkSphereHandleRepresentation>::New();
    view->renderer3d()->AddActor(seedActor3d);
    seedActor3d->VisibilityOn();

    d->actors[annData].actor2d = seedActor2d;
    d->actors[annData].actor3d = seedActor3d;
    d->actors[annData].AnnotationId =  d->SeedPoint;

    this->seedPointModified(annData);
    return true;
}


void v3dViewAnnotationInteractor::removeSeedPointAnnotation( medSeedPointAnnotationData * annData )
{
    v3dView * view = this->getV3dView();

    view->renderer2d()->RemoveActor2D(d->actors[annData].actor2d);
    view->renderer3d()->RemoveActor(d->actors[annData].actor3d);
}

void v3dViewAnnotationInteractor::seedPointModified( medSeedPointAnnotationData * annData )
{
    vtkSeedPointActor2D * seedActor2d =  vtkSeedPointActor2D::SafeDownCast( d->actors[annData].actor2d );
    vtkSphereHandleRepresentation * seedActor3d =  vtkSphereHandleRepresentation::SafeDownCast( d->actors[annData].actor3d );

    double p[3];
    medAbstractViewCoordinates::qtVector3dToDouble(annData->centerWorld(), p);
    seedActor2d->SetWorldPosition(p);
    seedActor3d->SetWorldPosition(p);

    if ( annData->isSelected() ) {
        seedActor2d->GetProperty()->SetColor(1,0,0);
    } else {
        seedActor2d->GetProperty()->SetColor(0,1,1);
    }
    this->getV3dView()->update();
    return;
}
