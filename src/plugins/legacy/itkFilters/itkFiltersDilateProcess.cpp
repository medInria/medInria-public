/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

#include <itkFiltersDilateProcess.h>

//-------------------------------------------------------------------------------------------

itkFiltersDilateProcess::itkFiltersDilateProcess(itkFiltersDilateProcess *parent) 
    : itkMorphologicalFiltersProcessBase(parent)
{  
}

//-------------------------------------------------------------------------------------------

bool itkFiltersDilateProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkDilateProcess", createitkFiltersDilateProcess);
}

QString itkFiltersDilateProcess::description() const
{
    return tr("Dilate filter");
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersDilateProcess()
{
    return new itkFiltersDilateProcess;
}
