#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medMorphomathOperationProcessNode.h>

#include <dtkComposer>
#include <dtkLog>

#include <medAbstractImageData.h>
#include <medIntParameter.h>

class medMorphomathOperationProcessNodePrivate
{
public:
    dtkComposerTransmitterReceiver<int> radius;
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
        qDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
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
            qDebug()<<"filtering done";
        }
    }
}
