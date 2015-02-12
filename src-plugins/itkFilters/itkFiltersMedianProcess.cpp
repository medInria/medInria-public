/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersMedianProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractDataFactory.h>

#include <medMetaDataKeys.h>

#include <itkFiltersMedianProcess_p.h>

//-------------------------------------------------------------------------------------------

itkFiltersMedianProcess::itkFiltersMedianProcess(itkFiltersMedianProcess *parent) 
    : itkFiltersProcessBase(*new itkFiltersMedianProcessPrivate(this), parent)
{
    DTK_D(itkFiltersMedianProcess);
    
    d->filter = this;
    d->output = NULL;
    
     d->description = tr("ITK median filter");
}


itkFiltersMedianProcess::itkFiltersMedianProcess(const itkFiltersMedianProcess& other) 
    : itkFiltersProcessBase(*new itkFiltersMedianProcessPrivate(*other.d_func()), other)
{
}

//-------------------------------------------------------------------------------------------

itkFiltersMedianProcess::~itkFiltersMedianProcess( void )
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersMedianProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkMedianProcess", createitkFiltersMedianProcess);
}

//-------------------------------------------------------------------------------------------

int itkFiltersMedianProcess::update ( void )
{
    DTK_D(itkFiltersMedianProcess);
    
    if ( !d->input )
        return -1;

    QString id = d->input->identifier();

    qDebug() << "itkFiltersMedianProcess, update : " << id;

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

dtkAbstractProcess * createitkFiltersMedianProcess ( void )
{
    return new itkFiltersMedianProcess;
}
