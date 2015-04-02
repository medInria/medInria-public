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
#include <medTriggerParameter.h>


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

    QUndoStack *commandStack;
    QUndoView* commandView;

    medTriggerParameter* resetStackParameter;

};

medAbstractApplyTransformationProcess::medAbstractApplyTransformationProcess(medAbstractProcess * parent):
    medAbstractProcess(parent), d(new medAbstractApplyTransformationProcessPrivate)
{
    d->geometry = NULL;
    d->inputImage = NULL;
    d->transformedImage = NULL;

    medProcessInput<medAbstractImageData> *geometry = new medProcessInput<medAbstractImageData>("Fixed Image", false, d->geometry);
    this->appendDataInput(geometry);

    medProcessInput<medAbstractImageData> *moving = new medProcessInput<medAbstractImageData>("Moving Image", false, d->inputImage);
    this->appendDataInput(moving);

    medProcessOutput<medAbstractImageData> *output = new medProcessOutput<medAbstractImageData>(("Output"), d->transformedImage);
    this->appendDataOutput(output);

    d->toolBox = NULL;

    d->commandStack = new QUndoStack(this);
    d->commandView = new QUndoView(d->commandStack);
    d->commandView->hide();

    d->resetStackParameter = new medTriggerParameter("Reset Stack", this);
    d->resetStackParameter->setButtonText("Reset Stack");

    connect(d->resetStackParameter, SIGNAL(triggered()), this, SLOT(resetTransformationStack()));
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
    d->commandStack->clear();
}

medToolBox* medAbstractApplyTransformationProcess::toolbox()
{
    if(d->toolBox.isNull())
    {
        d->toolBox = medAbstractProcess::toolbox();
        d->toolBox->addWidget(this->commandView());
        d->toolBox->addWidget(d->resetStackParameter->getPushButton());
        this->commandView()->show();
    }

    return d->toolBox;
}

void medAbstractApplyTransformationProcess::addSVFTransfoDialog()
{
    medAbstractImageData *data = NULL;
    medDataSourceDialog dialog(NULL);
    dialog.exec();


    emit addSVFTransfoRequest(data);
}

QUndoStack* medAbstractApplyTransformationProcess::commandStack()
{
    return d->commandStack;
}

QUndoView* medAbstractApplyTransformationProcess::commandView()
{
    return d->commandView;
}

