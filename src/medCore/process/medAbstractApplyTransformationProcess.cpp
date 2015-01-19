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
#include <medToolBox.h>
#include <medDataSourceDialog.h>


class medAbstractApplyTransformationProcessPrivate
{
public:
    medAbstractImageData *geometry;
    medAbstractImageData *inputImage;
    medAbstractImageData *transformedImage;

    QList<medAbstractTransformation *> transformations;

    QPointer<medToolBox> toolBox;
    QPushButton* addLinearTransfoButton;
    QPushButton* addSVFTransfoButton;
    QPushButton* addDFTransfoButton;

};

medAbstractApplyTransformationProcess::medAbstractApplyTransformationProcess(medAbstractProcess * parent):
    medAbstractProcess(parent), d(new medAbstractApplyTransformationProcessPrivate)
{
    d->geometry = NULL;
    d->inputImage = NULL;
    d->transformedImage = NULL;

    medProcessInput<medAbstractImageData> *geometry = new medProcessInput<medAbstractImageData>("Fixed Image", false, d->geometry);
    this->appendInput(geometry);

    medProcessInput<medAbstractImageData> *moving = new medProcessInput<medAbstractImageData>("Moving Image", false, d->inputImage);
    this->appendInput(moving);

    medProcessOutput<medAbstractImageData> *output = new medProcessOutput<medAbstractImageData>(("Output"), d->transformedImage);
    this->appendOutput(output);

    d->toolBox = NULL;
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
    QList<medAbstractParameter*> params;
    return params;
}

void medAbstractApplyTransformationProcess::addTransformation(medAbstractTransformation *transfo)
{
    d->transformations << transfo;
}

void medAbstractApplyTransformationProcess::addTransformations(QList<medAbstractTransformation *> transfo)
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
/*
medToolBox* medAbstractApplyTransformationProcess::toolbox()
{
    if(d->toolBox.isNull())
    {
        d->toolBox = new medToolBox;

        d->addSVFTransfoButton = new QPushButton(tr("Add SVF transfo"));
        connect(d->addSVFTransfoButton, SIGNAL(clicked()), this, SLOT(addSVFTransfoDialog()));

        d->toolBox->addWidget(d->addSVFTransfoButton);
    }

    return d->toolBox;
}*/

void medAbstractApplyTransformationProcess::addSVFTransfoDialog()
{
    medAbstractImageData *data = NULL;
    medDataSourceDialog dialog(NULL);
    dialog.exec();


    emit addSVFTransfoRequest(data);
}


