/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medAbstractSegmentationProcess.h"

#include <medAbstractImageData.h>

medAbstractSegmentationProcess::medAbstractSegmentationProcess(medAbstractProcess *parent): medAbstractProcess(parent)
{
    medProcessDataInput<medAbstractImageData> *input = new medProcessDataInput<medAbstractImageData>("Image", false, NULL);
    this->appendDataInput( input );

    this->appendDataOutput( new medProcessOutput<medAbstractImageData>("Output", NULL));
}

bool medAbstractSegmentationProcess::isInteractive() const
{
    return true;
}
