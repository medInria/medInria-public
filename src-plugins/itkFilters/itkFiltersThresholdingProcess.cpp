/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersThresholdingProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractDataFactory.h>

#include <medMetaDataKeys.h>

#include <itkImage.h>
#include <itkCommand.h>
#include <itkSmoothingRecursiveGaussianImageFilter.h>

#include <itkExceptionObject.h>
#include <itkFiltersThresholdingProcess_p.h>

itkFiltersThresholdingProcess::itkFiltersThresholdingProcess(itkFiltersThresholdingProcess *parent) 
    : itkFiltersProcessBase(*new itkFiltersThresholdingProcessPrivate(this), parent)
{
    DTK_D(itkFiltersThresholdingProcess);
    
    d->filter = this;
    d->output = NULL; 
    d->outsideValue =0;
    d->threshold = 0;
    d->comparisonOperator = true;
    
    d->description = tr("ITK thresholding filter");
}


itkFiltersThresholdingProcess::itkFiltersThresholdingProcess(const itkFiltersThresholdingProcess& other) 
    : itkFiltersProcessBase(*new itkFiltersThresholdingProcessPrivate(*other.d_func()), other)
{
}

itkFiltersThresholdingProcess::~itkFiltersThresholdingProcess( void )
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersThresholdingProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkThresholdingProcess", createitkFiltersThresholdingProcess);
}

//-------------------------------------------------------------------------------------------

void itkFiltersThresholdingProcess::setParameter(double data, int channel)
{
    if (channel > 2)
        return;

    DTK_D(itkFiltersThresholdingProcess);
    if (channel == 0)
        d->threshold = data;
    if (channel == 1)
        d->outsideValue = data;
    if (channel == 2)
        d->comparisonOperator = (bool)data;
}

//-------------------------------------------------------------------------------------------

int itkFiltersThresholdingProcess::update ( void )
{
    DTK_D(itkFiltersThresholdingProcess);
    
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

dtkAbstractProcess * createitkFiltersThresholdingProcess ( void )
{
    return new itkFiltersThresholdingProcess;
}
