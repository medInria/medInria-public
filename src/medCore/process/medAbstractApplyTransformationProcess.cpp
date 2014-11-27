/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractApplyTransformationProcess.h>

#include <medAbstractImageData.h>


class medAbstractApplyTransformationProcessPrivate
{
public:
    medAbstractImageData *geometry;
    medAbstractImageData *inputImage;
    medAbstractImageData *transformedImage;

    QList<medAbstractTransformation *> transformations;
};

medAbstractApplyTransformationProcess::medAbstractApplyTransformationProcess():
    medAbstractProcess(parent), d(new medAbstractApplyTransformationProcessPrivate)
{
    d->geometry = NULL;
    d->inputImage = NULL;
    d->transformedImage = NULL;

    medProcessInput<medAbstractImageData> *geometry = new medProcessInput<medAbstractImageData>("Fixed Image", false);
    this->appendInput(geometry);

    medProcessInput<medAbstractImageData> *moving = new medProcessInput<medAbstractImageData>("Moving Image", false);
    this->appendInput(moving);

    this->appendOutput( new medProcessOutput<medAbstractImageData>("Output"));

}

medAbstractApplyTransformationProcess::~medAbstractApplyTransformationProcess()
{
    delete d;
    d = NULL;
}

bool medAbstractApplyTransformationProcess::isInteractive() const
{
    return false;
}

QList<medAbstractParameter*> medAbstractApplyTransformationProcess::parameters()
{
    //TODO GPR: temporary
    return QList<medAbstractParameter*>();
}

void medAbstractApplyTransformationProcess::addTransformation(medAbstractTransformation *transfo)
{
    d->transformations << transfo;
}

void medAbstractApplyTransformationProcess::addTransformation(QList<medAbstractTransformation *> transfo)
{
    d->transformations << transfo;
}

QList<medAbstractTransformation *> medAbstractApplyTransformationProcess::transformationStack() const
{
    return d->transformations;
}

void medAbstractApplyTransformationProcess::setGeometry(medAbstractImageData *geometry)
{
    d->geometry = geometry;
}

medAbstractImageData * medAbstractApplyTransformationProcess::geometry() const
{
    return d->geometry;
}

void medAbstractApplyTransformationProcess::setInputImage(medAbstractImageData *inputImage)
{
    d->inputImage = inputImage;
}

medAbstractImageData* medAbstractApplyTransformationProcess::inputImage() const
{
    return d->inputImage;
}

void medAbstractApplyTransformationProcess::resetTransformationStack()
{
    d->transformations.clear();
}
