#ifndef _v3dViewGraphicsScene_h_
#define _v3dViewGraphicsScene_h_

#include <QtGui>

#include <medAbstractViewScene.h>
#include "v3dViewPluginExport.h"

class dtkAbstractDataImage;
class vtkImageView2D;
class v3dView;
class v3dViewGraphicsScenePrivate;

class v3dViewGraphicsScene : public medAbstractViewScene
{
    typedef medAbstractViewScene BaseClass;

    Q_OBJECT;

public:

    v3dViewGraphicsScene( v3dView * view, QWidget * parent = NULL );
    virtual ~v3dViewGraphicsScene();

public slots:

    void onImageDataChanged( dtkAbstractDataImage * imageData );
    /**
     * Brief description
     * Full description
     * @param     rect   
     */
    void onSceneRectChanged( const QRectF & rect );

    /**
     * Brief description
     * Full description
     * @param     slice
     */
    void onSliceChanged (int slice, bool propagate);
    /**
     * Brief description
     * Full description
     * @param     position   
     */
    void onPositionChanged (const QVector3D &position, bool propagate);
    /**
     * Brief description
     * Full description
     * @param     zoom  
     */
    void onZoomChanged (double zoom, bool propagate);
    /**
     * Brief description
     * Full description
     * @param     pan   
     */
    void onPanChanged (const QVector2D &pan, bool propagate);

    /**
     * Brief description
     * Full description
     * @param     position
     * @param     viewup
     * @param     focal
     * @param     parallelScale   
     */
    void onCameraChanged (const QVector3D &position,
                          const QVector3D &viewup,
                          const QVector3D &focal,
                          double parallelScale, bool propagate);

    void onSizeChanged( int width, int height);

    void onOrientationChanged( const QString & name );

protected:
    void showItems();
    void hideItems();
    void setItemsVisible( bool state );


    // Implement medAbstractViewScene
    //! Convert from world coordinates to scene coordinates.
    virtual QPointF worldToScene( const QVector3D & worldVec ) const;
    //! Convert from scene coordinates to world coordinates.
    virtual QVector3D sceneToWorld( const QPointF & sceneVec ) const;
    //! Get the view plane normal vector in world space.
    virtual QVector3D viewPlaneNormal() const;
    //! Get the view plane up direction
    virtual QVector3D viewUp() const;
    //! Is the scene 2D (true) or 3D (false)
    virtual bool isScene2D() const;
    virtual QVector3D sceneToImagePos(const QPointF &) const;

private:
    v3dViewGraphicsScenePrivate * d;
};


#endif // _v3dViewGraphicsScene_h_
