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

dtkAbstractProcess * createitkFiltersAddProcess ( void )
{
    return new itkFiltersAddProcess;
}
