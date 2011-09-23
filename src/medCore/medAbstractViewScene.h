#ifndef _medAbstractViewScene_h_
#define _medAbstractViewScene_h_

#include "medCoreExport.h"

#include <QGraphicsScene>
#include <QVector2D>
#include <QVector3D>

class medAbstractView;
//class medViewEventHandler;

/**
 * Base class providing access for GraphicsScenes used in medAbstractViews.
 * \sa medAbstractView
 ***/
class MEDCORE_EXPORT medAbstractViewScene : public QGraphicsScene
{
    typedef QGraphicsScene BaseClass;
    Q_OBJECT;
public:

public:
    medAbstractViewScene( medAbstractView * view, QWidget * parent = NULL );
    virtual ~medAbstractViewScene();

    //! Convert from world coordinates to scene coordinates.
    virtual QPointF worldToScene( const QVector3D & worldVec ) const = 0;
    //! Convert from scene coordinates to world coordinates.
    virtual QVector3D sceneToWorld( const QPointF & scenePoint ) const = 0;
    //! Get the view center vector in world space, the center of the slice for 2d views.
    virtual QVector3D viewCenter() const = 0;
    //! Get the view plane normal vector in world space.
    virtual QVector3D viewPlaneNormal() const = 0;
    //! Get the view plane up vector in world space.
    virtual QVector3D viewUp() const = 0;
    //! Is the scene 2D (true) or 3D (false)
    virtual bool isScene2D() const = 0;
    //! What is the thickness of the current slice (2D)
    virtual qreal sliceThickness() const = 0;

    //! Access the view.
    medAbstractView * view() { return m_view; }

protected :

    //! Override base class : do not accept drag events by default.
    void dragEnterEvent ( QGraphicsSceneDragDropEvent * event );

private:
    medAbstractViewScene( const medAbstractViewScene &);

    medAbstractView * m_view;

    class medAbstractViewScenePrivate;
    medAbstractViewScenePrivate * d;
};

#endif // _medAbstractViewScene_h_

