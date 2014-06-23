/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractLayeredView.h>

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

class medDoubleParameter;
class medCompositeParameter;
class medTriggerParameter;
class medAbstractVector3DParameter;

class medAbstractImageViewPrivate;

class MEDCORE_EXPORT medAbstractImageView: public medAbstractLayeredView
{
    Q_OBJECT

public:
    medAbstractImageView(QObject * parent = 0);
    virtual ~medAbstractImageView();

    medImageView::Orientation orientation();

    /**
     * Return a composite parameter made of:
     * QVector3D &position, QVector3D &viewup, QVector3D &focal, double &parallelScale
     */
    medCompositeParameter *cameraParameter();

    medAbstractVector3DParameter *positionBeingViewedParameter();

    /**
     * Return a composite parameter made of:
     * double &window, double level
     */
    medCompositeParameter *windowLevelParameter(unsigned int layer);

    medDoubleParameter *opacityParameter(unsigned int layer);
    
    medTriggerParameter *fourViewsParameter();

    medTimeLineParameter *timeLineParameter();

    /**
     * Convert from world coordinates to scene coordinates.
     */
    virtual QPointF mapWorldToDisplayCoordinates( const QVector3D & worldVec ) = 0;

    /**
     * Convert from scene coordinates to world coordinates.
     */
    virtual QVector3D mapDisplayToWorldCoordinates( const QPointF & scenePoint ) = 0;

    /**
     * Get the view center vector in world space, the center of the slice for 2d views.
     */
    virtual QVector3D viewCenter() = 0;

    /**
     * Get the view plane normal vector in world space.
     */
    virtual QVector3D viewPlaneNormal() = 0;

    /**
     * Get the view plane up vector in world space.
     */
    virtual QVector3D viewUp() = 0;

    /**
     * Is the scene 2D (true) or 3D (false)
     */
    virtual bool is2D() = 0;

    /**
     * What is the thickness of the current slice (2D)
     */
    virtual qreal sliceThickness() = 0;

    /**
     * The scale (number of pixels on screen per mm)
     */
    virtual qreal scale() = 0;

protected:
    virtual QWidget* buildToolBarWidget();

public slots:
    void switchToFourViews();

    void setOrientation(medImageView::Orientation orientation);


signals:
    void orientationChanged();
    void currentTimeChanged(const double &time);


protected:
    virtual medAbstractImageViewInteractor* primaryInteractor(medAbstractData* data);
    virtual QList<medAbstractInteractor*> extraInteractors(medAbstractData* data);
    virtual medAbstractImageViewInteractor* primaryInteractor(unsigned int layer);
    virtual QList<medAbstractInteractor*> extraInteractors(unsigned int layer);

    virtual medAbstractImageViewNavigator* primaryNavigator();
    virtual QList<medAbstractNavigator*> extraNavigators();

    virtual bool initialiseInteractors(medAbstractData* data);
    virtual bool initialiseNavigators();
    virtual void removeInteractors(medAbstractData *data);

private:
    medAbstractImageViewPrivate *d;
};


