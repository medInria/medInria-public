#ifndef _v3dViewGraphicsScene_h_
#define _v3dViewGraphicsScene_h_

#include <QtGui>

#include <medAnnotationGraphicsObject.h>
#include <medAbstractViewCoordinates.h>

#include "v3dViewPluginExport.h"

class medAbstractDataImage;
class vtkImageView2D;
class v3dView;
class v3dViewGraphicsScenePrivate;

class v3dViewGraphicsScene : public QGraphicsScene, public medAbstractViewCoordinates
{
    typedef QGraphicsScene BaseClass;

    Q_OBJECT;

public:

    v3dViewGraphicsScene( v3dView * view, QWidget * parent = NULL );
    virtual ~v3dViewGraphicsScene();

public slots:

    void onImageDataChanged( medAbstractDataImage * imageData );
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

    //! Access the view.
    medAbstractView * view();

protected:
    void showItems();
    void hideItems();
    void setItemsVisible( bool state );

    //! Override base class : do not accept drag events by default.
    void dragEnterEvent ( QGraphicsSceneDragDropEvent * event );

    // Implement medAbstractViewScene
    virtual QPointF worldToDisplay( const QVector3D & worldVec ) const;
    virtual QVector3D displayToWorld( const QPointF & sceneVec ) const;
    virtual QVector3D viewPlaneNormal() const;
    virtual QVector3D viewUp() const;
    virtual bool is2D() const;
    virtual QVector3D viewCenter() const;
    virtual qreal sliceThickness() const;
    virtual qreal scale() const;
    void sendItemChanged( medAnnotationGraphicsObject::AnnotationGraphicsItemChange change, const QVariant & value = QVariant() );

private:
    v3dViewGraphicsScenePrivate * d;
};


#endif // _v3dViewGraphicsScene_h_
