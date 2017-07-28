/*=========================================================================
 medInria
 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.
=========================================================================*/

#include <itkFiltersGrayscaleCloseProcess.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

//-------------------------------------------------------------------------------------------

itkFiltersGrayscaleCloseProcess::itkFiltersGrayscaleCloseProcess(itkFiltersGrayscaleCloseProcess *parent)
    : itkMorphologicalFiltersProcessBase()
{
    filter = this;
    descriptionText = tr("Grayscale Close filter");
}

bool itkFiltersGrayscaleCloseProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkGrayscaleCloseProcess", createitkFiltersGrayscaleCloseProcess);
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createitkFiltersGrayscaleCloseProcess ( void )
{
    return new itkFiltersGrayscaleCloseProcess;
}
