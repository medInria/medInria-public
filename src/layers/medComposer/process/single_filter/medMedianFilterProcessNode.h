/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include <medSingleFilterOperationProcessNode.h>
#include <medAbstractMedianFilterProcess.h>

#include <medComposerExport.h>

class medMedianFilterProcessNodePrivate;

class MEDCOMPOSER_EXPORT medMedianFilterProcessNode : public medSingleFilterOperationProcessNode<medAbstractMedianFilterProcess>
{
public:
     medMedianFilterProcessNode();
};


