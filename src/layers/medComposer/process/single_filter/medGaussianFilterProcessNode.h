#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSingleFilterDoubleOperationProcessNode.h>
#include <medAbstractGaussianFilterProcess.h>

#include <medComposerExport.h>

class medGaussianFilterProcessNodePrivate;

class MEDCOMPOSER_EXPORT medGaussianFilterProcessNode : public medSingleFilterOperationProcessNode<medAbstractGaussianFilterProcess>
{
public:
     medGaussianFilterProcessNode();

     virtual QWidget *editor(void);
};
