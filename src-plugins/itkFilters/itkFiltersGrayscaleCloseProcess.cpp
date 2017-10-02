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
    : itkMorphologicalFiltersProcessBase(parent)
{
}

bool itkFiltersGrayscaleCloseProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkGrayscaleCloseProcess", createitkFiltersGrayscaleCloseProcess);
}

QString itkFiltersGrayscaleCloseProcess::description() const
{
    return tr("Grayscale Close filter");
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createitkFiltersGrayscaleCloseProcess ( void )
{
    return new itkFiltersGrayscaleCloseProcess;
}
