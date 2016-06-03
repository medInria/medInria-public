/*=========================================================================
 medInria
 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.
=========================================================================*/

#include <itkFiltersOpenProcess.h>
#include <itkMorphologicalFiltersProcessBase_p.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

//-------------------------------------------------------------------------------------------

itkFiltersOpenProcess::itkFiltersOpenProcess(itkFiltersOpenProcess *parent)
    : itkMorphologicalFiltersProcessBase(*new itkMorphologicalFiltersProcessBasePrivate(this), parent)
{
    DTK_D(itkMorphologicalFiltersProcessBase);

    d->filter = this;
    d->description = tr("Open filter");
}

itkFiltersOpenProcess::itkFiltersOpenProcess(const itkFiltersOpenProcess& other)
    : itkMorphologicalFiltersProcessBase(*new itkMorphologicalFiltersProcessBasePrivate(*other.d_func()), other)
{
}

itkFiltersOpenProcess::~itkFiltersOpenProcess( void )
{
}

bool itkFiltersOpenProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkOpenProcess", createitkFiltersOpenProcess);
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createitkFiltersOpenProcess ( void )
{
    return new itkFiltersOpenProcess;
}
