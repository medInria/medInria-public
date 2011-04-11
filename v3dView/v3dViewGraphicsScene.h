#ifndef _v3dViewGraphicsScene_h_
#define _v3dViewGraphicsScene_h_

#include <QtGui>

#include "v3dViewPluginExport.h"

class dtkAbstractDataImage;
class vtkImageView2D;
class v3dView;
class v3dViewGraphicsScenePrivate;

class v3dViewGraphicsScene : public QGraphicsScene
{
    typedef QGraphicsScene BaseClass;

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
    void onSliceChanged (int slice);
    /**
     * Brief description
     * Full description
     * @param     position   
     */
    void onPositionChanged (const QVector3D &position);
    /**
     * Brief description
     * Full description
     * @param     zoom  
     */
    void onZoomChanged (double zoom);
    /**
     * Brief description
     * Full description
     * @param     pan   
     */
    void onPanChanged (const QVector2D &pan);

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
                          double parallelScale);

    void onSizeChanged( int width, int height);

    void onOrientationChanged( const QString & name );

protected:
    void showItems();
    void hideItems();
    void setItemsVisible( bool state );

private:
    v3dViewGraphicsScenePrivate * d;
};


#endif // _v3dViewGraphicsScene_h_
