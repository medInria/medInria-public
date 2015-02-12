/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersNormalizeProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractDataFactory.h>

#include <medMetaDataKeys.h>

#include <itkFiltersNormalizeProcess_p.h>

//-------------------------------------------------------------------------------------------

itkFiltersNormalizeProcess::itkFiltersNormalizeProcess(itkFiltersNormalizeProcess *parent) 
    : itkFiltersProcessBase(*new itkFiltersNormalizeProcessPrivate(this), parent)
{
    DTK_D(itkFiltersNormalizeProcess);
    
    d->filter = this;
    d->output = NULL;
    
    d->description = tr("ITK normalize filter");
}


itkFiltersNormalizeProcess::itkFiltersNormalizeProcess(const itkFiltersNormalizeProcess& other) 
    : itkFiltersProcessBase(*new itkFiltersNormalizeProcessPrivate(*other.d_func()), other)
{
}

//-------------------------------------------------------------------------------------------

itkFiltersNormalizeProcess::~itkFiltersNormalizeProcess( void )
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersNormalizeProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkNormalizeProcess", createitkFiltersNormalizeProcess);
}

//-------------------------------------------------------------------------------------------

int itkFiltersNormalizeProcess::update ( void )
{
    DTK_D(itkFiltersNormalizeProcess);
    
    if ( !d->input )
        return -1;

    QString id = d->input->identifier();

    qDebug() << "itkFiltersNormalizeProcess, update : " << id;

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

dtkAbstractProcess * createitkFiltersNormalizeProcess ( void )
{
    return new itkFiltersNormalizeProcess;
}
