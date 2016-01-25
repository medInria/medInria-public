/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractWindowingProcess.h>

#include <medAbstractImageData.h>
#include <medMetaDataKeys.h>

#include <medDoubleParameter.h>


class medAbstractWindowingProcessPrivate
{
public:
    medDoubleParameter* minimumIntensityValue;
    medDoubleParameter* maximumIntensityValue;
    medDoubleParameter* minimumOutputIntensityValue;
    medDoubleParameter* maximumOutputIntensityValue;

};

medAbstractWindowingProcess::medAbstractWindowingProcess(QObject *parent): medAbstractSingleFilterOperationProcess(parent),
    d(new medAbstractWindowingProcessPrivate)
{
    d->minimumIntensityValue = new medDoubleParameter("minimumIntensityValue", this);
    d->maximumIntensityValue = new medDoubleParameter("maximumIntensityValue", this);
    d->minimumOutputIntensityValue = new medDoubleParameter("minimumOutputIntensityValue", this);
    d->maximumOutputIntensityValue = new medDoubleParameter("maximumOutputIntensityValue", this);
}

medAbstractWindowingProcess::~medAbstractWindowingProcess()
{

}

medDoubleParameter* medAbstractWindowingProcess::minimumIntensityValue()
{
    return d->minimumIntensityValue;
}

medDoubleParameter* medAbstractWindowingProcess::maximumIntensityValue()
{
    return d->maximumIntensityValue;
}

medDoubleParameter* medAbstractWindowingProcess::minimumOutputIntensityValue()
{
    return d->minimumOutputIntensityValue;
}

medDoubleParameter* medAbstractWindowingProcess::maximumOutputIntensityValue()
{
    return d->maximumOutputIntensityValue;
}

