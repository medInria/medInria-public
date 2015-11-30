/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractDWIMaskingProcess.h>

#include <medAbstractImageData.h>

class medAbstractDWIMaskingProcessPrivate
{
public:
    medAbstractImageData *input;
    medAbstractImageData *output;
};

medAbstractDWIMaskingProcess::medAbstractDWIMaskingProcess(QObject *parent)
    : medAbstractProcess(parent), d(new medAbstractDWIMaskingProcessPrivate)
{
    d->input = NULL;
    d->output = NULL;
}

medAbstractDWIMaskingProcess::~medAbstractDWIMaskingProcess()
{

}

void medAbstractDWIMaskingProcess::setInput(medAbstractImageData *data)
{
    if (data->Dimension() != 4)
        return;

    d->input = data;
}

medAbstractImageData* medAbstractDWIMaskingProcess::input() const
{
    return d->input;
}

medAbstractImageData* medAbstractDWIMaskingProcess::output() const
{
    return d->output;
}

void medAbstractDWIMaskingProcess::setOutput(medAbstractImageData *data)
{
    if (data->Dimension() != 3)
        return;

    d->output = data;
}
