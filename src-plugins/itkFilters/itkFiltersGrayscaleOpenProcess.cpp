/*=========================================================================
 medInria
 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.
=========================================================================*/

#include <itkFiltersGrayscaleOpenProcess.h>
#include <itkMorphologicalFiltersProcessBase_p.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

//-------------------------------------------------------------------------------------------

itkFiltersGrayscaleOpenProcess::itkFiltersGrayscaleOpenProcess(itkFiltersGrayscaleOpenProcess *parent)
    : itkMorphologicalFiltersProcessBase(*new itkMorphologicalFiltersProcessBasePrivate(this), parent)
{
    DTK_D(itkMorphologicalFiltersProcessBase);

    d->filter = this;
    d->description = tr("Grayscale Open filter");
}

itkFiltersGrayscaleOpenProcess::itkFiltersGrayscaleOpenProcess(const itkFiltersGrayscaleOpenProcess& other)
    : itkMorphologicalFiltersProcessBase(*new itkMorphologicalFiltersProcessBasePrivate(*other.d_func()), other)
{
}

itkFiltersGrayscaleOpenProcess::~itkFiltersGrayscaleOpenProcess( void )
{
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
