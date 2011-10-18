#ifndef MEDABSTRACTANNOTATIONREPRESENTATION_H
#define MEDABSTRACTANNOTATIONREPRESENTATION_H

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
 * Base class for view annotations.
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

    virtual QString description(void) const;
    virtual QString identifier(void) const;

    // Derived classes should implement this with the annotation data they can handle.
    //    virtual QStringList handled(void) const;

    virtual void setData(dtkAbstractData *data);
    virtual void setView(dtkAbstractView *view);

    virtual bool showIn2dView() const;
    virtual bool showIn3dView() const;

    virtual bool isInSlice( const QVector3D & slicePoint, const QVector3D & sliceNormal, qreal thickness) const;

public slots:
    // Respond to add / removal of attached data to data items viewed.
    virtual void onAttachedDataAdded(medAttachedData* data);
    virtual void onAttachedDataRemoved(medAttachedData* data);

    //! Called when the annotation data is altered.
    virtual void onDataModified(medAnnotationData*data);

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

#endif // MEDABSTRACTANNOTATIONREPRESENTATION_H
