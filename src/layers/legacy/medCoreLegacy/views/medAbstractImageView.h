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

#include <medCoreLegacyExport.h>

class QVector3D;

class medAbstractData;

class medAbstractImageViewInteractor;
class medAbstractInteractor;

class medAbstractImageViewNavigator;
class medAbstractNavigator;

class medDoubleParameterL;
class medCompositeParameterL;
class medTriggerParameterL;
class medAbstractVector3DParameterL;

class medAbstractImageViewPrivate;

/**
 * @class medAbstractImageView
 * @brief Base class for image view types in medInria
 * medAbstractImageView specializes a medAbstractLayeredView.
 **/
class MEDCORELEGACY_EXPORT medAbstractImageView: public medAbstractLayeredView
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

    medCompositeParameterL *cameraParameter();
    medAbstractVector3DParameterL *positionBeingViewedParameter();
    medCompositeParameterL *windowLevelParameter(unsigned int layer);
    medDoubleParameterL *opacityParameter(unsigned int layer);
    medTriggerParameterL *fourViewsParameter();
    medTimeLineParameterL *timeLineParameter();

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


