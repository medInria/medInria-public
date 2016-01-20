/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractSingleFilterOperation4DoubleProcess.h>

#include <medAbstractImageData.h>
#include <medMetaDataKeys.h>

#include <medDoubleParameter.h>

class medAbstractSingleFilterOperation4DoubleProcessPrivate
{
public:
    medDoubleParameter* w;
    medDoubleParameter* x;
    medDoubleParameter* y;
    medDoubleParameter* z;

};

medAbstractSingleFilterOperation4DoubleProcess::medAbstractSingleFilterOperation4DoubleProcess(QObject *parent): medAbstractSingleFilterOperationProcess(parent),
    d(new medAbstractSingleFilterOperation4DoubleProcessPrivate)
{
    d->w = new medDoubleParameter("w", this);
    d->x = new medDoubleParameter("x", this);
    d->y = new medDoubleParameter("y", this);
    d->z = new medDoubleParameter("z", this);
}

medAbstractSingleFilterOperation4DoubleProcess::~medAbstractSingleFilterOperation4DoubleProcess()
{

}

medDoubleParameter* medAbstractSingleFilterOperation4DoubleProcess::w()
{
    return d->w;
}

medDoubleParameter* medAbstractSingleFilterOperation4DoubleProcess::x()
{
    return d->x;
}

medDoubleParameter* medAbstractSingleFilterOperation4DoubleProcess::y()
{
    return d->y;
}

medDoubleParameter* medAbstractSingleFilterOperation4DoubleProcess::z()
{
    return d->z;
}

