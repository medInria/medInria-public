/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkFiltersComponentSizeThresholdProcess.h"

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <itkCastImageFilter.h>

#include <medMetaDataKeys.h>
#include <medAbstractDataFactory.h>

#include "itkFiltersComponentSizeThresholdProcess_p.h"

//-------------------------------------------------------------------------------------------

itkFiltersComponentSizeThresholdProcess::itkFiltersComponentSizeThresholdProcess(itkFiltersComponentSizeThresholdProcess *parent) 
    : itkFiltersProcessBase(*new itkFiltersComponentSizeThresholdProcessPrivate(this), parent)
{
    DTK_D(itkFiltersComponentSizeThresholdProcess);
    
    d->filter = this;
    d->output = NULL;
    
    d->description = tr("Size Threshold filter");
}


itkFiltersComponentSizeThresholdProcess::itkFiltersComponentSizeThresholdProcess(const itkFiltersComponentSizeThresholdProcess& other) 
    : itkFiltersProcessBase(*new itkFiltersComponentSizeThresholdProcessPrivate(*other.d_func()), other)
{
}

//-------------------------------------------------------------------------------------------

itkFiltersComponentSizeThresholdProcess::~itkFiltersComponentSizeThresholdProcess( void )
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersComponentSizeThresholdProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkComponentSizeThresholdProcess", createitkFiltersComponentSizeThresholdProcess);
}

//-------------------------------------------------------------------------------------------

void itkFiltersComponentSizeThresholdProcess::setParameter(int data, int channel)
{
    if (channel != 0)
        return;
    
    DTK_D(itkFiltersComponentSizeThresholdProcess);
    
    d->minimumSize = data;
}

//-------------------------------------------------------------------------------------------

int itkFiltersComponentSizeThresholdProcess::update ( void )
{
    DTK_D(itkFiltersComponentSizeThresholdProcess);
    
    if ( !d->input )
        return -1;

    QString id = d->input->identifier();

    qDebug() << "itkFiltersComponentSizeThresholdProcess, update : " << id;

    if ( id == "itkDataImageChar3" )
    {
        d->update<char>();
    }
    else if ( id == "itkDataImageUChar3" )
    {
        d->update<unsigned char>();
    }
    else if ( id == "itkDataImageShort3" )
    {
        d->update<short>();
    }
    else if ( id == "itkDataImageUShort3" )
    {
        d->update<unsigned short>();
    }
    else if ( id == "itkDataImageInt3" )
    {
        d->update<int>();
    }
    else if ( id == "itkDataImageUInt3" )
    {
        d->update<unsigned int>();
    }
    else if ( id == "itkDataImageLong3" )
    {
        d->update<long>();
    }
    else if ( id== "itkDataImageULong3" )
    {
        d->update<unsigned long>();
    }
    else if ( id== "itkDataImageFloat3" )
    {
        d->castToUInt3<float>();
        d->update<unsigned int>();
    }
    else if ( id== "itkDataImageDouble3" )
    {
        d->castToUInt3<double>();
        d->update<unsigned int>();
    }
    else
    {
        qDebug() << "itkFiltersComponentSizeThresholdProcess Error : pixel type not yet implemented ("
        << id
        << ")";
        return -1;
    }

    return EXIT_SUCCESS;
}


// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersComponentSizeThresholdProcess ( void )
{
    return new itkFiltersComponentSizeThresholdProcess;
}
