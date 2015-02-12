/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersOpenProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractDataFactory.h>

#include <medMetaDataKeys.h>

#include <itkFiltersOpenProcess_p.h>

//-------------------------------------------------------------------------------------------

itkFiltersOpenProcess::itkFiltersOpenProcess(itkFiltersOpenProcess *parent) 
    : itkFiltersProcessBase(*new itkFiltersOpenProcessPrivate(this), parent)
{
    DTK_D(itkFiltersOpenProcess);
    
    d->filter = this;
    d->output = NULL;
    d->radius = 5;

    d->description = tr("ITK Open filter");
}


itkFiltersOpenProcess::itkFiltersOpenProcess(const itkFiltersOpenProcess& other) 
    : itkFiltersProcessBase(*new itkFiltersOpenProcessPrivate(*other.d_func()), other)
{
}

//-------------------------------------------------------------------------------------------

itkFiltersOpenProcess::~itkFiltersOpenProcess( void )
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersOpenProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkOpenProcess", createitkFiltersOpenProcess);
}

//-------------------------------------------------------------------------------------------

void itkFiltersOpenProcess::setParameter(double data, int channel)
{
    if (channel != 0)
        return;
    
    DTK_D(itkFiltersOpenProcess);
    d->radius = data;

}

//-------------------------------------------------------------------------------------------

int itkFiltersOpenProcess::update ( void )
{
    DTK_D(itkFiltersOpenProcess);
    
    if ( !d->input )
        return -1;

    QString id = d->input->identifier();

    qDebug() << "itkFiltersOpenProcess, update : " << id;

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
        qDebug() << "Error : pixel type not yet implemented ("
        << id
        << ")";
        return -1;
    }

    return EXIT_SUCCESS;
}


// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersOpenProcess ( void )
{
    return new itkFiltersOpenProcess;
}
