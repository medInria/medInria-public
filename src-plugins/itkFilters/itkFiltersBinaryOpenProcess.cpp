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
    descriptionText = tr("Binary Open filter");
}

itkFiltersBinaryOpenProcess::itkFiltersBinaryOpenProcess(const itkFiltersBinaryOpenProcess& other)
     : itkMorphologicalFiltersProcessBase(other)
{

}

bool itkFiltersBinaryOpenProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkBinaryOpenProcess", createitkFiltersBinaryOpenProcess);
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createitkFiltersBinaryOpenProcess ( void )
{
    return new itkFiltersBinaryOpenProcess;
}
