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

/**
 * @class medAbstractImageView
 * @brief Base class for image view types in medInria
 * medAbstractImageView specializes a medAbstractLayeredView.
 **/
class MEDCORE_EXPORT medAbstractImageView: public medAbstractLayeredView
{
    Q_OBJECT

public:
    medAbstractImageView(QObject * parent = 0);
    virtual ~medAbstractImageView();

    virtual void removeData(medAbstractData *data);

    medImageView::Orientation orientation();

    virtual QPointF mapWorldToDisplayCoordinates( const QVector3D & worldVec ) = 0;
    virtual QVector3D mapDisplayToWorldCoordinates( const QPointF & scenePoint ) = 0;
    virtual QVector3D viewCenter() = 0;
    virtual QVector3D viewPlaneNormal() = 0;
    virtual QVector3D viewUp() = 0;
    virtual bool is2D() = 0;
    virtual qreal sliceThickness() = 0;
    virtual qreal scale() = 0;

    virtual QWidget* toolBarWidget();

    medCompositeParameter *cameraParameter();
    medAbstractVector3DParameter *positionBeingViewedParameter();
    medCompositeParameter *windowLevelParameter(unsigned int layer);
    medDoubleParameter *opacityParameter(unsigned int layer);
    medTriggerParameter *fourViewsParameter();
    medTimeLineParameter *timeLineParameter();

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


