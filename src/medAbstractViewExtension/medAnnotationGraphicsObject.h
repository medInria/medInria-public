#ifndef MEDANNOTATIONGRAPHICSOBJECT_H
#define MEDANNOTATIONGRAPHICSOBJECT_H


#include "medAbstractViewExtensionExport.h"

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
class MEDABSTRACTVIEWEXTENSION_EXPORT medAnnotationGraphicsObject : public QGraphicsObject
{
    typedef QGraphicsObject BaseClass;

    Q_OBJECT;

public:
    medAnnotationGraphicsObject( QGraphicsItem * parent = 0 );
    virtual ~medAnnotationGraphicsObject();

    virtual void setAnnotationData( medAnnotationData * annotationData);
    medAnnotationData * annotationData() const;

    virtual bool showIn2dView() const;
    virtual bool showIn3dView() const;

    virtual bool isInSlice( const QVector3D & slicePoint, const QVector3D & sliceNormal, qreal thickness) const;
    medAbstractView * view() const;

public slots:
//    virtual void onCameraChanged();
protected:
    virtual QPointF worldToScene( const QVector3D & worldVec ) const;
    virtual QVector3D sceneToWorld( const QPointF & sceneVec ) const;
    //! Get the view plane up vector in world space.
    virtual QVector3D viewUp() const;

    bool isPointInSlice( const QVector3D & testPoint, const QVector3D & slicePoint, const QVector3D & sliceNormal,  qreal thickness) const;

    QList<medAbstractView *> views () const;

private:

    class medAbstractViewAnnotationPrivate;
    medAbstractViewAnnotationPrivate * d;
};

#endif // MEDANNOTATIONGRAPHICSOBJECT_H

