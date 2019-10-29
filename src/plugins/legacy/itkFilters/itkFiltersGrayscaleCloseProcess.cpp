/*=========================================================================
 medInria
 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.
=========================================================================*/

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

#include <itkFiltersGrayscaleCloseProcess.h>

//-------------------------------------------------------------------------------------------

itkFiltersGrayscaleCloseProcess::itkFiltersGrayscaleCloseProcess(itkFiltersGrayscaleCloseProcess *parent)
    : itkMorphologicalFiltersProcessBase(parent)
{
}

bool itkFiltersGrayscaleCloseProcess::registered()
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

dtkAbstractProcess *createitkFiltersGrayscaleCloseProcess()
{
    return new itkFiltersGrayscaleCloseProcess;
}
