/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medSingleFilterOperationProcessNode.h>

#include <dtkComposer>
#include <dtkLog>

#include <medAbstractImageData.h>
#include <medDoubleParameter.h>

class medSingleFilterOperationProcessNodePrivate
{
public:
    dtkComposerTransmitterReceiver<medAbstractImageData*> input1;

    dtkComposerTransmitterEmitter<medAbstractImageData*> output;
};

template <typename T>
medSingleFilterOperationProcessNode<T>::medSingleFilterOperationProcessNode(void)
    : dtkComposerNodeObject<T>(), d(new medSingleFilterOperationProcessNodePrivate())
{
    this->appendReceiver(&d->input1);

    this->appendEmitter(&d->output);
}

template <typename T>
medSingleFilterOperationProcessNode<T>::~medSingleFilterOperationProcessNode(void)
{
}

template <typename T>
void medSingleFilterOperationProcessNode<T>::run(void)
{
    if (d->input1.isEmpty())
    {
        dtkDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }
    else
    {
        medAbstractSingleFilterOperationProcess* filter = this->object();
        if(this->object())
        {
            filter->setInput(d->input1.data());
            filter->run();
            d->output.setData(filter->output());
            dtkDebug()<<"filtering done";
        }
    }
}

