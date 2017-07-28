/*=========================================================================
 medInria
 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.
=========================================================================*/

#include <itkFiltersGrayscaleOpenProcess.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

//-------------------------------------------------------------------------------------------

itkFiltersGrayscaleOpenProcess::itkFiltersGrayscaleOpenProcess(itkFiltersGrayscaleOpenProcess *parent)
    : itkMorphologicalFiltersProcessBase()
{
    filter = this;
    descriptionText = tr("Grayscale Open filter");
}

bool itkFiltersGrayscaleOpenProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkGrayscaleOpenProcess", createitkFiltersGrayscaleOpenProcess);
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createitkFiltersGrayscaleOpenProcess ( void )
{
    return new itkFiltersGrayscaleOpenProcess;
}
