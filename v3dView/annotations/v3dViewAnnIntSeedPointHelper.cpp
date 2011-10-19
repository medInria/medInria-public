/*
 * v3dViewAnnIntSeedPointHelper.cpp
 *
 *  Created on: 19 oct. 2011 08:34:58
 *      Author: jstark
 */

#include "v3dViewAnnIntSeedPointHelper.h"

#include "medSeedPointAnnotationData.h"

#include "v3dView.h"
#include "v3dViewAnnotationInteractor.h"

#include "vtkSeedPointHandle.h"

#include "vtkImageView2D.h"

#include <vtkCommand.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkSeedWidget.h>
#include <vtkSeedRepresentation.h>
#include <vtkSphereHandleRepresentation.h>
#include <vtkSmartPointer.h>


// This callback is responsible for setting the seed label.
class vtkSeedCallback : public vtkCommand
{
public:
    static vtkSeedCallback *New() 
    { return new vtkSeedCallback; }
    virtual void Execute(vtkObject*obj, unsigned long event, void *calldata)
    {
        if (event == vtkCommand::PlacePointEvent)
        {
        }
        if (event == vtkCommand::EndInteractionEvent)
        {
            vtkSeedWidget * spW = vtkSeedWidget::SafeDownCast(obj);
            if (spW)
            {
                helper->refreshFromWidget(spW);
            }
        }
    }
    v3dViewAnnIntSeedPointHelper * helper;
protected:
    vtkSeedCallback() {}
};


class v3dViewAnnIntSeedPointHelperPrivate {
public:
    vtkSmartPointer<vtkSeedCallback> spCbk;
};

v3dViewAnnIntSeedPointHelper::v3dViewAnnIntSeedPointHelper(v3dViewAnnotationInteractor * annInt) 
    : m_v3dViewAnnInt(annInt) ,
    d(new v3dViewAnnIntSeedPointHelperPrivate)
{

}

v3dViewAnnIntSeedPointHelper::~v3dViewAnnIntSeedPointHelper()
{
    delete d; d = NULL;
}

bool v3dViewAnnIntSeedPointHelper::addSeedPointAnnotation( medSeedPointAnnotationData * annData )
{
    v3dView * view = m_v3dViewAnnInt->getV3dView();

    double zero[2] = {0,0};
    typedef vtkSmartPointer<vtkSeedPointHandle> RepSmartPointer;
    RepSmartPointer handleRep = RepSmartPointer::New();

    vtkSmartPointer<vtkSeedWidget> seedActor2d = vtkSmartPointer<vtkSeedWidget>::New();
    seedActor2d->SetCurrentRenderer(view->renderer2d());
    seedActor2d->SetInteractor(view->renderer2d()->GetRenderWindow()->GetInteractor());
    seedActor2d->SetEnabled(1);
    seedActor2d->CreateDefaultRepresentation();
    seedActor2d->GetSeedRepresentation()->SetHandleRepresentation( handleRep );

    view->renderer2d()->AddActor(seedActor2d->GetRepresentation());

    vtkSmartPointer<vtkSeedWidget> seedActor3d = vtkSmartPointer<vtkSeedWidget>::New();
    seedActor3d->SetCurrentRenderer(view->renderer3d());
    seedActor3d->SetInteractor(view->renderer3d()->GetRenderWindow()->GetInteractor());
    seedActor3d->SetEnabled(1);
    seedActor3d->CreateDefaultRepresentation();
    seedActor3d->GetSeedRepresentation()->SetHandleRepresentation( handleRep );
    view->renderer3d()->AddActor(seedActor3d->GetRepresentation());

    v3dViewAnnotationInteractor::ActorInfo & actorInfo(m_v3dViewAnnInt->getActorMap()[annData]);
    actorInfo.actor2d = seedActor2d;
    actorInfo.actor3d = seedActor3d;
    actorInfo.AnnotationId =  v3dViewAnnotationInteractor::SeedPoint;

    this->seedPointModified(annData);

    d->spCbk = vtkSmartPointer<vtkSeedCallback>::New();
    d->spCbk->helper = this;
//    widget->AddObserver(vtkCommand::PlacePointEvent,scbk);
    seedActor2d->AddObserver(vtkCommand::EndInteractionEvent,d->spCbk);
    seedActor3d->AddObserver(vtkCommand::EndInteractionEvent,d->spCbk);
    m_v3dViewAnnInt->getV3dView()->update();

    return true;
}


void v3dViewAnnIntSeedPointHelper::removeSeedPointAnnotation( medSeedPointAnnotationData * annData )
{
    v3dView * view = m_v3dViewAnnInt->getV3dView();
    v3dViewAnnotationInteractor::ActorInfo & actorInfo(m_v3dViewAnnInt->getActorMap()[annData]);

    view->renderer2d()->RemoveActor(actorInfo.actor2d->GetRepresentation());
    view->renderer3d()->RemoveActor(actorInfo.actor3d->GetRepresentation());
    m_v3dViewAnnInt->getV3dView()->update();
}

void v3dViewAnnIntSeedPointHelper::seedPointModified( medSeedPointAnnotationData * annData )
{
    v3dViewAnnotationInteractor::ActorInfo & actorInfo(m_v3dViewAnnInt->getActorMap()[annData]);
    vtkSeedWidget * seedActor2d =  vtkSeedWidget::SafeDownCast(actorInfo.actor2d);
    vtkSeedWidget * seedActor3d =  vtkSeedWidget::SafeDownCast(actorInfo.actor3d);

    double p[3];

    const int numSeeds = annData->getNumberOfSeeds();

    vtkSeedRepresentation * rep2d = seedActor2d->GetSeedRepresentation();
    while ( rep2d->GetNumberOfSeeds() > numSeeds ) {
        seedActor2d->DeleteSeed(rep2d->GetNumberOfSeeds() - 1);
    }

    vtkSeedRepresentation * rep3d = seedActor3d->GetSeedRepresentation();
    while ( rep3d->GetNumberOfSeeds() > numSeeds ) {
        seedActor3d->DeleteSeed(rep3d->GetNumberOfSeeds() - 1);
    }

    for ( int iSeed(0); iSeed<numSeeds; ++iSeed) {
        medAbstractViewCoordinates::qtVector3dToDouble(annData->centerWorld(iSeed), p);
        //rep2d->GetHandleRepresentation(iSeed)->SetWorldPosition(p);
        //rep3d->GetHandleRepresentation(iSeed)->SetWorldPosition(p);
        seedActor2d->GetSeed(iSeed)->SetWorldPosition(p);
        seedActor3d->GetSeed(iSeed)->SetWorldPosition(p);
    }

    //if ( annData->isSelected() ) {
    //    seedActor2d->GetProperty()->SetColor(1,0,0);
    //} else {
    //    seedActor2d->GetProperty()->SetColor(0,1,1);
    //}
    m_v3dViewAnnInt->getV3dView()->update();
    return;
}

void v3dViewAnnIntSeedPointHelper::refreshFromWidget( vtkSeedWidget * spW )
{
    v3dViewAnnotationInteractor::ActorMap::iterator it;
    if ( ! m_v3dViewAnnInt->findActorMapForWidget(spW, it) ){
        return;
    }

    medSeedPointAnnotationData * spData = qobject_cast<medSeedPointAnnotationData*>(it->first);
    if ( !spData ) 
        return;

    vtkSeedRepresentation * rep = spW->GetSeedRepresentation();
    const int numSeeds = rep->GetNumberOfSeeds();

    if ( numSeeds < spData->getNumberOfSeeds() ) {
        spData->setNumberOfSeeds(numSeeds);
    }

    for ( int iSeed(0); iSeed<numSeeds; ++iSeed) {
        double pos[3];
        spW->GetSeedRepresentation()->GetSeedWorldPosition(iSeed,pos);
        spData->setCenterWorld(iSeed, medAbstractViewCoordinates::doubleToQtVector3d(pos));
    }
}
