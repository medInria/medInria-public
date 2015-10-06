/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medOpeningImageProcessNode.h>

#include <dtkComposer>

#include <medProcessLayer.h>
#include <medAbstractImageData.h>

class medOpeningImageProcessNodePrivate
{
public:
    dtkComposerTransmitterReceiver<double> radius;
    dtkComposerTransmitterReceiver<medAbstractImageData*> input;

    dtkComposerTransmitterEmitter<medAbstractImageData*> output;
};

medOpeningImageProcessNode::medOpeningImageProcessNode(void) :
    dtkComposerNodeObject<medAbstractOpeningImageProcess>(),
    d(new medOpeningImageProcessNodePrivate())
{
    this->setFactory(medProcessLayer::morphomathOperation::openingImage::pluginFactory());

    this->appendReceiver(&d->input);
    this->appendReceiver(&d->radius);
    this->appendEmitter (&d->output);
}

medOpeningImageProcessNode::~medOpeningImageProcessNode(void)
{
    delete d;
}

void medOpeningImageProcessNode::run(void)
{
    if (d->radius.isEmpty() || d->input.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }
    else
    {

        medAbstractOpeningImageProcess* filter = this->object();
        if(!this->object())
            return;
        filter->setInput(d->input.data());
        filter->setKernelRadius(d->radius.data());
        filter->run();
        d->output.setData(filter->output());
        qDebug()<<"filtering done";
    }
}

