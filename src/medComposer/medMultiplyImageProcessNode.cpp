/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medMultiplyImageProcessNode.h>

#include <dtkComposer>
#include <medProcessLayer.h>
#include <medAbstractImageData.h>
#include <medAbstractMultiplyImageProcess.h>

class medMultiplyImageProcessNodePrivate
{
public:
    dtkComposerTransmitterReceiver<medAbstractImageData*> input1;
    dtkComposerTransmitterReceiver<medAbstractImageData*> input2;

    dtkComposerTransmitterEmitter<medAbstractImageData*> output;
};

medMultiplyImageProcessNode::medMultiplyImageProcessNode(void) : dtkComposerNodeObject<medAbstractMultiplyImageProcess>(),
    d(new medMultiplyImageProcessNodePrivate())
{
    this->setFactory(medProcessLayer::arithmeticalOperation::multiplyImage::pluginFactory());

    this->appendReceiver(&d->input1);
    this->appendReceiver(&d->input2);

    this->appendEmitter(&d->output);
}

medMultiplyImageProcessNode::~medMultiplyImageProcessNode(void)
{
    delete d;
}

void medMultiplyImageProcessNode::run(void)
{
    if(d->input1.isEmpty() || d->input2.isEmpty())
    {
        dtkDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }
    else
    {

        medAbstractMultiplyImageProcess* filter = this->object();
        if(!this->object())
            return;
        filter->setInput1(d->input1.data());
        filter->setInput2(d->input2.data());
        filter->run();
        d->output.setData(filter->output());
        dtkDebug()<<"filtering done";
    }
}

