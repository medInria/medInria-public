/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

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
    dtkComposerTransmitterReceiver<double> sigma;
};

template <typename T>
medSingleFilterDoubleOperationProcessNode<T>::medSingleFilterDoubleOperationProcessNode(void)
    : medSingleFilterOperationProcessNode<T>(), d(new medSingleFilterDoubleOperationProcessNodePrivate())
{
    this->appendReceiver(&d->sigma);
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
        if(!d->sigma.isEmpty())
        {

            if(this->object())
            {
                medAbstractSingleFilterOperationDoubleProcess* const filter = this->object();
                filter->sigma()->setValue(d->sigma.data());
            }
        }
    }

    return status;
}
