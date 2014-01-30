/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractLayeredView.h>

#include <dtkCore/dtkSmartPointer.h>

#include <medAbstractImageViewInteractor.h>
#include <medAbstractImageViewNavigator.h>

#include "medCoreExport.h"

class QVector3D;

class medAbstractData;

class medAbstractImageViewInteractor;
class medAbstractExtraInteractor;

class medAbstractImageViewNavigator;
class medAbstractExtraNavigator;

class medAbstractImageViewPrivate;
class MEDCORE_EXPORT medAbstractImageView: public medAbstractLayeredView
{
    Q_OBJECT

public:
    medAbstractImageView(QObject * parent = 0);
    virtual ~medAbstractImageView();

    //TODO: find a better name than orientation - RDE.
    enum medVtkViewOrientation
    {
        VIEW_ORIENTATION_AXIAL,
        VIEW_ORIENTATION_SAGITTAL,
        VIEW_ORIENTATION_CORONAL,
        VIEW_ORIENTATION_3D,
        VIEW_ALL_ORIENTATION
    };


    unsigned int sliceAtPosition(unsigned int layer, QVector3D &position);
    QVector3D positionBeingViewed();


    void dataWindowLevel(medAbstractData* data, double &window, double &level);
    void layerWindowLevel(unsigned int layer, double &window, double &level);

    void camera(QVector3D &position,
                QVector3D &viewup,
                QVector3D &focal,
                double &parallelScale);

    /**
     * Set the opacity of the data on the corresponding layer
     */
    void setOpacity (unsigned int layer, double opacity);
    void setOpacity (medAbstractData* data, double opacity);

    /**
     * Get the opacity of the data on the corresponding layer
     */
    double opacity(unsigned int layer) ;
    double opacity(medAbstractData* data) ;

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
    

public slots:
    void setOrientation(medVtkViewOrientation orientation);
    void setDataWindowLevel (medAbstractData* data, double &window, double &level);
    void setLayerWindowLevel (unsigned int layer, double &window, double &level);
    /**
      * Set the slice being viewed so that it contains the given spatial postion
       @position is expressed in real world coordinates.
     **/
    void moveToSliceAtPosition (const QVector3D &position);

    /**
       Set the camera settings of the view.
    **/
    void setCamera(const QVector3D &position,
                           const QVector3D &viewup,
                           const QVector3D &focal,
                           double parallelScale);



protected:
    virtual medAbstractImageViewInteractor* primaryInteractor(medAbstractData* data) = 0;
    virtual QList<medAbstractExtraInteractor*> extraInteractor(medAbstractData* data) = 0;
    virtual medAbstractImageViewInteractor* primaryInteractor(unsigned int layer) = 0;
    virtual QList<medAbstractExtraInteractor*> extraInteractor(unsigned int layer) = 0;

    virtual medAbstractImageViewNavigator* primaryNavigator() = 0;
    virtual QList<medAbstractExtraNavigator*> extraNavigator() = 0;

public:

    /**
     * @brief implementationOf
     * @return Upper abstract class it derives from.
     * Do NOT reimplement it in non abstract class.
     * Used by the factory to kwnow what can be create.
     */
    static QString implementationOf()
    {
        return "medAbstractImageView";
    }


private:
    medAbstractImageViewPrivate *d;
};


