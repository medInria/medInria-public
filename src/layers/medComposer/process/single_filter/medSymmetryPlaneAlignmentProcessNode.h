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
#include <medAbstractSymmetryPlaneAlignmentProcess.h>

#include <medComposerExport.h>

class medSymmetryPlaneAlignmentProcessNodePrivate;

class MEDCOMPOSER_EXPORT medSymmetryPlaneAlignmentProcessNode : public medSingleFilterOperationProcessNode <medAbstractSymmetryPlaneAlignmentProcess>
{
public:
     medSymmetryPlaneAlignmentProcessNode();

     virtual QWidget *editor(void);
};


