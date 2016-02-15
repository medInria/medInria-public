/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
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
    medDoubleParameter* sigma;
};

medAbstractSingleFilterOperationDoubleProcess::medAbstractSingleFilterOperationDoubleProcess(QObject *parent): medAbstractSingleFilterOperationProcess(parent),
    d(new medAbstractSingleFilterOperationDoubleProcessPrivate)
{
    d->sigma = new medDoubleParameter("sigma", this);
    d->sigma->setCaption("CHANGE ME");
    d->sigma->setDescription("CHANGE ME");
    d->sigma->setValue(1);
}

medAbstractSingleFilterOperationDoubleProcess::~medAbstractSingleFilterOperationDoubleProcess()
{

}

medDoubleParameter* medAbstractSingleFilterOperationDoubleProcess::sigma() const
{
    return d->sigma;
}

