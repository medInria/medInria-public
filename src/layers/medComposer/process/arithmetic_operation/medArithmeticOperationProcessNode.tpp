/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medArithmeticOperationProcessNode.h>

#include <dtkComposer>
#include <dtkLog>

#include <medAbstractImageData.h>
#include <medDoubleParameter.h>

class medArithmeticOperationProcessNodePrivate
{
public:
    dtkComposerTransmitterReceiver<medAbstractImageData*> input1;
    dtkComposerTransmitterReceiver<medAbstractImageData*> input2;

    dtkComposerTransmitterEmitter<medAbstractImageData*> output;
};

template <typename T>
medArithmeticOperationProcessNode<T>::medArithmeticOperationProcessNode(void)
    : dtkComposerNodeObject<T>(), d(new medArithmeticOperationProcessNodePrivate())
{
    this->appendReceiver(&d->input1);
    this->appendReceiver(&d->input2);

    this->appendEmitter(&d->output);
}

template <typename T>
medArithmeticOperationProcessNode<T>::~medArithmeticOperationProcessNode(void)
{
}

template <typename T>
void medArithmeticOperationProcessNode<T>::run(void)
{
    if (d->input1.isEmpty() || d->input2.isEmpty())
    {
        dtkDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }
    else
    {
        medAbstractArithmeticOperationProcess* filter = this->object();
        if(this->object())
        {
            filter->setInput1(d->input1.data());
            filter->setInput2(d->input2.data());
            filter->run();
            d->output.setData(filter->output());
            dtkDebug()<<"filtering done";
        }
    }
}

