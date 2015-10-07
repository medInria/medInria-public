/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSubtractImageProcessNode.h>

#include <dtkComposer>
#include <medProcessLayer.h>
#include <medAbstractImageData.h>
#include <medAbstractSubtractImageProcess.h>

class medSubtractImageProcessNodePrivate
{
public:
    dtkComposerTransmitterReceiver<medAbstractImageData*> input1;
    dtkComposerTransmitterReceiver<medAbstractImageData*> input2;

    dtkComposerTransmitterEmitter<medAbstractImageData*> output;
};

medSubtractImageProcessNode::medSubtractImageProcessNode(void) : dtkComposerNodeObject<medAbstractSubtractImageProcess>(),
    d(new medSubtractImageProcessNodePrivate)
{
    this->setFactory(medProcessLayer::arithmeticalOperation::subtractImage::pluginFactory());

    this->appendReceiver(&d->input1);
    this->appendReceiver(&d->input2);

    this->appendEmitter(&d->output);
}

medSubtractImageProcessNode::~medSubtractImageProcessNode(void)
{
    delete d;
}

void medSubtractImageProcessNode::run(void)
{
    if(d->input1.isEmpty() || d->input2.isEmpty())
    {
        dtkDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }
    else
    {

        medAbstractSubtractImageProcess* filter = this->object();
        if(!this->object())
            return;
        filter->setInput1(d->input1.data());
        filter->setInput2(d->input2.data());
        filter->run();
        d->output.setData(filter->output());
        dtkDebug()<<"filtering done";
    }
}

