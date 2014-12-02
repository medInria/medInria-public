/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medAbstractDiffScalarMapsProcess.h"

#include <medAbstractImageData.h>
#include <medAbstractDiffusionModelImageData.h>


medAbstractDiffScalarMapsProcess::medAbstractDiffScalarMapsProcess(medAbstractProcess* parent): medAbstractProcess(parent)
{
    medProcessInput<medAbstractDiffusionModelImageData> *input = new medProcessInput<medAbstractDiffusionModelImageData>("Diffusion Model", false, NULL);
    this->appendInput( input );

    this->appendOutput( new medProcessOutput<medAbstractImageData*>("Output", NULL));
}

bool medAbstractDiffScalarMapsProcess::isInteractive() const
{
    return false;
}
