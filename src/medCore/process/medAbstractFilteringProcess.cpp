/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medAbstractFilteringProcess.h"

#include <medAbstractImageData.h>

medAbstractFilteringProcess::medAbstractFilteringProcess(medAbstractProcess *parent)
{
    medProcessInput<medAbstractImageData*> *input = new medProcessInput<medAbstractImageData*>;
    input->name = "Image";
    input->isOptional = false;
    this->appendInput( input );

    this->appendOutput( new medProcessInput<medAbstractImageData*>);
}

bool medAbstractFilteringProcess::isInteractive()
{
    return true;
}
