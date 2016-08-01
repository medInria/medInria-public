/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersWindowingProcess.h>


#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractDataFactory.h>

#include <medMetaDataKeys.h>

#include <itkFiltersWindowingProcess_p.h>

//-------------------------------------------------------------------------------------------

itkFiltersWindowingProcess::itkFiltersWindowingProcess(itkFiltersWindowingProcess *parent) 
    : itkFiltersProcessBase(*new itkFiltersWindowingProcessPrivate(this), parent)
{
    DTK_D(itkFiltersWindowingProcess);
    
    d->filter = this;
    d->output = NULL;
    
    d->description = tr("ITK intensity windowing filter");
}


itkFiltersWindowingProcess::itkFiltersWindowingProcess(const itkFiltersWindowingProcess& other) 
    : itkFiltersProcessBase(*new itkFiltersWindowingProcessPrivate(*other.d_func()), other)
{
}

//-------------------------------------------------------------------------------------------

itkFiltersWindowingProcess::~itkFiltersWindowingProcess( void )
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersWindowingProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkWindowingProcess", createitkFiltersWindowingProcess);
}

//-------------------------------------------------------------------------------------------

void itkFiltersWindowingProcess::setParameter(double data, int channel)
{
    if (channel > 3)
        return;
 
    DTK_D(itkFiltersWindowingProcess);
    
    switch ( channel )
    {
    case 0:
        d->minimumIntensityValue = data;
        break;
    case 1:
        d->maximumIntensityValue = data;
        break;
    case 2:
        d->minimumOutputIntensityValue = data;
        break;
    case 3:
        d->maximumOutputIntensityValue = data;
        break;
    }
}

//-------------------------------------------------------------------------------------------

int itkFiltersWindowingProcess::update ( void )
{
    DTK_D(itkFiltersWindowingProcess);
    
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

dtkAbstractProcess * createitkFiltersWindowingProcess ( void )
{
    return new itkFiltersWindowingProcess;
}
