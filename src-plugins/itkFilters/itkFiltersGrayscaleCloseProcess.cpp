/*=========================================================================
 medInria
 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.
=========================================================================*/

#include <itkFiltersGrayscaleCloseProcess.h>
#include <itkMorphologicalFiltersProcessBase_p.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

//-------------------------------------------------------------------------------------------

itkFiltersGrayscaleCloseProcess::itkFiltersGrayscaleCloseProcess(itkFiltersGrayscaleCloseProcess *parent)
    : itkMorphologicalFiltersProcessBase(*new itkMorphologicalFiltersProcessBasePrivate(this), parent)
{
    DTK_D(itkMorphologicalFiltersProcessBase);

    d->filter = this;
    d->description = tr("Grayscale Close filter");
}

itkFiltersGrayscaleCloseProcess::itkFiltersGrayscaleCloseProcess(const itkFiltersGrayscaleCloseProcess& other)
    : itkMorphologicalFiltersProcessBase(*new itkMorphologicalFiltersProcessBasePrivate(*other.d_func()), other)
{
}

itkFiltersGrayscaleCloseProcess::~itkFiltersGrayscaleCloseProcess( void )
{
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
