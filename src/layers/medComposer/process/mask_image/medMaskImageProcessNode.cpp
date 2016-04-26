/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <medMaskImageProcessNode.h>

#include <dtkComposer>
#include <dtkLog>

#include <medAbstractImageData.h>
#include <medCore.h>

class medMaskImageProcessNodePrivate
{
public:
    dtkComposerTransmitterReceiver<medAbstractImageData *> input;
    dtkComposerTransmitterReceiver<medAbstractImageData *> mask;
    dtkComposerTransmitterEmitter<medAbstractImageData *> output;
};

medMaskImageProcessNode::medMaskImageProcessNode()
    : dtkComposerNodeObject<medAbstractMaskImageProcess>(), d(new medMaskImageProcessNodePrivate())
{
    this->appendReceiver(&d->input);
    this->appendReceiver(&d->mask);
    this->appendEmitter(&d->output);

    this->setFactory(medCore::maskImage::pluginFactory());
}

medMaskImageProcessNode::~medMaskImageProcessNode()
{
}

void medMaskImageProcessNode::run()
{
    if (d->input.isEmpty() || d->mask.isEmpty())
    {
        dtkDebug() << Q_FUNC_INFO << "Missing inputs. Aborting.";
        return;
    }

    medAbstractMaskImageProcess *filter = this->object();
    if (this->object())
    {
        filter->setInput(d->input.data());
        filter->setMask(d->mask.data());
        filter->run();
        d->output.setData(filter->output());
    }
}

