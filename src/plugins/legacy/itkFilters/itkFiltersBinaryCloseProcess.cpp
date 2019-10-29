/*=========================================================================
 medInria
 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.
=========================================================================*/

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

#include <itkFiltersBinaryCloseProcess.h>

//-------------------------------------------------------------------------------------------

itkFiltersBinaryCloseProcess::itkFiltersBinaryCloseProcess(itkFiltersBinaryCloseProcess *parent)
    : itkMorphologicalFiltersProcessBase(parent)
{
}

bool itkFiltersBinaryCloseProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkBinaryCloseProcess", createitkFiltersBinaryCloseProcess);
}

QString itkFiltersBinaryCloseProcess::description() const
{
    return tr("Binary Close filter");
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createitkFiltersBinaryCloseProcess()
{
    return new itkFiltersBinaryCloseProcess;
}
