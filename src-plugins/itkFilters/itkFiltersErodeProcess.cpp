/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersErodeProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

//-------------------------------------------------------------------------------------------

itkFiltersErodeProcess::itkFiltersErodeProcess(itkFiltersErodeProcess *parent) 
    : itkMorphologicalFiltersProcessBase()
{
    filter = this;
    descriptionText = tr("Erode filter");
}

//-------------------------------------------------------------------------------------------

bool itkFiltersErodeProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkErodeProcess", createitkFiltersErodeProcess);
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersErodeProcess ( void )
{
    return new itkFiltersErodeProcess;
}
