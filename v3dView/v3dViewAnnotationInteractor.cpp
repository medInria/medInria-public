#include "v3dViewAnnotationInteractor.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkLog.h>

#include "medAnnotationData.h"
#include "medSeedPointAnnotationData.h"

#include "v3dView.h"

//
#include "v3dViewAnnIntSeedPointHelper.h"

#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkSeedWidget.h>
#include <vtkSeedRepresentation.h>
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
    v3dViewAnnotationInteractor::ActorMap  actors;

    QScopedPointer<v3dViewAnnIntSeedPointHelper> seedPointHelper;
};

// Implementation
v3dViewAnnotationInteractor::v3dViewAnnotationInteractor(): medAbstractAnnotationViewInteractor(), d(new v3dViewAnnotationInteractorPrivate)
{
    // addProperty here
//    this->addProperty("Visibility", QStringList() << "true" << "false");
    d->seedPointHelper.reset(new v3dViewAnnIntSeedPointHelper(this));
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
    ActorMap::iterator it = d->actors.find(data);

    if ( it == d->actors.end() )
        return;

    switch( it->second.AnnotationId ) {
    case SeedPoint:
        d->seedPointHelper->seedPointModified(qobject_cast<medSeedPointAnnotationData*>(data));
        break;
    }
}

bool v3dViewAnnotationInteractor::onAddAnnotation( medAnnotationData * annItem )
{
    bool isAdded = false;
    if ( medSeedPointAnnotationData * spad = qobject_cast<medSeedPointAnnotationData*>(annItem) ) {
        isAdded =  d->seedPointHelper->addSeedPointAnnotation( spad );
    }
    return isAdded;
}

void v3dViewAnnotationInteractor::onRemoveAnnotation( medAnnotationData * annData )
{
    ActorMap::iterator it = d->actors.find(annData);
    if ( it == d->actors.end() )
        return;

    if ( medSeedPointAnnotationData * spad = qobject_cast<medSeedPointAnnotationData*>(annData) ) {
         d->seedPointHelper->removeSeedPointAnnotation( spad );
    }

    d->actors.erase(annData);
}

v3dViewAnnotationInteractor::ActorMap & v3dViewAnnotationInteractor::getActorMap()
{
    return d->actors;
}

bool v3dViewAnnotationInteractor::findActorMapForWidget(vtkAbstractWidget * w, ActorMap::iterator & it)
{
    for( it = d->actors.begin(); it != d->actors.end(); ++it ) {
        if ( ( it->second.actor2d == w ) ||
             ( it->second.actor3d == w ) ) 
        {
            return true;
        }
    }
    return false;
}



