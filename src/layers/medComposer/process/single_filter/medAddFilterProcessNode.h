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
#include <medAbstractAddFilterProcess.h>

#include <medComposerExport.h>

class medAddFilterProcessNodePrivate;

class MEDCOMPOSER_EXPORT medAddFilterProcessNode : public medSingleFilterOperationProcessNode<medAbstractAddFilterProcess>
{
public:
     medAddFilterProcessNode();
};


