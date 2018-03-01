/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include <medAbstractSubtractImageProcess.h>
#include <medArithmeticOperationProcessNode.h>

#include <medComposerExport.h>

class medSubtractImageProcessNodePrivate;

class MEDCOMPOSER_EXPORT medSubtractImageProcessNode : public medArithmeticOperationProcessNode<medAbstractSubtractImageProcess>
{
public:
     medSubtractImageProcessNode();
};


