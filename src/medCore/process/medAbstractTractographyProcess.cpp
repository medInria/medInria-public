/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medAbstractTractographyProcess.h"

#include <medAbstractFibersData.h>
#include <medAbstractDiffusionModelImageData.h>


medAbstractTractographyProcess::medAbstractTractographyProcess(medAbstractProcess* parent): medAbstractProcess(parent)
{
    medProcessInput<medAbstractDiffusionModelImageData> *input = new medProcessInput<medAbstractDiffusionModelImageData>("Diffusion Model", false);
    this->appendInput( input );

    this->appendOutput( new medProcessOutput<medAbstractFibersData>("Output"));
}

bool medAbstractTractographyProcess::isInteractive() const
{
    return false;
}
