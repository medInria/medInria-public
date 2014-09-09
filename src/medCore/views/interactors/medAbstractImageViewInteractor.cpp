/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractImageViewInteractor.h>

#include <medAbstractImageView.h>

#include <medDoubleParameter.h>
#include <medCompositeParameter.h>

class medAbstractImageViewInteractorPrivate
{
public:
    medAbstractImageView *view;
    medDoubleParameter *opacityParameter;
    medCompositeParameter *windowLevelParameter;
};

medAbstractImageViewInteractor::medAbstractImageViewInteractor(medAbstractView *parent):
    medAbstractLayeredViewInteractor(parent), d(new medAbstractImageViewInteractorPrivate)
{
    d->view = dynamic_cast<medAbstractImageView *>(parent);
    d->opacityParameter = NULL;
    d->windowLevelParameter = NULL;

    connect(d->view, SIGNAL(currentTimeChanged(double)), this, SLOT(setCurrentTime(double)));
}

medAbstractImageViewInteractor::~medAbstractImageViewInteractor()
{
    delete d;
}


medDoubleParameter* medAbstractImageViewInteractor::opacityParameter()
{
    if(!d->opacityParameter)
    {
        d->opacityParameter = new medDoubleParameter("Opacity", this);
        d->opacityParameter->setRange(0,1);
        d->opacityParameter->setSingleStep(0.01);
        d->opacityParameter->setValue(1);
        connect(d->opacityParameter, SIGNAL(valueChanged(double)), this, SLOT(setOpacity(double)));
    }
    return d->opacityParameter;
}

medCompositeParameter* medAbstractImageViewInteractor::windowLevelParameter()
{
    if(!d->windowLevelParameter)
    {
        d->windowLevelParameter = new medCompositeParameter("WindowLevel", this);
        connect(d->windowLevelParameter, SIGNAL(valuesChanged(QHash<QString,QVariant>)), this, SLOT(setWindowLevel(QHash<QString,QVariant>)));
    }
    return d->windowLevelParameter;
}

/**
* @brief setCurrentTime Reimplement this methode if the data has a temporal component,
* to update what to display in the view according to the parameter time.
* Do not update the view, it has to be done by the primary navigator.
* @param time
*/
void medAbstractImageViewInteractor::setCurrentTime(double time)
{
    qDebug() << "No implementation of setCurrentTime(const double &time) for" << this->identifier();
}
