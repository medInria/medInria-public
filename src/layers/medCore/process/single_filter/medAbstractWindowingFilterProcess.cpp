/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractWindowingFilterProcess.h>

#include <medAbstractImageData.h>
#include <medMetaDataKeys.h>

#include <medDoubleParameter.h>


class medAbstractWindowingFilterProcessPrivate
{
public:
    medDoubleParameter* minimumIntensityValue;
    medDoubleParameter* maximumIntensityValue;
    medDoubleParameter* minimumOutputIntensityValue;
    medDoubleParameter* maximumOutputIntensityValue;

};

medAbstractWindowingFilterProcess::medAbstractWindowingFilterProcess(QObject *parent): medAbstractSingleFilterOperationProcess(parent),
    d(new medAbstractWindowingFilterProcessPrivate)
{
    d->minimumIntensityValue = new medDoubleParameter("minimumIntensityValue", this);
    d->minimumIntensityValue->setCaption("Minimum intensity");
    d->minimumIntensityValue->setDescription("Minimum input intensity range");

    d->maximumIntensityValue = new medDoubleParameter("maximumIntensityValue", this);
    d->maximumIntensityValue->setCaption("Maximum intensity");
    d->maximumIntensityValue->setDescription("Maximum input intensity range");

    d->minimumOutputIntensityValue = new medDoubleParameter("minimumOutputIntensityValue", this);
    d->minimumOutputIntensityValue->setCaption("Minimum output intensity");
    d->minimumOutputIntensityValue->setDescription("Minimum output intensity range");

    d->maximumOutputIntensityValue = new medDoubleParameter("maximumOutputIntensityValue", this);
    d->maximumOutputIntensityValue->setCaption("Maximum output intensity");
    d->maximumOutputIntensityValue->setDescription("Maximum output intensity range");
}

medAbstractWindowingFilterProcess::~medAbstractWindowingFilterProcess()
{

}

medDoubleParameter* medAbstractWindowingFilterProcess::minimumIntensityValue()
{
    return d->minimumIntensityValue;
}

medDoubleParameter* medAbstractWindowingFilterProcess::maximumIntensityValue()
{
    return d->maximumIntensityValue;
}

medDoubleParameter* medAbstractWindowingFilterProcess::minimumOutputIntensityValue()
{
    return d->minimumOutputIntensityValue;
}

medDoubleParameter* medAbstractWindowingFilterProcess::maximumOutputIntensityValue()
{
    return d->maximumOutputIntensityValue;
}

