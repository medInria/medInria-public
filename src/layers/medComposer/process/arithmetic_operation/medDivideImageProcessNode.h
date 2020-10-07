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
#include <medAbstractDivideImageProcess.h>

#include <medComposerExport.h>

class medDivideImageProcessNodePrivate;

class MEDCOMPOSER_EXPORT medDivideImageProcessNode : public medArithmeticOperationProcessNode<medAbstractDivideImageProcess>
{
public:
     medDivideImageProcessNode();
};
