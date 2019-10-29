/*=========================================================================
 medInria
 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.
=========================================================================*/

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

#include <itkFiltersBinaryOpenProcess.h>

//-------------------------------------------------------------------------------------------

itkFiltersBinaryOpenProcess::itkFiltersBinaryOpenProcess(itkFiltersBinaryOpenProcess *parent)
    : itkMorphologicalFiltersProcessBase(parent)
{
}

bool itkFiltersBinaryOpenProcess::registered()
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

dtkAbstractProcess *createitkFiltersBinaryOpenProcess()
{
    return new itkFiltersBinaryOpenProcess;
}
