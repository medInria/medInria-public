#ifndef MEDABSTRACTANNOTATIONREPRESENTATION_H
#define MEDABSTRACTANNOTATIONREPRESENTATION_H




#include "medCoreExport.h"

#include <QGraphicsObject>

class QVector2D;
class QVector3D;
class medAbstractView;

class medAnnotationData;

/**
 * Base class for view annotations.
 * 
 * This is abstract : It does not implement the QGraphicsItem::paint method.
 * 
 ***/
class MEDCORE_EXPORT medAbstractAnnotationRepresentation : public dtkAbstractData
{
    typedef dtkAbstractData BaseClass;

    Q_OBJECT;
public:

public:
    medAbstractAnnotationRepresentation( dtkAbstractData * parent = 0 );
    virtual ~medAbstractAnnotationRepresentation();

    virtual void setAnnotationData( const medAnnotationData * annotationData);
    const medAnnotationData * annotationData() const;

    medAbstractView * view() const;

protected slots:
    //! Called when the annotation data is altered.
    virtual void onDataModified();

protected:

private:

    class Impl;
    Impl * d;
};


#endif // MEDABSTRACTANNOTATIONREPRESENTATION_H
