/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractEstimateTransformationProcess.h>

#include <medAbstractImageData.h>
#include <medAbstractTransformation.h>
#include <medTriggerParameter.h>


class medAbstractEstimateTransformationProcessPrivate
{
public:
    medTriggerParameter* saveTransfoParameter;
};



medAbstractEstimateTransformationProcess::medAbstractEstimateTransformationProcess(medAbstractProcess *parent):
    medAbstractProcess(parent), d(new medAbstractEstimateTransformationProcessPrivate)
{
    medProcessDataInput<medAbstractImageData> *fixed = new medProcessDataInput<medAbstractImageData>("Fixed Image", false);
    this->appendDataInput( fixed );

    medProcessDataInput<medAbstractImageData> *moving = new medProcessDataInput<medAbstractImageData>("Moving Image", false);
    this->appendDataInput( moving );

    this->appendOutput( new medProcessOutput<medAbstractTransformation>("Output"));

    d->saveTransfoParameter = new medTriggerParameter("Save Transfo", this);
    connect(d->saveTransfoParameter, SIGNAL(triggered()), this, SLOT(saveTransform()));
}

medAbstractEstimateTransformationProcess::~medAbstractEstimateTransformationProcess()
{

}

bool medAbstractEstimateTransformationProcess::isInteractive() const
{
    return false;
}

QList<medAbstractParameter*> medAbstractEstimateTransformationProcess::parameters()
{
    QList<medAbstractParameter*> params;
    params << d->saveTransfoParameter;
    return params;
}

bool medAbstractEstimateTransformationProcess::saveTransform()
{
    return true;

}
