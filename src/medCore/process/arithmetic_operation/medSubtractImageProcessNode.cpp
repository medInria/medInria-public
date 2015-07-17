/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSubtractImageProcessNode.h>

#include <medProcessLayer.h>

#include <medAbstractImageData.h>

class medSubtractImageProcessNodePrivate
{
public:
    dtkComposerTransmitterReceiver<medAbstractImageData*> input1;
    dtkComposerTransmitterReceiver<medAbstractImageData*> input2;

public:
    dtkComposerTransmitterEmitter<medAbstractImageData*> ouput;
};

medSubtractImageProcessNode::medSubtractImageProcessNode(void) : dtkComposerNodeObject<medAbstractSubtractImageProcessPresenter>(),
    d(new medSubtractImageProcessNodePrivate)
{
    this->setFactory(medProcessLayer::subtractImage::pluginFactory());

    this->appendReceiver(&d->input1);
    this->appendReceiver(&d->input2);

    this->appendEmitter(&d->ouput);
}

medSubtractImageProcessNode::~medSubtractImageProcessNode(void)
{
    delete d;
    d = NULL;
}

void medSubtractImageProcessNode::run(void)
{
    if(d->input1.data() && d->input2.data())
    {
        if (!this->object())
        {
            dtkWarn() << Q_FUNC_INFO << "No matrix instantiated, abort:" << this->currentImplementation();
            d->ouput.clearData();
            return;
        }

        this->object()->process()->setInput1(d->input1.data());
        this->object()->process()->setInput2(d->input2.data());

        this->object()->process()->run();

        d->ouput.setData(this->object()->process()->output());
    }
    else
    {

        dtkWarn() << Q_FUNC_INFO << "The inputs are not all set. Nothing is done.";
        d->ouput.clearData();
        return;

    }
}

//
// medSubtractImageProcessNode.h ends here
