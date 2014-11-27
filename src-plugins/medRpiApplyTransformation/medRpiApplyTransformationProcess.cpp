/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#include <medRpiApplyTransformationProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

#include <medAbstractImageData.h>
#include <medSVFTransformation.h>
#include <medAbstractTransformation.h>

// /////////////////////////////////////////////////////////////////
// medRpiApplyTransformationDiffeomorphicDemonsPrivate
// /////////////////////////////////////////////////////////////////

class medRpiApplyTransformationProcessPrivate
{

};

// /////////////////////////////////////////////////////////////////
// medRpiApplyTransformationProcess
// /////////////////////////////////////////////////////////////////

medRpiApplyTransformationProcess::medRpiApplyTransformationProcess() : medAbstractProcess(), d(new medRpiApplyTransformationProcessPrivate)
{

}

medRpiApplyTransformationProcess::~medRpiApplyTransformationProcess()
{
    delete d;
    d = NULL;
}

bool medRpiApplyTransformationProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("medRpiApplyTransformationProcess",
                                                                      createmedRpiApplyTransformation, "medAbstractEstimateTransformationProcess");
}

QString medRpiApplyTransformationProcess::description() const
{
    return "medRpiApplyTransformationProcess";
}

QString medRpiApplyTransformationProcess::identifier() const
{
    return "medRpiApplyTransformationProcess";
}

QList<medAbstractParameter*> medRpiApplyTransformationProcess::parameters()
{

}

int medRpiApplyTransformationProcess::update()
{
    if(!this->geometry() || !this->inputImage() || this->transformationStack().isEmpty())
    {
        qDebug() << "Wrong parameters, could not apply rpi transformation...";
        return false;
    }

    qDebug() << "medRpiApplyTransformationProcess update !!!";
    return true;
}

bool medRpiApplyTransformationProcess::isInteractive() const
{
    return false;
}

dtkAbstractProcess *createmedRpiApplyTransformation()
{
    return new medRpiApplyTransformationProcess;
}

