/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include <medAbstractErodeImageProcess.h>
#include <medMorphomathOperationProcessNode.h>

#include <medComposerExport.h>

class medErodeImageProcessNodePrivate;

class MEDCOMPOSER_EXPORT medErodeImageProcessNode : public medMorphomathOperationProcessNode<medAbstractErodeImageProcess>
{
public:
     medErodeImageProcessNode();
};


