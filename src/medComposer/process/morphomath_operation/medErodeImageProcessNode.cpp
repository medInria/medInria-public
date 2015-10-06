/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medErodeImageProcessNode.h>

#include <dtkComposer>

#include <medProcessLayer.h>
#include <medAbstractImageData.h>

class medErodeImageProcessNodePrivate
{
public:
    dtkComposerTransmitterReceiver<double> radius;
    dtkComposerTransmitterReceiver<medAbstractImageData*> input;

    dtkComposerTransmitterEmitter<medAbstractImageData*> output;
};

medErodeImageProcessNode::medErodeImageProcessNode(void) :
    dtkComposerNodeObject<medAbstractErodeImageProcess>(),
    d(new medErodeImageProcessNodePrivate())
{
    this->setFactory(medProcessLayer::morphomathOperation::erodeImage::pluginFactory());

    this->appendReceiver(&d->input);
    this->appendReceiver(&d->radius);
    this->appendEmitter (&d->output);
}

medErodeImageProcessNode::~medErodeImageProcessNode(void)
{
    delete d;
}

void medErodeImageProcessNode::run(void)
{
    if (d->radius.isEmpty() || d->input.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }
    else
    {

        medAbstractErodeImageProcess* filter = this->object();
        if(!this->object())
            return;
        filter->setInput(d->input.data());
        filter->setKernelRadius(d->radius.data());
        filter->run();
        d->output.setData(filter->output());
        qDebug()<<"filtering done";
    }
}

