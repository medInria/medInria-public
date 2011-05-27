#ifndef _medAbstractViewScene_h_
#define _medAbstractViewScene_h_

#include "medViewWidgetsExport.h"

#include <QGraphicsScene>
#include <QVector2D>
#include <QVector3D>

class medAbstractView;
class medAbstractViewAnnotation;
class medViewEventHandler;

/**
 * Base class providing access to objects in the view's graphics scene.
 ***/
class MEDABSTRACTVIEWEXTENSION_EXPORT medAbstractViewScene : public QGraphicsScene
{
    typedef QGraphicsScene BaseClass;
    Q_OBJECT;

public:
    medAbstractViewScene( medAbstractView * view, QWidget * parent = NULL );
    virtual ~medAbstractViewScene();

    //! Convert from world coordinates to scene coordinates.
    virtual QPointF worldToScene( const QVector3D & worldVec ) const = 0;
    //! Convert from scene coordinates to world coordinates.
    virtual QVector3D sceneToWorld( const QPointF & scenePoint ) const = 0;
    //! Get the view plane normal vector in world space.
    virtual QVector3D viewPlaneNormal() const = 0;
    //! Get the view plane up vector in world space.
    virtual QVector3D viewUp() const = 0;
    //! Is the scene 2D (true) or 3D (false)
    virtual bool isScene2D() const = 0;
    //! Get the image position for the given scene position. Valid only for 2D views.
    virtual QVector3D sceneToImagePos(const QPointF & scenePoint) const = 0;


    virtual void addAnnotation( medAbstractViewAnnotation * annotation);
    virtual void removeAnnotation( medAbstractViewAnnotation * annotation);

    //! Access the view.
    medAbstractView * view() { return m_view; }

protected :

private:
    medAbstractViewScene( const medAbstractViewScene &);

    medAbstractView * m_view;

    class medAbstractViewScenePrivate;
    medAbstractViewScenePrivate * d;
};

#endif // _medAbstractViewScene_h_

