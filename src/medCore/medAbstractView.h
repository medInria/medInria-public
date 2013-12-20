/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkSmartPointer.h>

#include "medCoreExport.h"

class medAbstractData;
class medAbstractViewPrivate;

class medAbstractViewCoordinates;
class medViewBackend;

class QColor;

/**
 * @class medAbstractView
 * @brief Base class for view types in medInria
 * medAbstractView specializes a dtkAbstractView in the context of medInria.
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
    virtual ~medAbstractView();

<<<<<<< HEAD
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

    virtual QWidget *receiverWidget();

    /**
       Set the view slice.
    **/
    void setSlice       (int slice);

    /**
       Set the view position, i.e., focus on a particular spatial coordinate.
       @position is expressed in real world coordinates.
     **/
    void setPosition    (const QVector3D &position);
    QVector3D position() const;

    /**
=======
    //TODO rename?
    virtual QWidget *receiverWidget();

    /**
>>>>>>> medAbstractView refactoring
       Set the view zoom factor.
    **/
    void setZoom        (double zoom);
    double zoom() const;

    /**
       Set the view pan.
    **/
    void setPan         (const QVector2D &pan);
    QVector2D pan() const;

<<<<<<< HEAD
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
     * Get the current layer. The current layer is used to determine which layer will receive
     * property changed.
     */
    virtual int currentLayer() const;

    /**
     * Get the flag that indicates if the view has an image loaded
     */
    virtual bool hasImage() const;

    /**
     * Get the number of layers of the view.
     */
    virtual int layerCount() const;
=======

    //! Get the coordinates helper
    // TODO: check if this is required, and what is exactly required
    virtual medAbstractViewCoordinates * coordinates() = 0;

    virtual medViewBackend * backend() const;


    //TODO GPR: check datatypes: used in medToolbox::update
    void addDataType(const QString & dataDescription);
>>>>>>> medAbstractView refactoring

    /**
     * Remove an overlay.
     */
    virtual void removeOverlay(int layer);

    /**
     * Setting data using a dtkSmartPointer
     */
    virtual void setSharedDataPointer(dtkSmartPointer<medAbstractData> data);
    virtual void setSharedDataPointer( dtkSmartPointer<medAbstractData> data,
                                                int layer);

    void setCurrentMeshLayer(int meshLayer);
    virtual int currentMeshLayer() const;
    void setMeshLayerCount(int meshLayerCount);
    virtual int meshLayerCount() const;

    bool isInList(medAbstractData * data, int layer);

    /**
     * Return true if the data is already contained in the data list
     */
    bool isInList(medAbstractData * data);
    void addDataInList(medAbstractData * data, int layer);
    void addDataInList(medAbstractData * data);
    medAbstractData* dataInList(int layer) const;
    void setDataInList(medAbstractData * data, int layer);

    void addDataType(const QString & dataDescription);
    void removeDataType(const QString & dataDescription);
    QHash<QString, unsigned int> dataTypes();

<<<<<<< HEAD
    /** The color used to represent the extent or space of this view in another view */
    virtual QColor color() const;
    virtual void setColor( const QColor & color);

    virtual QString getLUT(int layer) const;
    virtual QString getPreset(int layer) const;

    //! Get the coordinates helper
    virtual medAbstractViewCoordinates * coordinates() = 0;

    virtual medViewBackend * backend() const;

=======
>>>>>>> medAbstractView refactoring
signals:
    void selected();
    void unselected();

    /**
       This signal is emitted when a view is about to close.
     **/
    void closing();

    /**
       This signal is emitted when the color lookup table has changed.
     **/
    void lutChanged();

    /**
       This signal is emitted when the view wants to be displayed in full screen.
     **/
    //TODO: à vérifier mais sans doute à bouger dans les container: rename to maximizeRequested
    void fullScreen    (bool);

    /**
       This signal is emitted when the zoom factor of the view has changed.
     **/
    void zoomChanged      (double zoom, bool propagate);

    /**
       This signal is emitted when the pan (=translation) of the view has
       changed.
     **/
    void panChanged       (const QVector2D &pan, bool propagate);

<<<<<<< HEAD
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

    void dataAdded (medAbstractData* data);

    void dataAdded (medAbstractData* data, int layer);
    void dataRemoved (int layer);
    void dataRemoved(medAbstractData* data,int layer);

    void TwoDTriggered(dtkAbstractView* d);
    void ThreeDTriggered(dtkAbstractView* d);

    /** Emitted when the oblique view settings change */
    void obliqueSettingsChanged (const medAbstractView *self);

    void colorChanged();
=======
>>>>>>> medAbstractView refactoring

public slots:

<<<<<<< HEAD
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

    /** When another linked view changes it's oblique settings the pool calls this:*/
    virtual void onObliqueSettingsChanged(const medAbstractView * vsender);

    /** Called when another view leaves the pool */
    virtual void onAppendViewToPool( medAbstractView * viewAppended );

    /** Called when another view leaves the pool */
    virtual void onRemoveViewFromPool( medAbstractView * viewRemoved );


    void setFullScreen( bool state );
    /**
     * Set the current layer. The current layer is used to determine which layer will receive
     * property changed.
     */
    virtual void setCurrentLayer(int layer);

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
    void emitObliqueSettingsChangedEvent();

    // Emitted whenever the plane color changes.
    void emitColorChangedEvent();
=======
    //TODO: rename to toggleMaximize ?
    void setFullScreen( bool state );

>>>>>>> medAbstractView refactoring

private:
    medAbstractViewPrivate *d;
};

// derive and implement if you need to provide access to your backend
class medViewBackend {
};
