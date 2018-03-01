/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include <medAbstractDilateImageProcess.h>
#include <medMorphomathOperationProcessNode.h>

#include <medComposerExport.h>

class medDilateImageProcessNodePrivate;

class MEDCOMPOSER_EXPORT medDilateImageProcessNode : public medMorphomathOperationProcessNode<medAbstractDilateImageProcess>
{
public:
     medDilateImageProcessNode();
};


