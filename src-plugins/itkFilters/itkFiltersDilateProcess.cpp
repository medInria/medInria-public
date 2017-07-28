/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersDilateProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

//-------------------------------------------------------------------------------------------

itkFiltersDilateProcess::itkFiltersDilateProcess(itkFiltersDilateProcess *parent) 
    : itkMorphologicalFiltersProcessBase()
{  
    filter = this;
    descriptionText = tr("Dilate filter");
}

//-------------------------------------------------------------------------------------------

bool itkFiltersDilateProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkDilateProcess", createitkFiltersDilateProcess);
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersDilateProcess ( void )
{
    return new itkFiltersDilateProcess;
}
