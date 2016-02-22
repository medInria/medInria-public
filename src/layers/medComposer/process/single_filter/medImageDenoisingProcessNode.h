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
#include <medAbstractImageDenoisingProcess.h>

#include <medComposerExport.h>

class medImageDenoisingProcessNodePrivate;

class MEDCOMPOSER_EXPORT medImageDenoisingProcessNode : public medSingleFilterOperationProcessNode <medAbstractImageDenoisingProcess>
{
public:
     medImageDenoisingProcessNode();

     virtual QWidget *editor(void);
};


