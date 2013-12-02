/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medAbstractAnnotationViewInteractor.h"

#include <medAbstractView.h>

#include "medAnnotationData.h"
#include <medAbstractViewCoordinates.h>

#include <dtkCore/dtkSmartPointer.h>
#include <dtkLog/dtkLog.h>

#include <QDebug>
#include <QVector2D>
#include <QVector3D>


class medAbstractAnnotationViewInteractor::Impl
{
public:
    // Which annotations are installed for which data.
    typedef QSet< dtkSmartPointer<medAnnotationData> > AnnotationHash;
    AnnotationHash installedAnnotations;
};

medAbstractAnnotationViewInteractor::medAbstractAnnotationViewInteractor()
    :    d( new Impl )
{
}

medAbstractAnnotationViewInteractor::~medAbstractAnnotationViewInteractor()
{
    delete d;
    d = NULL;
}

void medAbstractAnnotationViewInteractor::setData(dtkAbstractData *data)
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

void medAbstractAnnotationViewInteractor::setView( dtkAbstractView *view )
{
    BaseClass::setView(view);

    medAnnotationData * annotationData = qobject_cast<medAnnotationData *>(this->data());
    medAbstractView * mview = qobject_cast< medAbstractView *>(this->view());
    if ( mview && annotationData ) {
        this->initialize(mview,annotationData);
    }
}

void medAbstractAnnotationViewInteractor::onDataModified( medAbstractData*data )
{

}


bool medAbstractAnnotationViewInteractor::isPointInSlice( const QVector3D & testPoint, const QVector3D & slicePoint, const QVector3D & sliceNormal, qreal thickness ) const
{
    qreal distanceToPlane = QVector3D::dotProduct( (testPoint - slicePoint), sliceNormal );

    distanceToPlane = distanceToPlane / sliceNormal.length();

    return ( distanceToPlane >= -thickness ) && ( distanceToPlane < thickness );
}

bool medAbstractAnnotationViewInteractor::isPointInCurrentSlice( const QVector3D & testPoint ) const
{
    const medAbstractViewCoordinates * coords = this->coordinates();
    return this->isPointInSlice( testPoint, coords->viewCenter(), coords->viewPlaneNormal(), 0.5*coords->sliceThickness() );
}

QPointF medAbstractAnnotationViewInteractor::worldToScene( const QVector3D & worldVec ) const
{
    return this->coordinates()->worldToDisplay( worldVec );
}

QVector3D medAbstractAnnotationViewInteractor::sceneToWorld( const QPointF & sceneVec ) const
{
    return this->coordinates()->displayToWorld( sceneVec );
}

QVector3D medAbstractAnnotationViewInteractor::viewUp() const
{
    return this->coordinates()->viewUp( );
}

bool medAbstractAnnotationViewInteractor::isInSlice( const QVector3D & slicePoint, const QVector3D & sliceNormal, qreal thickness ) const
{
    return true;
}

bool medAbstractAnnotationViewInteractor::showIn2dView() const
{
    return true;
}

bool medAbstractAnnotationViewInteractor::showIn3dView() const
{
    return true;
}

medAbstractViewCoordinates * medAbstractAnnotationViewInteractor::coordinates()
{
    return qobject_cast<medAbstractView *>(this->view())->coordinates();
}

const medAbstractViewCoordinates * medAbstractAnnotationViewInteractor::coordinates() const
{
    return qobject_cast<medAbstractView *>(
        const_cast<medAbstractAnnotationViewInteractor*>(this)->view()
        )->coordinates();
}



QString medAbstractAnnotationViewInteractor::description( void ) const
{
    return QString("medAbstractAnnotationRepresentation");
}

QString medAbstractAnnotationViewInteractor::identifier( void ) const
{
    return QString("medAbstractAnnotationRepresentation");
}

void medAbstractAnnotationViewInteractor::initialize( medAbstractView * view, medAbstractData * data )
{

}

void medAbstractAnnotationViewInteractor::onAttachedDataAdded( medAttachedData* data )
{
    medAnnotationData * annData = qobject_cast<medAnnotationData *>(data);
    if ( annData ) {
        this->addAnnotation(annData);
    }
}

void medAbstractAnnotationViewInteractor::onAttachedDataRemoved( medAttachedData* data )
{
    medAnnotationData * annData = qobject_cast<medAnnotationData *>(data);
    if ( annData ) {
        this->removeAnnotation(annData);
    }
}

void medAbstractAnnotationViewInteractor::addAnnotation( medAnnotationData * annData )
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

void medAbstractAnnotationViewInteractor::removeAnnotation( medAnnotationData * annData )
{

}

bool medAbstractAnnotationViewInteractor::onAddAnnotation( medAnnotationData *  annItem )
{
    return false;
}

void medAbstractAnnotationViewInteractor::onRemoveAnnotation( medAnnotationData *  annItem )
{
}
