/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medCoreExport.h"

#include <dtkCore/dtkAbstractViewInteractor.h>

class QVector2D;
class QVector3D;
class medAbstractView;

class medAnnotationData;
class medAbstractViewCoordinates;
class medAttachedData;
class medAbstractData;

/**
 * Base class for view annotation interactors. These
 * are classes that interface between the annotations and the views, allowing 
 * the user to manipulate the annotations, or feedback state changes to UI items.
 * 
 ***/
class MEDCORE_EXPORT medAbstractAnnotationViewInteractor : public dtkAbstractViewInteractor
{
    typedef dtkAbstractViewInteractor BaseClass;

public:
    Q_OBJECT;

public:
    medAbstractAnnotationViewInteractor();
    virtual ~medAbstractAnnotationViewInteractor();

    //! Implement dtkAbstractObject
    virtual QString description() const;
    virtual QString identifier() const;

    //! Implement dtkAbstractViewInteractor
    virtual void setData(dtkAbstractData *data);
    virtual void setView(dtkAbstractView *view);

    //! Whether the interactor should be on when the view is in 2d and 3d mode.
    virtual bool showIn2dView() const;
    virtual bool showIn3dView() const;

    //! Return true if the annotation should be shown in the given slice.
    virtual bool isInSlice( const QVector3D & slicePoint, const QVector3D & sliceNormal, qreal thickness) const;

public slots:
    //! Respond to add / removal of attached data to data items viewed.
    virtual void onAttachedDataAdded(medAttachedData* data);
    virtual void onAttachedDataRemoved(medAttachedData* data);

    //! Called when the annotation data is altered.
    virtual void onDataModified(medAbstractData* data);

protected:
    virtual QPointF worldToScene( const QVector3D & worldVec ) const;
    virtual QVector3D sceneToWorld( const QPointF & sceneVec ) const;
    //! Get the view plane up vector in world space.
    virtual QVector3D viewUp() const;

    bool isPointInSlice( const QVector3D & testPoint, const QVector3D & slicePoint, const QVector3D & sliceNormal,  qreal thickness) const;
    bool isPointInCurrentSlice( const QVector3D & testPoint) const;

    virtual medAbstractViewCoordinates * coordinates();
    virtual const medAbstractViewCoordinates * coordinates() const;

    virtual void initialize(medAbstractView * view, medAbstractData * data); //Called when the data and the view are valid.

    void addAnnotation( medAnnotationData * annData );
    void removeAnnotation( medAnnotationData * annData );

    virtual bool onAddAnnotation( medAnnotationData * annItem );
    virtual void onRemoveAnnotation( medAnnotationData * annItem );

protected:

private:

    class Impl;
    Impl * d;
};


