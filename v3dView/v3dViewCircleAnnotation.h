#ifndef _v3dViewCircleAnnotation_h_
#define _v3dViewCircleAnnotation_h_

#include "v3dViewPluginExport.h"

#include "medAnnotationData.h"
#include "medAnnotationGraphicsObject.h"

class v3dView;
class v3dViewCircleAnnotationPrivate;
class v3dViewCircleAnnotation;

class v3dViewCircleAnnotationData : public medAnnotationData
{
    Q_OBJECT;
public:
    v3dViewCircleAnnotationData();
    ~v3dViewCircleAnnotationData();

    void setCenter( const QVector3D & center );
    void setRadius( qreal r );

    void setSlice( QString orientation, int slice );

private:
friend class v3dViewCircleAnnotation;
    qreal     radiusWorld;
    QVector3D centerWorld;

    QString   orientation;
    int       slice;
    QColor    color;

};

class v3dViewCircleAnnotation : public medAnnotationGraphicsObject
{
    typedef medAnnotationGraphicsObject BaseClass;

    Q_OBJECT;

public:
    v3dViewCircleAnnotation( v3dViewCircleAnnotationData * data, QGraphicsItem * parent = 0 );
    virtual ~v3dViewCircleAnnotation();


    QRectF boundingRect() const;

    void paint( QPainter * painter,
                const QStyleOptionGraphicsItem * option,
                QWidget * widget);

    void updateSceneCoords();

private:
    v3dViewCircleAnnotationPrivate * d;
};


#endif // _v3dViewCircleAnnotation_h_
