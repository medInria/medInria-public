/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersAddProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractDataFactory.h>

#include <medMetaDataKeys.h>

#include <itkFiltersAddProcess_p.h>

//-------------------------------------------------------------------------------------------

itkFiltersAddProcess::itkFiltersAddProcess(itkFiltersAddProcess *parent) 
    : itkFiltersProcessBase(*new itkFiltersAddProcessPrivate(this), parent)
{
    DTK_D(itkFiltersAddProcess);
    
    d->filter = this;
    d->output = NULL;
    
    d->description = tr("ITK add constant filter");
}


itkFiltersAddProcess::itkFiltersAddProcess(const itkFiltersAddProcess& other) 
    : itkFiltersProcessBase(*new itkFiltersAddProcessPrivate(*other.d_func()), other)
{
}

//-------------------------------------------------------------------------------------------

itkFiltersAddProcess::~itkFiltersAddProcess( void )
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersAddProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkAddProcess", createitkFiltersAddProcess);
}

//-------------------------------------------------------------------------------------------

void itkFiltersAddProcess::setParameter(double data, int channel)
{
    if (channel != 0)
        return;
    
    DTK_D(itkFiltersAddProcess);
    
    d->addValue = data;
}

//-------------------------------------------------------------------------------------------

int itkFiltersAddProcess::update ( void )
{
    DTK_D(itkFiltersAddProcess);
    
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
        else if ( id == "itkDataImageFloat3" )
        {
            res = d->update<float>();
        }
        else if ( id == "itkDataImageDouble3" )
        {
            res = d->update<double>();
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

dtkAbstractProcess * createitkFiltersAddProcess ( void )
{
    return new itkFiltersAddProcess;
}
