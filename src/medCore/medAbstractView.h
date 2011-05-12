/* medAbstractView.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu Oct 21 19:54:36 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Oct 26 10:46:04 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 9
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDABSTRACTVIEW_H
#define MEDABSTRACTVIEW_H

#include <dtkCore/dtkAbstractView.h>

#include <QSharedPointer>
#include "medCoreExport.h"

class medAbstractViewPrivate;

/**
 * @class medAbstractView
 * @brief Base class for view types in medinria
 * medAbstractView specializes a dtkAbstractView in the context of medinria.
 * Two concepts are added: view synchronization and color lookup.
 *
 * View synchronization is achieved via the Qt signal/slot mecanism. 5 signals
 * were added: positionChanged(), zoomChanged(), panChanged(), windowingChanged(),
 * cameraChanged() with the corresponding slots: setPosition(), setZoom(),
 * setPan(), setWindowLevel() and setCamera(). Any subclass of medAbstractView
 * should implement those slots and emit signals when apropriate. One can activate/
 * deactivate synchronization using the corresponding setLinkPosition(),
 * setLinkWindowing(), setLinkPan(), setLinkZoom() and setLinkCamera() slots. This
 * class should be used in conjunction with the class @medViewPool, which realizes
 * the synchronization.
 *
 * Color lookup tables are specified with a QList of scalars and corresponding
 * colors using the setColorLookupTable() or setTransferFunctions() methods.
 **/

class MEDCORE_EXPORT medAbstractView: public dtkAbstractView
{
    Q_OBJECT

public:
    medAbstractView(medAbstractView * parent = 0);
    medAbstractView(const medAbstractView& view);
    virtual ~medAbstractView(void);

    virtual void setColorLookupTable(int min_range,
                                     int max_range,
                                     int size,
                                     const int & table);

    virtual void setColorLookupTable( QList<double> scalars,
                      QList<QColor> colors );
    virtual void setTransferFunctions( QList<double> scalars,
                       QList<QColor> colors );
    virtual void getTransferFunctions( QList<double> & scalars,
                       QList<QColor> & colors );

    virtual QWidget *receiverWidget(void);

    /**
       Set the view slice.
    **/
    void setSlice       (int slice);

    /**
       Set the view position, i.e., focus on a particular spatial coordinate.
       @position is expressed in real world coordinates.
     **/
    void setPosition    (const QVector3D &position);
    QVector3D position(void) const;

    /**
       Set the view zoom factor.
    **/
    void setZoom        (double zoom);
    double zoom(void) const;

    /**
       Set the view pan.
    **/
    void setPan         (const QVector2D &pan);
    QVector2D pan(void) const;

    /**
       Set the window/level of the view.
    **/
    void setWindowLevel (double level, double window);
    void windowLevel(double &level, double &window) const;

    /**
       Set the camera settings of the view.
    **/
    void setCamera   (const QVector3D &position, const QVector3D &viewup, const QVector3D &focal, double parallelScale);
    void camera(QVector3D &position, QVector3D &viewup, QVector3D &focal, double &parallelScale) const;
    
    /**
     * Set the visibility of the data on the corresponding layer
     */
    void setVisibility (bool visibility, int layer);

    /**
     * Get the visibility of the data on the corresponding layer
     */
    virtual bool visibility(int layer) const;

    /**
     * Set the opacity of the data on the corresponding layer
     */
    void setOpacity (double opacity, int layer);

    /**
     * Get the opacity of the data on the corresponding layer
     */
    virtual double opacity(int layer) const;

    /**
     * Set the current layer. The current layer is used to determine which layer will receive
     * property changed.
     */
    void setCurrentLayer(int layer);
  
    /**
     * Get the current layer. The current layer is used to determine which layer will receive
     * property changed.
     */
    virtual int currentLayer(void) const;
    
    /**
     * Get the number of layers of the view.
     */    
    virtual int layerCount(void) const;

    /**
     * Remove an overlay.
     */
    virtual void removeOverlay(int layer);

    /**
     * Setting data using a qSharedPointer
     */
    virtual void setSharedDataPointer(QSharedPointer<dtkAbstractData> data);

signals:
    /**
       This signal is emitted when a view is about to close.
     **/
    void closing       (void);

    /**
       In medinria, the daddy is the reference view (contoured in red). Only one
       per pool is authorized. Emit this signal when the view wants to become the
       daddy.
     **/
    void becomeDaddy   (bool);

    /**
       This signal is emitted when a view wants to register its data to the daddy.
       A registration plugin must be available. Registration is handled in the
       @medViewPool.
     **/
    void reg           (bool);

    /**
       This signal is emitted when the color lookup table has changed.
     **/
    void lutChanged    (void);

    /**
       This signal is emitted when the view wants to be displayed in full screen.
     **/
    void fullScreen    (bool);

    /**
       This signal is emitted when the shown slice of the view has
       changed.  A changed slice always comes along with a changed
       position, but the positionChanged signal is sent before the new
       slice number is computed in vtkImageView2D.
     **/
    void sliceChanged     (int slice, bool propagate);

    /**
       This signal is emitted when the current position pointed by the view has changed.
       This is the case, for instance, when the slice of a 3D image was changed, or when
       the user cliked on a specific voxel.
       The position is expressed in physical coordinates.
     **/
    void positionChanged  (const QVector3D &position, bool propagate);

    /**
       This signal is emitted when the zoom factor of the view has changed.
     **/
    void zoomChanged      (double zoom, bool propagate);

    /**
       This signal is emitted when the pan (=translation) of the view has
       changed.
     **/
    void panChanged       (const QVector2D &pan, bool propagate);

    /**
       This signal is emitted when the windowing (window/level controlling the image
       contrast) has changed.
     **/
    void windowingChanged (double level, double window, bool propagate);

    /**
       This signal is emitted when the camera of the view has changed. The camera settings
       are expressed in 4 parameters:
       @position: the physical position of the camera
       @viewup: the view up direction
       @focal: the focal position of the camera
       @parallelScale: the scaling factor in parallel projection
    **/
    void cameraChanged    (const QVector3D &position,
                           const QVector3D &viewup,
                           const QVector3D &focal,
                           double parallelScale,
                           bool propagate);
    
    /**
     * This signal is emitted when the visibility of a layer has changed.
     */
    void visibilityChanged(bool visibility, int layer);
    
    /**
     * This signal is emitted when the opacity of a layer has changed.
     */
    void opacityChanged(double value, int layer);

    /**
     *  This signal is emitted when the user adds a data to the view
     */
    void dataAdded (int layer);

    void dataAdded (dtkAbstractData* data);

public slots:
    /**
       Tells the view (not to) synchronize its position with other views.
     **/
    virtual void setLinkPosition (bool value);
    bool positionLinked (void) const;

    /**
       Tells the view (not to) synchronize its window/level with other views.
     **/
    virtual void setLinkWindowing (bool value);
    bool windowingLinked (void) const;

    /**
       Tells the view (not to) synchronize its camera settings with other views.
    **/
    virtual void setLinkCamera (bool value);
    bool cameraLinked (void) const;

    virtual void onSliceChanged     (int slice);
    virtual void onPositionChanged  (const QVector3D &position);
    virtual void onZoomChanged      (double zoom);
    virtual void onPanChanged       (const QVector2D &pan);
    virtual void onWindowingChanged (double level, double window);
    virtual void onCameraChanged    (const QVector3D &position,
                     const QVector3D &viewup,
                     const QVector3D &focal,
                     double parallelScale);
    
    virtual void onVisibilityChanged(bool visible, int layer);
    
    virtual void onOpacityChanged(double opacity, int layer);

    
protected:
    void emitViewSliceChangedEvent    (int slice);
    void emitViewPositionChangedEvent (const QVector3D &position);
    void emitViewZoomChangedEvent     (double zoom);
    void emitViewPanChangedEvent      (const QVector2D &pan);
    void emitViewWindowingChangedEvent(double level, double window);
    void emitViewCameraChangedEvent   (const QVector3D &position,
                                       const QVector3D &viewup,
                                       const QVector3D &focal,
                                       double parallelScale);

private:
    medAbstractViewPrivate *d;
};

#endif
