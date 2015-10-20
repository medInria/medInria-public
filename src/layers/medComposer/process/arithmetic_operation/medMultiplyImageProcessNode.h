/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include <medArithmeticOperationProcessNode.h>
#include <medAbstractMultiplyImageProcess.h>

#include <medComposerExport.h>

class medMultiplyImageProcessNodePrivate;

class MEDCOMPOSER_EXPORT medMultiplyImageProcessNode : public medArithmeticOperationProcessNode<medAbstractMultiplyImageProcess>
{
public:
     medMultiplyImageProcessNode();
};


