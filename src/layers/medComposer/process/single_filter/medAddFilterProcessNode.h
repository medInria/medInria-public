/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include <medSingleFilterDoubleOperationProcessNode.h>
#include <medAbstractAddFilterProcess.h>

#include <medComposerExport.h>


class MEDCOMPOSER_EXPORT medAddFilterProcessNode : public medSingleFilterDoubleOperationProcessNode<medAbstractAddFilterProcess>
{
public:
     medAddFilterProcessNode();

     virtual QWidget *editor(void);
};


