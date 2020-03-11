#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

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
bool medSingleFilterOperationProcessNode<T>::prepareInput(void)
{
    if (d->input1.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return false;
    }
    else
    {

        if(this->object())
        {
            medAbstractSingleFilterOperationProcess* const filter = this->object();
            filter->setInput(d->input1.data());
        }
    }

    return true;
}

template <typename T>
void medSingleFilterOperationProcessNode<T>::prepareOutput(void)
{
    medAbstractSingleFilterOperationProcess* const filter = this->object();
    d->output.setData(filter->output());
}

template <typename T>
void medSingleFilterOperationProcessNode<T>::run(void)
{
    medAbstractSingleFilterOperationProcess* const filter = this->object();

    if (!filter || !this->prepareInput())
        return;

    filter->run();
    this->prepareOutput();

    qDebug()<<"filtering done";
}
