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
#include <medAbstractInvertFilterProcess.h>

#include <medComposerExport.h>

class medInvertFilterProcessNodePrivate;

class MEDCOMPOSER_EXPORT medInvertFilterProcessNode : public medSingleFilterOperationProcessNode<medAbstractInvertFilterProcess>
{
public:
     medInvertFilterProcessNode();

     virtual QWidget *editor(void);
};


