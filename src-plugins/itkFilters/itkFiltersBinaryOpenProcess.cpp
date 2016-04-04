/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersBinaryOpenProcess.h>
#include <itkMorphologicalFiltersProcessBase_p.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

//-------------------------------------------------------------------------------------------

itkFiltersBinaryOpenProcess::itkFiltersBinaryOpenProcess(itkFiltersBinaryOpenProcess *parent)
    : itkMorphologicalFiltersProcessBase(*new itkMorphologicalFiltersProcessBasePrivate(this), parent)
{
    DTK_D(itkMorphologicalFiltersProcessBase);

    d->filter = this;
    d->description = tr("Open filter");
}

itkFiltersBinaryOpenProcess::itkFiltersBinaryOpenProcess(const itkFiltersBinaryOpenProcess& other)
    : itkMorphologicalFiltersProcessBase(*new itkMorphologicalFiltersProcessBasePrivate(*other.d_func()), other)
{
}

itkFiltersBinaryOpenProcess::~itkFiltersBinaryOpenProcess( void )
{
}

bool itkFiltersBinaryOpenProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkFiltersBinaryOpenProcess", createitkFiltersBinaryOpenProcess);
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createitkFiltersBinaryOpenProcess ( void )
{
    return new itkFiltersBinaryOpenProcess;
}
