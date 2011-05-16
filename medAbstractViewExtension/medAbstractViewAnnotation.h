#ifndef _medAbstractViewAnnotation_h_
#define _medAbstractViewAnnotation_h_

#include "medViewWidgetsExport.h"

#include <QGraphicsObject>

class QVector2D;
class QVector3D;
class medAbstractView;


/**
 * Base class for view annotations.
 * 
 * This is abstract : It does not implement the QGraphicsItem::paint method.
 * 
 ***/
class MEDABSTRACTVIEWEXTENSION_EXPORT medAbstractViewAnnotation : public QGraphicsObject
{
    typedef QGraphicsObject BaseClass;

    Q_OBJECT;

public:
    medAbstractViewAnnotation( medAbstractView * view, QGraphicsItem * parent = 0 );
    virtual ~medAbstractViewAnnotation();

    virtual bool showIn2dView() const;
    virtual bool showIn3dView() const;

    virtual bool isInSlice( const QVector3D & slicePoint, const QVector3D & sliceNormal, qreal thickness) const;

public slots:
//    virtual void onCameraChanged();
protected:
    virtual QPointF worldToScene( const QVector3D & worldVec ) const;
    virtual QVector3D sceneToWorld( const QPointF & sceneVec ) const;
    //! Get the view plane up vector in world space.
    virtual QVector3D viewUp() const;

    bool isPointInSlice( const QVector3D & testPoint, const QVector3D & slicePoint, const QVector3D & sliceNormal,  qreal thickness) const;

    medAbstractView * view() { return m_view; }
private:

    medAbstractView * m_view;
    class medAbstractViewAnnotationPrivate;
    medAbstractViewAnnotationPrivate * d;
};


#endif // _v3dViewCircleAnnotation_h_
