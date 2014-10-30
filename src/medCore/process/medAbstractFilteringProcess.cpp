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
    medInputDataPort *input = new medInputDataPort;
    input->name = "Input";
    input->isOptional = false;
    input->input = NULL;

    medOutputDataPort *output = new medOutputDataPort;
    output->name = "Output";
    output->output= NULL;


    this->appendInput(input);
    this->appendOutput(output);
}

bool medAbstractFilteringProcess::isInteractive()
{
    return true;
}


