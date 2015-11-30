/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractDiffusionModelEstimationProcess.h>
#include <medAbstractImageData.h>

class medAbstractDiffusionModelEstimationProcessPrivate
{
public:
    medAbstractImageData *input;
    medAbstractImageData *mask;
    medAbstractImageData *output;
};

medAbstractDiffusionModelEstimationProcess::medAbstractDiffusionModelEstimationProcess(QObject *parent)
    : medAbstractProcess(parent), d(new medAbstractDiffusionModelEstimationProcessPrivate)
{
    d->input = NULL;
    d->mask = NULL;
    d->output = NULL;
}

medAbstractDiffusionModelEstimationProcess::~medAbstractDiffusionModelEstimationProcess()
{
}

void medAbstractDiffusionModelEstimationProcess::setInput(medAbstractImageData *data)
{
    if (data->Dimension() != 4)
        return;

    d->input = data;
}

medAbstractImageData* medAbstractDiffusionModelEstimationProcess::input() const
{
    return d->input;
}

void medAbstractDiffusionModelEstimationProcess::setMask(medAbstractImageData *mask)
{
    if (mask->Dimension() != 3)
        return;

    d->mask = mask;
}

medAbstractImageData* medAbstractDiffusionModelEstimationProcess::mask() const
{
    return d->mask;
}

medAbstractImageData* medAbstractDiffusionModelEstimationProcess::output() const
{
    return d->output;
}

void medAbstractDiffusionModelEstimationProcess::setOutput(medAbstractImageData *data)
{
    if (data->Dimension() != 4)
        return;

    d->output = data;
}
