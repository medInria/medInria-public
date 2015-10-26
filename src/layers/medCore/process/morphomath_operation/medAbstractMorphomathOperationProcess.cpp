/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractMorphomathOperationProcess.h>

#include <medAbstractImageData.h>
#include <medIntParameter.h>

class medAbstractMorphomathOperationProcessPrivate
{
public:
    medAbstractImageData *input;
    medAbstractImageData *output;

    medIntParameter *kernelRadius;
};

medAbstractMorphomathOperationProcess::medAbstractMorphomathOperationProcess(QObject *parent)
    : medAbstractProcess(parent), d(new medAbstractMorphomathOperationProcessPrivate)
{
    d->input = NULL;
    d->output = NULL;

    d->kernelRadius = new medIntParameter("kernel_radius", this);
    d->kernelRadius->setCaption("Kernel radius");
    d->kernelRadius->setDescription("Set the radius in all directions of the kernel used as structuring element");
    d->kernelRadius->setValue(1);
}

medAbstractMorphomathOperationProcess::~medAbstractMorphomathOperationProcess()
{

}

void medAbstractMorphomathOperationProcess::setInput(medAbstractImageData *data)
{
    d->input = data;
}

medAbstractImageData* medAbstractMorphomathOperationProcess::input() const
{
    return d->input;
}

medAbstractImageData* medAbstractMorphomathOperationProcess::output() const
{
    return d->output;
}

void medAbstractMorphomathOperationProcess::setOutput(medAbstractImageData *data)
{
    d->output = data;
}

medIntParameter* medAbstractMorphomathOperationProcess::kernelRadius() const
{
    return d->kernelRadius;
}
