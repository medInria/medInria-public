/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersGaussianProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractDataFactory.h>

#include <medMetaDataKeys.h>

#include <itkImage.h>
#include <itkCommand.h>
#include <itkSmoothingRecursiveGaussianImageFilter.h>

#include <itkExceptionObject.h>
#include <itkFiltersGaussianProcess_p.h>

itkFiltersGaussianProcess::itkFiltersGaussianProcess(itkFiltersGaussianProcess *parent) 
    : itkFiltersProcessBase(*new itkFiltersGaussianProcessPrivate(this), parent)
{
    DTK_D(itkFiltersGaussianProcess);
    
    d->filter = this;
    d->output = NULL; 
    
     d->description = tr("ITK gaussian filter");
}


itkFiltersGaussianProcess::itkFiltersGaussianProcess(const itkFiltersGaussianProcess& other) 
    : itkFiltersProcessBase(*new itkFiltersGaussianProcessPrivate(*other.d_func()), other)
{
}

itkFiltersGaussianProcess::~itkFiltersGaussianProcess( void )
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersGaussianProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkGaussianProcess", createitkFiltersGaussianProcess);
}

//-------------------------------------------------------------------------------------------

void itkFiltersGaussianProcess::setParameter(double data, int channel)
{
    if (channel != 0)
        return;
    
    DTK_D(itkFiltersGaussianProcess);
    
    d->sigma = data;
}

//-------------------------------------------------------------------------------------------

int itkFiltersGaussianProcess::update ( void )
{
    DTK_D(itkFiltersGaussianProcess);
    
    if ( !d->input )
        return -1;

    QString id = d->input->identifier();

    qDebug() << "itkFiltersGaussianProcess, update : " << id;

    try
    {
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
    }
    catch (itk::ExceptionObject &e)
    {
        emit failure();
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersGaussianProcess ( void )
{
    return new itkFiltersGaussianProcess;
}
