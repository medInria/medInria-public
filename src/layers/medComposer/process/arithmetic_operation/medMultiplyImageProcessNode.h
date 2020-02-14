#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medArithmeticOperationProcessNode.h>
#include <medAbstractMultiplyImageProcess.h>

#include <medComposerExport.h>

class medMultiplyImageProcessNodePrivate;

class MEDCOMPOSER_EXPORT medMultiplyImageProcessNode : public medArithmeticOperationProcessNode<medAbstractMultiplyImageProcess>
{
public:
     medMultiplyImageProcessNode();
};
