/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDoubleParameter.h>
#include <medAbstractSubtractFilterProcess.h>

medAbstractSubtractFilterProcess::medAbstractSubtractFilterProcess(QObject *parent)
    : medAbstractSingleFilterOperationDoubleProcess(parent)
{
    this->doubleParameter()->setCaption("Subtracted constant");
    this->doubleParameter()->setDescription("Value of subtracted constant");
}
