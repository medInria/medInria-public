/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractFilteringProcess.h>

#include <medAbstractImageData.h>
#include <medViewContainerSplitter.h>
#include <medViewContainer.h>


medAbstractFilteringProcess::medAbstractFilteringProcess(medAbstractProcess *parent)
{
    medProcessInput<medAbstractData> *input = new medProcessInput<medAbstractData>("Input", false, NULL);
    medProcessOutput<medAbstractData> *output = new medProcessOutput<medAbstractData>("Output", NULL);
    //output->output= NULL;

    //TODO: Should this be done here or by implementation
    // (would allow to precise types)
    this->appendDataInput(input);
    this->appendDataOutput(output);
}

bool medAbstractFilteringProcess::isInteractive() const
{
    return false;
}


