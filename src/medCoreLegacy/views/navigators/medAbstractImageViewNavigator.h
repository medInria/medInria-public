/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractLayeredViewNavigator.h>
#include <medImageViewEnum.h>

#include <medCoreExport.h>

class medAbstractImageView;
class medCompositeParameter;
class medAbstractVector3DParameter;
class medTimeLineParameter;

class medAbstractImageViewNavigatorPrivate;
class MEDCORE_EXPORT medAbstractImageViewNavigator : public medAbstractLayeredViewNavigator
{
    Q_OBJECT

public:
             medAbstractImageViewNavigator(medAbstractView* parent);
    virtual ~medAbstractImageViewNavigator();

public:
    virtual medImageView::Orientation orientation() const = 0;

    virtual medCompositeParameter* cameraParameter();
    virtual medAbstractVector3DParameter* positionBeingViewedParameter();
    virtual medTimeLineParameter* timeLineParameter();

public slots:

    virtual void setOrientation(medImageView::Orientation orientation) = 0;
    virtual void setCamera(QHash<QString,QVariant>) = 0;
    virtual void moveToPosition (const QVector3D &position) = 0;
    
    virtual void setCurrentTime (const double &time);

private slots:
    virtual void updateTimeLineParameter();

signals:
    void currentTimeChanged(const double &time);

private:
    medAbstractImageViewNavigatorPrivate *d;
};
