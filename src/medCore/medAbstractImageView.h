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
    void setSlice       (int slice);

    /**
      * Set the slice being viewed so that it contains the given spatial postion
       @position is expressed in real world coordinates.
     **/
    virtual void setToSliceAtPosition    (const QVector3D &position);
    QVector3D positionBeingViewed() const;

    /**
       Set the window/level of the view.
    **/
    virtual void setWindowLevel (double level, double window);
    void windowLevel(double &level, double &window) const;

    /**
       Set the camera settings of the view.
    **/
    virtual void setCamera   (const QVector3D &position, const QVector3D &viewup, const QVector3D &focal, double parallelScale);
    void camera(QVector3D &position, QVector3D &viewup, QVector3D &focal, double &parallelScale) const;


    /**
     * Set the opacity of the data on the corresponding layer
     */
    virtual void setOpacity (double opacity, int layer);

    /**
     * Get the opacity of the data on the corresponding layer
     */
    virtual double opacity(int layer) const;


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
    virtual void setLinkPosition (bool value);
    bool positionLinked() const;

    /**
       Tells the view (not to) synchronize its window/level with other views.
     **/
    virtual void setLinkWindowing (bool value);
    bool windowingLinked() const;

    /**
       Tells the view (not to) synchronize its camera settings with other views.
    **/
    virtual void setLinkCamera (bool value);
    bool cameraLinked() const;

private:
    medAbstractImageViewPrivate *d;
};


