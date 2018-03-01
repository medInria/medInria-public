/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include <medSingleFilterDoubleOperationProcessNode.h>
#include <medAbstractDivideFilterProcess.h>

#include <medComposerExport.h>

class medDivideFilterProcessNodePrivate;

class MEDCOMPOSER_EXPORT medDivideFilterProcessNode : public medSingleFilterDoubleOperationProcessNode<medAbstractDivideFilterProcess>
{
public:
     medDivideFilterProcessNode();

     virtual QWidget *editor(void);
};


