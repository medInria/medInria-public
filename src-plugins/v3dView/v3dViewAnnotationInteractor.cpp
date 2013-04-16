/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "v3dViewAnnotationInteractor.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkLog/dtkLog.h>

#include "medAnnotationData.h"
#include "medSeedPointAnnotationData.h"
#include "medImageMaskAnnotationData.h"

#include "v3dView.h"

//
#include "v3dViewAnnIntSeedPointHelper.h"
#include "v3dViewAnnIntImageMaskHelper.h"

#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkSeedWidget.h>
#include <vtkSeedRepresentation.h>
#include <vtkSphereHandleRepresentation.h>
#include <vtkSmartPointer.h>
// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractViewInteractor *createV3dViewAnnotationInteractor()
{
    return new v3dViewAnnotationInteractor;
}

// pImpl
class v3dViewAnnotationInteractorPrivate
{
public:
    typedef QVector<v3dViewAnnIntHelper *> HelperVecType;
    HelperVecType helpers;

    typedef std::map< medAnnotationData*, HelperVecType::size_type> HelperMap;
    HelperMap  dataToHelperIdMap;
};

// Implementation
v3dViewAnnotationInteractor::v3dViewAnnotationInteractor(): medAbstractAnnotationViewInteractor(), d(new v3dViewAnnotationInteractorPrivate)
{
    // addProperty here
//    this->addProperty("Visibility", QStringList() << "true" << "false");
    d->helpers.push_back(new v3dViewAnnIntSeedPointHelper(this));
    d->helpers.push_back(new v3dViewAnnIntImageMaskHelper(this));
}

v3dViewAnnotationInteractor::~v3dViewAnnotationInteractor()
{
    this->disable();
    qDeleteAll(d->helpers);
    delete d;
    d = NULL;
}

QString v3dViewAnnotationInteractor::description() const
{
    return tr("Interactor displaying annotations");
}

QString v3dViewAnnotationInteractor::identifier() const
{
    return s_identifier();
}

QStringList v3dViewAnnotationInteractor::handled() const
{
    return QStringList () << v3dView::s_identifier();
}

bool v3dViewAnnotationInteractor::registered()
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

void v3dViewAnnotationInteractor::enable()
{
    dtkWarn() << "enabling " << this->identifier() ;

    if (this->enabled())
        return;

    //updatePipeline ();

    dtkAbstractViewInteractor::enable();
}


void v3dViewAnnotationInteractor::disable()
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

void v3dViewAnnotationInteractor::setView(dtkAbstractView* view)
{
    BaseClass::setView(view);
    if (view)     setData((dtkAbstractData*)view->data());
    else setData(NULL);
}


void v3dViewAnnotationInteractor::initialize( medAbstractView * view, medAbstractData* data )
{
    BaseClass::initialize(view,data);
}

void v3dViewAnnotationInteractor::onDataModified( medAbstractData* data )
{
    medAnnotationData * annData = qobject_cast<medAnnotationData*>(data);
    if (!annData)
        return;

    v3dViewAnnotationInteractorPrivate::HelperMap::iterator it = d->dataToHelperIdMap.find(annData);

    if ( it == d->dataToHelperIdMap.end() )
        return;

    v3dViewAnnIntHelper * helper = d->helpers.at( it->second );
    helper->annotationModified(annData);
}

bool v3dViewAnnotationInteractor::onAddAnnotation( medAnnotationData * annItem )
{
    typedef v3dViewAnnotationInteractorPrivate::HelperVecType::size_type IndexType;
    bool isAdded = false;
    for (IndexType i(0), end(d->helpers.size()); i<end; ++i){
        v3dViewAnnIntHelper * helper = d->helpers[i];
        if ( helper->addAnnotation(annItem) ) {
            isAdded = true;
            d->dataToHelperIdMap[annItem] = i;
            break;
        }
    }
    return isAdded;
}

void v3dViewAnnotationInteractor::onRemoveAnnotation( medAnnotationData * annData )
{
    v3dViewAnnotationInteractorPrivate::HelperMap::iterator it = d->dataToHelperIdMap.find(annData);
    if ( it == d->dataToHelperIdMap.end() )
        return;

    typedef v3dViewAnnotationInteractorPrivate::HelperVecType::size_type IndexType;
    IndexType iHelper = it->second;

    d->helpers[iHelper]->removeAnnotation( annData );
    d->dataToHelperIdMap.erase(annData);
}





v3dViewAnnIntHelper::v3dViewAnnIntHelper( v3dViewAnnotationInteractor * annInt)
    : m_v3dViewAnnInt( annInt )
{

}

v3dViewAnnIntHelper::~v3dViewAnnIntHelper()
{

}


