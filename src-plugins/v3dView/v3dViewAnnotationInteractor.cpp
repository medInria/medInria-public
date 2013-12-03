/*=========================================================================

 medInria

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

#include "medVtkView.h"

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

    typedef QSet< dtkSmartPointer<medAnnotationData> > AnnotationHash;
    AnnotationHash installedAnnotations;
};

// Implementation
v3dViewAnnotationInteractor::v3dViewAnnotationInteractor(): medAbstractVtkViewInteractor(), d(new v3dViewAnnotationInteractorPrivate)
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
    return QStringList () << v3dView::s_identifier() << medVtkView::s_identifier();
}

bool v3dViewAnnotationInteractor::isDataTypeHandled(QString dataType) const
{
    return true;
}

bool v3dViewAnnotationInteractor::registered()
{
    return dtkAbstractViewFactory::instance()->registerViewInteractorType(
                                                                          v3dViewAnnotationInteractor::s_identifier(),
                                                                          QStringList () << v3dView::s_identifier() << medVtkView::s_identifier(),
                                                                          createV3dViewAnnotationInteractor);
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

void v3dViewAnnotationInteractor::setData(dtkAbstractData *data)
{
    if ( this->data() ) {
        disconnect(this->data(), SIGNAL(dataModified(medAbstractData*)), this, SLOT(onDataModified(medAbstractData*)) );
        // Remove annotations
        foreach( dtkSmartPointer<medAnnotationData> key,  d->installedAnnotations ) {
            if ( key->parentData() == data ) {
                this->removeAnnotation( key );
            }
        }
    }
    
    medAbstractData * mdata = qobject_cast<medAbstractData *>(data);
    if (mdata)
    {
        connect( mdata, SIGNAL(attachedDataAdded(medAttachedData*)), this,  SLOT(onAttachedDataAdded(medAttachedData*)) );
        connect( mdata, SIGNAL(attachedDataRemoved(medAttachedData*)), this,  SLOT(onAttachedDataRemoved(medAttachedData*)) );
        
        // Add any existing annotations on the data to our list.
        QList<medAttachedData*> attached = mdata->attachedData();
        foreach( medAttachedData* item,  attached ) {
            medAnnotationData *annItem = qobject_cast<medAnnotationData*>(item);
            if (annItem) {
                this->onAttachedDataAdded(annItem);
            }
        }
        
        medAbstractView * mview = qobject_cast< medAbstractView *>(this->view());
        if ( mview && mdata ) {
            this->initialize(mview,mdata);
        }
    } // if mdata
}

void v3dViewAnnotationInteractor::setView(dtkAbstractView* view)
{
    BaseClass::setView(view);
    
    medAnnotationData * annotationData = qobject_cast<medAnnotationData *>(this->data());
    medAbstractView * mview = qobject_cast< medAbstractView *>(this->view());
    
    if ( mview && annotationData )
    {
        this->initialize(mview,annotationData);
    }

    if (view)
        setData((dtkAbstractData*)view->data());
    else
        setData(NULL);
}

bool v3dViewAnnotationInteractor::showIn2dView() const
{
    return true;
}

bool v3dViewAnnotationInteractor::showIn3dView() const
{
    return true;
}

bool v3dViewAnnotationInteractor::isInSlice( const QVector3D & slicePoint, const QVector3D & sliceNormal,
                                            qreal thickness ) const
{
    return true;
}

void v3dViewAnnotationInteractor::initialize( medAbstractView * view, medAbstractData* data )
{
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

void v3dViewAnnotationInteractor::addAnnotation( medAnnotationData * annData )
{
    if ( d->installedAnnotations.contains(annData) ) {
        return;
    }
    
    bool isAdded = this->onAddAnnotation(annData);
    
    if ( isAdded ) {
        d->installedAnnotations.insert( annData );
        
        connect(annData, SIGNAL(dataModified(medAbstractData*)), this, SLOT(onDataModified(medAbstractData*)) );
    }
}

void v3dViewAnnotationInteractor::removeAnnotation( medAnnotationData * annData )
{
    
}

void v3dViewAnnotationInteractor::onAttachedDataAdded( medAttachedData* data )
{
    medAnnotationData * annData = qobject_cast<medAnnotationData *>(data);
    if ( annData ) {
        this->addAnnotation(annData);
    }
}

void v3dViewAnnotationInteractor::onAttachedDataRemoved( medAttachedData* data )
{
    medAnnotationData * annData = qobject_cast<medAnnotationData *>(data);
    if ( annData ) {
        this->removeAnnotation(annData);
    }
}

QPointF v3dViewAnnotationInteractor::worldToScene( const QVector3D & worldVec ) const
{
    return this->coordinates()->worldToDisplay( worldVec );
}

QVector3D v3dViewAnnotationInteractor::sceneToWorld( const QPointF & sceneVec ) const
{
    return this->coordinates()->displayToWorld( sceneVec );
}

QVector3D v3dViewAnnotationInteractor::viewUp() const
{
    return this->coordinates()->viewUp( );
}

bool v3dViewAnnotationInteractor::isPointInSlice(const QVector3D & testPoint, const QVector3D & slicePoint,
                                                 const QVector3D & sliceNormal, qreal thickness ) const
{
    qreal distanceToPlane = QVector3D::dotProduct( (testPoint - slicePoint), sliceNormal );
    
    distanceToPlane = distanceToPlane / sliceNormal.length();
    
    return ( distanceToPlane >= -thickness ) && ( distanceToPlane < thickness );
}

bool v3dViewAnnotationInteractor::isPointInCurrentSlice( const QVector3D & testPoint ) const
{
    const medAbstractViewCoordinates * coords = this->coordinates();
    return this->isPointInSlice(testPoint, coords->viewCenter(), coords->viewPlaneNormal(),
                                0.5*coords->sliceThickness());
}

void v3dViewAnnotationInteractor::setOpacity(dtkAbstractData * /*data*/, double /*opacity*/)
{
    //TODO
}

double v3dViewAnnotationInteractor::opacity(dtkAbstractData * /*data*/) const
{
    //TODO
    return 100;
}

void v3dViewAnnotationInteractor::setVisible(dtkAbstractData * /*data*/, bool /*visible*/)
{
    //TODO
}

bool v3dViewAnnotationInteractor::isVisible(dtkAbstractData * /*data*/) const
{
    //TODO
    return true;
}


medAbstractViewCoordinates * v3dViewAnnotationInteractor::coordinates()
{
    return qobject_cast<medAbstractView *>(this->view())->coordinates();
}

const medAbstractViewCoordinates * v3dViewAnnotationInteractor::coordinates() const
{
    return qobject_cast<medAbstractView *>(const_cast<v3dViewAnnotationInteractor*>(this)->view())->coordinates();
}

v3dViewAnnIntHelper::v3dViewAnnIntHelper( v3dViewAnnotationInteractor * annInt)
    : m_v3dViewAnnInt( annInt )
{

}

v3dViewAnnIntHelper::~v3dViewAnnIntHelper()
{

}


