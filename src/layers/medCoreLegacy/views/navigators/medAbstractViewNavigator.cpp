/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractViewNavigator.h>

#include <medAbstractView.h>

#include <medDoubleParameter.h>
#include <medVector2DParameter.h>

class medAbstractViewNavigatorPrivate
{
public:
    medAbstractView *view;
    medDoubleParameter *zoomParameter;
    medVector2DParameter *panParameter;   
};

medAbstractViewNavigator::medAbstractViewNavigator(medAbstractView* parent):
    medAbstractNavigator(parent), d(new medAbstractViewNavigatorPrivate)
{
    d->view = parent;
    d->zoomParameter = NULL;
    d->panParameter = NULL;
}

medAbstractViewNavigator::~medAbstractViewNavigator()
{
    delete d;
}

medDoubleParameter* medAbstractViewNavigator::zoomParameter()
{
    if(!d->zoomParameter)
    {
        d->zoomParameter = new medDoubleParameter("Zoom", this);
        d->zoomParameter->setValue(1);
        connect(d->zoomParameter, SIGNAL(valueChanged(double)), this, SLOT(setZoom(double)));
    }
    return d->zoomParameter;
}

medAbstractVector2DParameter* medAbstractViewNavigator::panParameter()
{
    if(!d->panParameter)
    {
        d->panParameter = new medVector2DParameter("Pan", this);
        connect(d->panParameter, SIGNAL(valueChanged(QVector2D)), this, SLOT(setPan(QVector2D)));
    }
    return d->panParameter;
}
