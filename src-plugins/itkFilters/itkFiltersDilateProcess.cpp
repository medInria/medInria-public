/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersDilateProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractDataFactory.h>

#include <medMetaDataKeys.h>
#include <itkMorphologicalFiltersProcessBase_p.h>

//-------------------------------------------------------------------------------------------

itkFiltersDilateProcess::itkFiltersDilateProcess(itkFiltersDilateProcess *parent) 
    : itkMorphologicalFiltersProcessBase(*new itkMorphologicalFiltersProcessBasePrivate(this), parent)
{
    DTK_D(itkMorphologicalFiltersProcessBase);
    
    d->filter = this;
    d->description = tr("Dilate filter");
}


itkFiltersDilateProcess::itkFiltersDilateProcess(const itkFiltersDilateProcess& other) 
    : itkMorphologicalFiltersProcessBase(*new itkMorphologicalFiltersProcessBasePrivate(*other.d_func()), other)
{
}

//-------------------------------------------------------------------------------------------

itkFiltersDilateProcess::~itkFiltersDilateProcess( void )
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersDilateProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkDilateProcess", createitkFiltersDilateProcess);
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersDilateProcess ( void )
{
    return new itkFiltersDilateProcess;
}
