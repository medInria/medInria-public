/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

#include <itkFiltersErodeProcess.h>

//-------------------------------------------------------------------------------------------

itkFiltersErodeProcess::itkFiltersErodeProcess(itkFiltersErodeProcess *parent) 
    : itkMorphologicalFiltersProcessBase(parent)
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersErodeProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkErodeProcess", createitkFiltersErodeProcess);
}

QString itkFiltersErodeProcess::description() const
{
    return tr("Erode filter");
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersErodeProcess()
{
    return new itkFiltersErodeProcess;
}
