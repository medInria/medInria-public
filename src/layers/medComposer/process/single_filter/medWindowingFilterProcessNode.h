#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSingleFilterOperationProcessNode.h>
#include <medAbstractWindowingFilterProcess.h>

#include <medComposerExport.h>

class medWindowingFilterProcessNodePrivate;

class MEDCOMPOSER_EXPORT medWindowingFilterProcessNode : public medSingleFilterOperationProcessNode<medAbstractWindowingFilterProcess>
{
public:
     medWindowingFilterProcessNode();

     virtual QWidget *editor(void);
};
