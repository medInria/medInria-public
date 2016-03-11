/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersBinaryCloseProcess.h>
#include <itkMorphologicalFiltersProcessBase_p.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

//-------------------------------------------------------------------------------------------

itkFiltersBinaryCloseProcess::itkFiltersBinaryCloseProcess(itkFiltersBinaryCloseProcess *parent)
    : itkMorphologicalFiltersProcessBase(*new itkMorphologicalFiltersProcessBasePrivate(this), parent)
{
    DTK_D(itkMorphologicalFiltersProcessBase);

    d->filter = this;
    d->description = tr("Close filter");
}

itkFiltersBinaryCloseProcess::itkFiltersBinaryCloseProcess(const itkFiltersBinaryCloseProcess& other)
    : itkMorphologicalFiltersProcessBase(*new itkMorphologicalFiltersProcessBasePrivate(*other.d_func()), other)
{
}

itkFiltersBinaryCloseProcess::~itkFiltersBinaryCloseProcess( void )
{
}

bool itkFiltersBinaryCloseProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkFiltersBinaryCloseProcess", createitkFiltersBinaryCloseProcess);
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createitkFiltersBinaryCloseProcess ( void )
{
    return new itkFiltersBinaryCloseProcess;
}
