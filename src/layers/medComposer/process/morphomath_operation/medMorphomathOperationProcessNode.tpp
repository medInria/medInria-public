/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include <medMorphomathOperationProcessNode.h>

#include <dtkComposer>
#include <dtkLog>

#include <medAbstractImageData.h>
#include <medDoubleParameter.h>

class medMorphomathOperationProcessNodePrivate
{
public:
    dtkComposerTransmitterReceiver<double> radius;
    dtkComposerTransmitterReceiver<medAbstractImageData*> input;

    dtkComposerTransmitterEmitter<medAbstractImageData*> output;
};

template <typename T>
medMorphomathOperationProcessNode<T>::medMorphomathOperationProcessNode(void)
    : dtkComposerNodeObject<T>(), d(new medMorphomathOperationProcessNodePrivate())
{
    this->appendReceiver(&d->input);
    this->appendReceiver(&d->radius);
    this->appendEmitter (&d->output);
}

template <typename T>
medMorphomathOperationProcessNode<T>::~medMorphomathOperationProcessNode(void)
{
}

template <typename T>
void medMorphomathOperationProcessNode<T>::run(void)
{
    if (d->radius.isEmpty() || d->input.isEmpty())
    {
        dtkDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }
    else
    {
        medAbstractMorphomathOperationProcess* filter = this->object();
        if(this->object())
        {
            filter->setInput(d->input.data());
            filter->kernelRadius()->setValue(d->radius.data());
            filter->run();
            d->output.setData(filter->output());
            dtkDebug()<<"filtering done";
        }
    }
}

