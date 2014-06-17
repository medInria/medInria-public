/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractImageViewNavigator.h>

#include <medAbstractImageView.h>

#include <medCompositeParameter.h>
#include <medVector3DParameter.h>
#include <medDoubleParameter.h>


class medAbstractImageViewNavigatorPrivate
{
public:
    medAbstractImageView *view;
    medCompositeParameter *cameraParameter;
    medVector3DParameter *positionBeingViewedParameter;
    medDoubleParameter *timeParameter;

};

medAbstractImageViewNavigator::medAbstractImageViewNavigator(medAbstractView *parent):
    medAbstractLayeredViewNavigator(parent), d(new medAbstractImageViewNavigatorPrivate)
{
    d->positionBeingViewedParameter = NULL;
    d->cameraParameter = NULL;
    d->timeParameter = NULL;

    d->view = dynamic_cast<medAbstractImageView *>(parent);
    if(!d->view)
    {
        qWarning() << "Derived type of medAbstractImageViewNavigator should always be parented \
                      with derived type of medAbstractImageView.";
        return;
    }

    connect(this, SIGNAL(currentTimeChanged(double)), d->view, SIGNAL(currentTimeChanged(double)));
}

medAbstractImageViewNavigator::~medAbstractImageViewNavigator()
{
    delete d;
}

medCompositeParameter* medAbstractImageViewNavigator::cameraParameter()
{
    if(!d->cameraParameter)
    {
        d->cameraParameter = new medCompositeParameter("Camera", this);
        d->cameraParameter->addVariant("Camera Position", QVariant(QVector3D()));
        d->cameraParameter->addVariant("Camera Up", QVariant(QVector3D()));
        d->cameraParameter->addVariant("Camera Focal", QVariant(QVector3D()));
        d->cameraParameter->addVariant("Parallel Scale", QVariant((double)0.0), 0, HUGE_VAL);
        connect(d->cameraParameter, SIGNAL(valuesChanged(QHash<QString,QVariant>)), this, SLOT(setCamera(QHash<QString,QVariant>)));
    }
    return d->cameraParameter;
}

medAbstractVector3DParameter* medAbstractImageViewNavigator::positionBeingViewedParameter()
{
    if(!d->positionBeingViewedParameter)
    {
        d->positionBeingViewedParameter = new medVector3DParameter("Position", this);
        connect(d->positionBeingViewedParameter, SIGNAL(valueChanged(QVector3D)), this, SLOT(moveToPosition(QVector3D)));
    }
    return d->positionBeingViewedParameter;
}

medDoubleParameter* medAbstractImageViewNavigator::timeParameter()
{
    if(!d->timeParameter)
    {
        d->timeParameter = new medDoubleParameter("Time", this);
        connect(d->timeParameter, SIGNAL(valueChanged(double)), this, SLOT(setCurrentTime(double)));
    }
    return d->timeParameter;
}
