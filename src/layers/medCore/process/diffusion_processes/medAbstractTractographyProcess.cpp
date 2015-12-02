/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractTractographyProcess.h>
#include <medAbstractFibersData.h>
#include <medAbstractDiffusionModelImageData.h>

class medAbstractTractographyProcessPrivate
{
public:
    medAbstractDiffusionModelImageData *input;
    medAbstractFibersData *output;
};

medAbstractTractographyProcess::medAbstractTractographyProcess(QObject *parent)
    : medAbstractProcess(parent), d(new medAbstractTractographyProcessPrivate)
{
    d->input = NULL;
    d->output = NULL;
}

medAbstractTractographyProcess::~medAbstractTractographyProcess()
{
}

void medAbstractTractographyProcess::setInput(medAbstractDiffusionModelImageData *data)
{
    d->input = data;
}

medAbstractDiffusionModelImageData* medAbstractTractographyProcess::input() const
{
    return d->input;
}

medAbstractFibersData* medAbstractTractographyProcess::output() const
{
    return d->output;
}

void medAbstractTractographyProcess::setOutput(medAbstractFibersData *data)
{
    d->output = data;
}
