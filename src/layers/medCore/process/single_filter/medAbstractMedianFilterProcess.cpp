/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractMedianFilterProcess.h>

#include <medAbstractImageData.h>
#include <medMetaDataKeys.h>

#include <medIntParameter.h>

class medAbstractMedianFilterProcessPrivate
{
public:
    medIntParameter* radius;
};

medAbstractMedianFilterProcess::medAbstractMedianFilterProcess(QObject *parent): medAbstractSingleFilterOperationProcess(parent),
    d(new medAbstractMedianFilterProcessPrivate)
{
    d->radius = new medIntParameter("radius", this);
    d->radius->setCaption("Box radius");
    d->radius->setValue(1);
    d->radius->setDescription("Median filter box radius");
}

medAbstractMedianFilterProcess::~medAbstractMedianFilterProcess()
{

}

medIntParameter* medAbstractMedianFilterProcess::radius()
{
    return d->radius;
}
