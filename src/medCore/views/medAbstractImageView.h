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
#include <medImageViewEnum.h>

#include <medCoreExport.h>

class QVector3D;

class medAbstractData;

class medAbstractImageViewInteractor;
class medAbstractInteractor;

class medAbstractImageViewNavigator;
class medAbstractNavigator;

class medAbstractImageViewPrivate;
class MEDCORE_EXPORT medAbstractImageView: public medAbstractLayeredView
{
    Q_OBJECT

public:
    medAbstractImageView(QObject * parent = 0);
    virtual ~medAbstractImageView();

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

    medImageView::Orientation orientation();

    //! Convert from world coordinates to scene coordinates.
    virtual QPointF mapWorldToDisplayCoordinates( const QVector3D & worldVec ) = 0;
    //! Convert from scene coordinates to world coordinates.
    virtual QVector3D mapDisplayToWorldCoordinates( const QPointF & scenePoint ) = 0;
    //! Get the view center vector in world space, the center of the slice for 2d views.
    virtual QVector3D viewCenter() = 0;
    //! Get the view plane normal vector in world space.
    virtual QVector3D viewPlaneNormal() = 0;
    //! Get the view plane up vector in world space.
    virtual QVector3D viewUp() = 0;
    //! Is the scene 2D (true) or 3D (false)
    virtual bool is2D() = 0;
    //! What is the thickness of the current slice (2D)
    virtual qreal sliceThickness() = 0;
    //! The scale (number of pixels on screen per mm)
    virtual qreal scale() = 0;
    

public slots:
    void setOrientation(medImageView::Orientation orientation);
    void setDataWindowLevel (medAbstractData* data, double &window, double &level);
    void setLayerWindowLevel (unsigned int layer, double &window, double &level);
    void setWindowLevel(double window, double level);
    /**
      * Set the slice being viewed so that it contains the given spatial postion
       @position is expressed in real world coordinates.
     **/
    void moveToSliceAtPosition (const QVector3D &position);

    void moveToSlice (int slice);

    /**
       Set the camera settings of the view.
    **/
    void setCamera(const QVector3D &position,
                           const QVector3D &viewup,
                           const QVector3D &focal,
                           double parallelScale);

signals:
    void orientationChanged();
    void sliceChanged(int);
    void windowLevelChanged(double window, double level);
    void positionViewedChanged(const QVector3D &position);
    void cameraChanged(const QVector3D &position,
                       const QVector3D &viewup,
                       const QVector3D &focal,
                       double parallelScale);

protected:
    virtual medAbstractImageViewInteractor* primaryInteractor(medAbstractData* data);
    virtual QList<medAbstractInteractor*> extraInteractors(medAbstractData* data);
    virtual medAbstractImageViewInteractor* primaryInteractor(unsigned int layer);
    virtual QList<medAbstractInteractor*> extraInteractors(unsigned int layer);

    virtual medAbstractImageViewNavigator* primaryNavigator();
    virtual QList<medAbstractNavigator*> extraNavigators();

    virtual void initialiseInteractors(medAbstractData* data);
    virtual void initialiseNavigators();
    virtual void removeInteractors(medAbstractData *data);


private:
    medAbstractImageViewPrivate *d;
};


