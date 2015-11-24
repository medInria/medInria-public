/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkMorphologicalFiltersProcessBase.h>

#include <medAbstractDataFactory.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

#include <itkMorphologicalFiltersProcessBase_p.h>

itkMorphologicalFiltersProcessBase::itkMorphologicalFiltersProcessBase(itkMorphologicalFiltersProcessBase *parent) 
    : itkFiltersProcessBase(*new itkMorphologicalFiltersProcessBasePrivate(this), parent)
{
    DTK_D(itkMorphologicalFiltersProcessBase);
    
    d->input = NULL;
    d->output = NULL;

    d->radius[0] = 0;
    d->radius[1] = 0;
    d->radius[2] = 0;

    d->radiusMm[0] = 0;
    d->radiusMm[1] = 0;
    d->radiusMm[2] = 0;

    d->isRadiusInPixels = false;
    d->radiusInPixels = 0;
    
    d->description = "";
}

itkMorphologicalFiltersProcessBase::itkMorphologicalFiltersProcessBase(const itkMorphologicalFiltersProcessBase& other) 
    : itkFiltersProcessBase(*new itkMorphologicalFiltersProcessBasePrivate(*other.d_func()), other)
{
    
}

itkMorphologicalFiltersProcessBase::~itkMorphologicalFiltersProcessBase()
{
    
}

void itkMorphologicalFiltersProcessBase::setParameter(double data, int channel)
{
    if (channel <= 1)
    {
        DTK_D(itkMorphologicalFiltersProcessBase);

        d->radiusInPixels = data;

        d->radius[0] = data;
        d->radius[1] = data;
        d->radius[2] = data;

        if (channel == 1) // data is in pixels
        {
            d->isRadiusInPixels = true;
        }
        if (channel == 0) //data is in mm
        {
            d->isRadiusInPixels = false;
        }
    }
}
