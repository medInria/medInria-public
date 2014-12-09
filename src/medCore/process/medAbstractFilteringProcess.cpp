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
    medInputDataPort *input = new medInputDataPort("Input", false, NULL);
    input->setInput(NULL);

    medOutputDataPort *output = new medOutputDataPort("Output", NULL);
    //output->output= NULL;

    //TODO: Should this be done here or by implementation
    // (would allow to precise types)
    this->appendInput(input);
    this->appendOutput(output);
}

bool medAbstractFilteringProcess::isInteractive() const
{
    return false;
}


