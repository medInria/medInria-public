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
#include <medAbstractShrinkFilterProcess.h>

#include <medComposerExport.h>

class medShrinkFilterProcessNodePrivate;

class MEDCOMPOSER_EXPORT medShrinkFilterProcessNode : public medSingleFilterOperationProcessNode<medAbstractShrinkFilterProcess>
{
public:
     medShrinkFilterProcessNode();

     virtual QWidget *editor(void);
};


