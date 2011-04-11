#ifndef _v3dViewCircleAnnotation_h_
#define _v3dViewCircleAnnotation_h_

#include "v3dViewPluginExport.h"

#include <QGraphicsObject>

class v3dView;
class v3dViewCircleAnnotationPrivate;

class v3dViewCircleAnnotation : public QGraphicsObject
{
    typedef QGraphicsObject BaseClass;

    Q_OBJECT;

public:
    v3dViewCircleAnnotation( v3dView       * view,
                             QGraphicsItem * parent = 0 );
    virtual ~v3dViewCircleAnnotation();

    void setCenter( const QVector3D & center );
    void setRadius( qreal r );

    void setSlice( QString orientation, int slice );

    QRectF boundingRect() const;

    void paint( QPainter * painter,
                const QStyleOptionGraphicsItem * option,
                QWidget * widget);

    void updateSceneCoords();

private:
    v3dViewCircleAnnotationPrivate * d;
};


#endif // _v3dViewCircleAnnotation_h_
