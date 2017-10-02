/*=========================================================================
 medInria
 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.
=========================================================================*/

#include <itkFiltersBinaryOpenProcess.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

//-------------------------------------------------------------------------------------------

itkFiltersBinaryOpenProcess::itkFiltersBinaryOpenProcess(itkFiltersBinaryOpenProcess *parent)
    : itkMorphologicalFiltersProcessBase(parent)
{
}

bool itkFiltersBinaryOpenProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkBinaryOpenProcess", createitkFiltersBinaryOpenProcess);
}

QString itkFiltersBinaryOpenProcess::description() const
{
    return tr("Binary Open filter");
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createitkFiltersBinaryOpenProcess ( void )
{
    return new itkFiltersBinaryOpenProcess;
}
