/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractSingleFilterOperationDoubleProcess.h>

#include <medAbstractImageData.h>
#include <medMetaDataKeys.h>

#include <medDoubleParameter.h>

class medAbstractSingleFilterOperationDoubleProcessPrivate
{
public:
    medDoubleParameter* doubleParameter;
};

medAbstractSingleFilterOperationDoubleProcess::medAbstractSingleFilterOperationDoubleProcess(QObject *parent): medAbstractSingleFilterOperationProcess(parent),
    d(new medAbstractSingleFilterOperationDoubleProcessPrivate)
{
    d->doubleParameter = new medDoubleParameter("sigma", this);
    d->doubleParameter->setCaption("CHANGE ME");
    d->doubleParameter->setDescription("CHANGE ME");
    d->doubleParameter->setValue(1);
}

medAbstractSingleFilterOperationDoubleProcess::~medAbstractSingleFilterOperationDoubleProcess()
{

}

medDoubleParameter* medAbstractSingleFilterOperationDoubleProcess::doubleParameter() const
{
    return d->doubleParameter;
}

