/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDilateImageProcessNode.h>

#include <dtkComposer>

#include <medProcessLayer.h>
#include <medAbstractImageData.h>

class medDilateImageProcessNodePrivate
{
public:
    dtkComposerTransmitterReceiver<double> radius;
    dtkComposerTransmitterReceiver<medAbstractImageData*> input;

    dtkComposerTransmitterEmitter<medAbstractImageData*> output;
};

medDilateImageProcessNode::medDilateImageProcessNode(void) :
    dtkComposerNodeObject<medAbstractDilateImageProcess>(),
    d(new medDilateImageProcessNodePrivate())
{
    this->setFactory(medProcessLayer::morphomathOperation::dilateImage::pluginFactory());

    this->appendReceiver(&d->input);
    this->appendReceiver(&d->radius);
    this->appendEmitter (&d->output);
}

medDilateImageProcessNode::~medDilateImageProcessNode(void)
{
    delete d;
}

void medDilateImageProcessNode::run(void)
{
    if (d->radius.isEmpty() || d->input.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }
    else
    {

        medAbstractDilateImageProcess* filter = this->object();
        if(!this->object())
            return;
        filter->setInput(d->input.data());
        filter->setKernelRadius(d->radius.data());
        filter->run();
        d->output.setData(filter->output());
        qDebug()<<"filtering done";
    }
}

