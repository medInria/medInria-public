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
#include <medAbstractNormalizeFilterProcess.h>

#include <medComposerExport.h>

class medNormalizeFilterProcessNodePrivate;

class MEDCOMPOSER_EXPORT medNormalizeFilterProcessNode : public medSingleFilterOperationProcessNode<medAbstractNormalizeFilterProcess>
{
public:
     medNormalizeFilterProcessNode();

     virtual QWidget *editor(void);
};
