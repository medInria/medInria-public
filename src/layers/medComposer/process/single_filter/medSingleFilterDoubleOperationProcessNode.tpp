#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSingleFilterDoubleOperationProcessNode.h>

#include <dtkComposer>
#include <dtkLog>

#include <medAbstractImageData.h>
#include <medDoubleParameter.h>

#include <medWidgets.h>

#include <medAbstractSingleFilterOperationDoubleProcess.h>
#include <medAbstractSingleFilterOperationDoubleProcessPresenter.h>

class medSingleFilterDoubleOperationProcessNodePrivate
{
public:
    dtkComposerTransmitterReceiver<double> doubleParameter;
};

template <typename T>
medSingleFilterDoubleOperationProcessNode<T>::medSingleFilterDoubleOperationProcessNode(void)
    : medSingleFilterOperationProcessNode<T>(), d(new medSingleFilterDoubleOperationProcessNodePrivate())
{
    this->appendReceiver(&d->doubleParameter);
}

template <typename T>
medSingleFilterDoubleOperationProcessNode<T>::~medSingleFilterDoubleOperationProcessNode(void)
{
}

template <typename T>
bool medSingleFilterDoubleOperationProcessNode<T>::prepareInput(void)
{
    bool status = medSingleFilterOperationProcessNode<T>::prepareInput();

    if (status)
    {
        if(!d->doubleParameter.isEmpty())
        {

            if(this->object())
            {
                medAbstractSingleFilterOperationDoubleProcess* const filter = this->object();
                filter->doubleParameter()->setValue(d->doubleParameter.data());
            }
        }
    }

    return status;
}
