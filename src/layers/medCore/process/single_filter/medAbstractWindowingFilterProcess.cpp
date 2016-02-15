/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
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
    d->maximumIntensityValue = new medDoubleParameter("maximumIntensityValue", this);
    d->minimumOutputIntensityValue = new medDoubleParameter("minimumOutputIntensityValue", this);
    d->maximumOutputIntensityValue = new medDoubleParameter("maximumOutputIntensityValue", this);
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

