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
#include <medTriggerParameter.h>
#include <medVector3DParameter.h>

class medAbstractImageViewNavigatorPrivate
{
public:
    medAbstractImageView *view;
    medCompositeParameter *cameraParameter;
    medVector3DParameter *positionBeingViewedParameter;
    medTriggerParameter *fourViewsParameter;
};

medAbstractImageViewNavigator::medAbstractImageViewNavigator(medAbstractView *parent):
    medAbstractLayeredViewNavigator(parent), d(new medAbstractImageViewNavigatorPrivate)
{
    d->view = dynamic_cast<medAbstractImageView *>(parent);
    d->positionBeingViewedParameter = NULL;
    d->cameraParameter = NULL;
    d->fourViewsParameter = NULL;
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
        d->cameraParameter->addVariant("Parallel Scale", QVariant((double)0.0));
        connect(d->cameraParameter, SIGNAL(valuesChanged(QList<QVariant>)), this, SLOT(setCamera(QList<QVariant>)));
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

medTriggerParameter *medAbstractImageViewNavigator::fourViewsParameter()
{
    if (!d->fourViewsParameter)
    {
        d->fourViewsParameter = new medTriggerParameter("Four views", this);
        QIcon fourViewsIcon (":/icons/fourViews.png");
        d->fourViewsParameter->setButtonIcon(fourViewsIcon);

        connect(d->fourViewsParameter,SIGNAL(triggered()),this,SLOT(switchToFourViews()));
    }
    
    return d->fourViewsParameter;
}
