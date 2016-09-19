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
    
    int res = DTK_FAILURE;

    if ( d->input )
    {
        QString id = d->input->identifier();

        if ( id == "itkDataImageChar3" )
        {
            res = d->update<char>();
        }
        else if ( id == "itkDataImageUChar3" )
        {
            res = d->update<unsigned char>();
        }
        else if ( id == "itkDataImageShort3" )
        {
            res = d->update<short>();
        }
        else if ( id == "itkDataImageUShort3" )
        {
            res = d->update<unsigned short>();
        }
        else if ( id == "itkDataImageInt3" )
        {
            res = d->update<int>();
        }
        else if ( id == "itkDataImageUInt3" )
        {
            res = d->update<unsigned int>();
        }
        else if ( id == "itkDataImageLong3" )
        {
            res = d->update<long>();
        }
        else if ( id== "itkDataImageULong3" )
        {
            res = d->update<unsigned long>();
        }
        else if ( id== "itkDataImageFloat3" )
        {
            if (d->castToUInt3<float>())
            {
                res = d->update<unsigned int>();
            }
        }
        else if ( id== "itkDataImageDouble3" )
        {
            if(d->castToUInt3<double>())
            {
                return d->update<unsigned int>();
            }
        }
        else
        {
            res = medAbstractProcess::PIXEL_TYPE;
        }
    }

    return res;
}


// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersComponentSizeThresholdProcess ( void )
{
    return new itkFiltersComponentSizeThresholdProcess;
}
