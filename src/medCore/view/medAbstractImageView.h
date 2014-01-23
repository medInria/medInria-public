/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkSmartPointer.h>

#include "medCoreExport.h"
#include "medAbstractLayeredView.h"

class medAbstractImageViewPrivate;
class medAbstractViewCoordinates;
class QColor;


class MEDCORE_EXPORT medAbstractImageView: public medAbstractLayeredView
{
    Q_OBJECT

public:
    medAbstractImageView(medAbstractView * parent = 0);
    medAbstractImageView(const medAbstractView& view);
    virtual ~medAbstractImageView();

    /**
     *  Set the view slice.
    **/
    void setSlice (int slice);

    /**
      * Set the slice being viewed so that it contains the given spatial postion
       @position is expressed in real world coordinates.
     **/
    virtual void moveToSliceAtPosition    (const QVector3D &position);
    QVector3D positionBeingViewed() const;

    /**
       Set the window/level of the view.
    **/
    virtual void setWindowLevel (double level, double window);
    void windowLevel(double &level, double &window) const;

    /**
       Set the camera settings of the view.
    **/
    virtual void setCamera(const QVector3D &position,
                           const QVector3D &viewup,
                           const QVector3D &focal,
                           double parallelScale);
    void camera(QVector3D &position,
                QVector3D &viewup,
                QVector3D &focal,
                double &parallelScale) const;

    /**
     * Set the opacity of the data on the corresponding layer
     */
    virtual void setOpacity (double opacity, int layer);

    /**
     * Get the opacity of the data on the corresponding layer
     */
    virtual double opacity(int layer) const;

    //! Convert from world coordinates to scene coordinates.
    virtual QPointF worldToDisplay( const QVector3D & worldVec ) const = 0;
    //! Convert from scene coordinates to world coordinates.
    virtual QVector3D displayToWorld( const QPointF & scenePoint ) const = 0;
    //! Get the view center vector in world space, the center of the slice for 2d views.
    virtual QVector3D viewCenter() const = 0;
    //! Get the view plane normal vector in world space.
    virtual QVector3D viewPlaneNormal() const = 0;
    //! Get the view plane up vector in world space.
    virtual QVector3D viewUp() const = 0;
    //! Is the scene 2D (true) or 3D (false)
    virtual bool is2D() const = 0;
    //! What is the thickness of the current slice (2D)
    virtual qreal sliceThickness() const = 0;
    //! The scale (number of pixels on screen per mm)
    virtual qreal scale() const = 0;
    
    
signals:
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
     * This signal is emitted when the opacity of a layer has changed.
     */
    void opacityChanged(double value, int layer);

public slots:

    /**
       Tells the view (not to) synchronize its position with other views.
     **/
    virtual void toggleLinkPosition (bool value);
    bool isPositionLinked() const;

    /**
       Tells the view (not to) synchronize its window/level with other views.
     **/
    virtual void toggleLinkWindowing (bool value);
    bool isWindowingLinked() const;

    /**
       Tells the view (not to) synchronize its camera settings with other views.
    **/
    virtual void toggleLinkCamera (bool value);
    bool isCameraLinked() const;

protected:
    virtual void moveToSliceAtPosition_impl(const QVector3D &position) = 0;
    virtual void setWindowLevel_impl(double level, double window) = 0;
    virtual void setCamera_impl(const QVector3D &position,
                           const QVector3D &viewup,
                           const QVector3D &focal,
                           double parallelScale) = 0;
    virtual void setOpacity_impl(double opacity, int layer) = 0;


protected slots:


private:
    medAbstractImageViewPrivate *d;
};


