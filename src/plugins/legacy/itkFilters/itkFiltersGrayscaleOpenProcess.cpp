/*=========================================================================
 medInria
 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.
=========================================================================*/

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

#include <itkFiltersGrayscaleOpenProcess.h>

//-------------------------------------------------------------------------------------------

itkFiltersGrayscaleOpenProcess::itkFiltersGrayscaleOpenProcess(itkFiltersGrayscaleOpenProcess *parent)
    : itkMorphologicalFiltersProcessBase(parent)
{
}

bool itkFiltersGrayscaleOpenProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkGrayscaleOpenProcess", createitkFiltersGrayscaleOpenProcess);
}

QString itkFiltersGrayscaleOpenProcess::description() const
{
    return tr("Grayscale Open filter");
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createitkFiltersGrayscaleOpenProcess()
{
    return new itkFiltersGrayscaleOpenProcess;
}
