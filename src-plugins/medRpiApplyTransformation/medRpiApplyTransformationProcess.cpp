/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#include <medRpiApplyTransformationProcess.h>

#include <medAbstractImageData.h>
#include <medSVFTransformation.h>
#include <medAbstractTransformation.h>

// /////////////////////////////////////////////////////////////////
// medRpiApplyTransformationDiffeomorphicDemonsPrivate
// /////////////////////////////////////////////////////////////////

class medRpiApplyTransformationPrivate
{
public:
    medAbstractImageData *geometry;
    medAbstractImageData *inputImage;
    medAbstractImageData *transformedImage;

    QList<medAbstractTransformation *> transformations;
};

// /////////////////////////////////////////////////////////////////
// medRpiApplyTransformationProcess
// /////////////////////////////////////////////////////////////////

medRpiApplyTransformationProcess::medRpiApplyTransformationProcess() : medAbstractProcess(), d(new medRpiApplyTransformationPrivate)
{
    d->geometry = NULL;
    d->inputImage = NULL;
    d->transformedImage = NULL;
}

medRpiApplyTransformationProcess::~medRpiApplyTransformationProcess()
{
    delete d;
    d = NULL;
}

bool medRpiApplyTransformationProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("medRpiApplyTransformationProcess",
                                                                      createmedRpiApplyTransformation, "medAbstractRegistrationProcess");
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

void medRpiApplyTransformationProcess::addTransformation(medAbstractTransformation *transfo)
{
    d->transformations << transfo;
}

void medRpiApplyTransformationProcess::addTransformation(QList<medAbstractTransformation *> transfo)
{
    d->transformations << transfo;
}

QList<medAbstractTransformation *> medRpiApplyTransformationProcess::transformationStack() const
{
    return d->transformations;
}

void medRpiApplyTransformationProcess::setGeometry(medAbstractImageData *geometry)
{
    d->geometry = geometry;
}

medAbstractImageData * medRpiApplyTransformationProcess::geometry() const
{
    return d->geometry;
}

void medRpiApplyTransformationProcess::setInputImage(medAbstractImageData *inputImage)
{
    d->inputImage = inputImage;
}

medAbstractImageData* medRpiApplyTransformationProcess::inputImage() const
{
    return d->inputImage;
}

void medRpiApplyTransformationProcess::resetTransformationStack()
{
    d->transformations.clear();
}

void medRpiApplyTransformationProcess::update()
{
    if(!d->geometry || ! !d->inputImage || d->transformations.isEmpty())
        return;


}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////
/**
 * @brief Function to instantiate the process from the factory.
 * @see registered()
 *
 * @param void
*/
dtkAbstractProcess *createmedRpiApplyTransformation()
{
    return new medRpiApplyTransformationProcess;
}

