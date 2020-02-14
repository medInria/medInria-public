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
#include <medAbstractSymmetryPlaneAlignmentProcess.h>

#include <medComposerExport.h>

class medSymmetryPlaneAlignmentProcessNodePrivate;

class MEDCOMPOSER_EXPORT medSymmetryPlaneAlignmentProcessNode : public medSingleFilterOperationProcessNode <medAbstractSymmetryPlaneAlignmentProcess>
{
public:
     medSymmetryPlaneAlignmentProcessNode();

     virtual QWidget *editor(void);
};


