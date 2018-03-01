/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDoubleParameter.h>
#include <medAbstractDivideFilterProcess.h>

medAbstractDivideFilterProcess::medAbstractDivideFilterProcess(QObject *parent)
    : medAbstractSingleFilterOperationDoubleProcess(parent)
{
    this->doubleParameter()->setCaption("Divided constant");
    this->doubleParameter()->setDescription("Value of divided constant");
}
