/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersMultiplyProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractDataFactory.h>

#include <medMetaDataKeys.h>
#include <itkFiltersMultiplyProcess_p.h>

//-------------------------------------------------------------------------------------------

itkFiltersMultiplyProcess::itkFiltersMultiplyProcess(itkFiltersMultiplyProcess *parent) 
    : itkFiltersProcessBase(*new itkFiltersMultiplyProcessPrivate(this), parent)
{
    DTK_D(itkFiltersMultiplyProcess);
    
    d->filter = this;
    d->output = NULL;
    
     d->description = tr("ITK multiply by constant filter");
}


itkFiltersMultiplyProcess::itkFiltersMultiplyProcess(const itkFiltersMultiplyProcess& other) 
    : itkFiltersProcessBase(*new itkFiltersMultiplyProcessPrivate(*other.d_func()), other)
{
}

//-------------------------------------------------------------------------------------------

itkFiltersMultiplyProcess::~itkFiltersMultiplyProcess( void )
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersMultiplyProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkMultiplyProcess", createitkFiltersMultiplyProcess);
}

//-------------------------------------------------------------------------------------------

void itkFiltersMultiplyProcess::setParameter(double data, int channel)
{
    if (channel != 0)
        return;
 
    DTK_D(itkFiltersMultiplyProcess);
    
    d->multiplyFactor = data;
}

//-------------------------------------------------------------------------------------------

int itkFiltersMultiplyProcess::update ( void )
{
    DTK_D(itkFiltersMultiplyProcess);
    
    if ( d->input )
    {
        QString id = d->input->identifier();

        if ( id == "itkDataImageChar3" )
        {
            return d->update<char>();
        }
        else if ( id == "itkDataImageUChar3" )
        {
            return d->update<unsigned char>();
        }
        else if ( id == "itkDataImageShort3" )
        {
            return d->update<short>();
        }
        else if ( id == "itkDataImageUShort3" )
        {
            return d->update<unsigned short>();
        }
        else if ( id == "itkDataImageInt3" )
        {
            return d->update<int>();
        }
        else if ( id == "itkDataImageUInt3" )
        {
            return d->update<unsigned int>();
        }
        else if ( id == "itkDataImageLong3" )
        {
            return d->update<long>();
        }
        else if ( id== "itkDataImageULong3" )
        {
            return d->update<unsigned long>();
        }
        else if ( id == "itkDataImageFloat3" )
        {
            return d->update<float>();
        }
        else if ( id == "itkDataImageDouble3" )
        {
            return d->update<double>();
        }
        else
        {
            qDebug() << description()
                     <<", Error : pixel type not yet implemented ("
                    << id
                    << ")";
        }
    }

    return medAbstractProcess::DIMENSION_3D;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersMultiplyProcess ( void )
{
    return new itkFiltersMultiplyProcess;
}
