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
    
    if ( !d->input )
        return -1;

    QString id = d->input->identifier();

    qDebug() << "itkFiltersWindowingProcess, update : " << id;

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
    else if ( id == "itkDataImageFloat3" )
    {
        d->update<float>();
    }
    else if ( id == "itkDataImageDouble3" )
    {
        d->update<double>();
    }
    else
    {
        qDebug() << "itkFiltersWindowingProcess Error : pixel type not yet implemented ("
        << id
        << ")";
        return -1;
    }

    return EXIT_SUCCESS;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersWindowingProcess ( void )
{
    return new itkFiltersWindowingProcess;
}
