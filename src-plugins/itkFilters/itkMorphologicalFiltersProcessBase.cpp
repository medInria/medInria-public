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
    : medAbstractProcess(*new itkMorphologicalFiltersProcessBasePrivate(this), parent)
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
    : medAbstractProcess(*new itkMorphologicalFiltersProcessBasePrivate(*other.d_func()), other)
{
    
}


itkMorphologicalFiltersProcessBase& itkMorphologicalFiltersProcessBase::operator = (const itkMorphologicalFiltersProcessBase& other)
{
    return *this;
}

itkMorphologicalFiltersProcessBase::~itkMorphologicalFiltersProcessBase()
{
    
}

void itkMorphologicalFiltersProcessBase::emitProgress(int progress)
{
    emit progressed(progress);
}

QString itkMorphologicalFiltersProcessBase::description()
{
    DTK_D(itkMorphologicalFiltersProcessBase);
    
    return d->description;
}


void itkMorphologicalFiltersProcessBase::setInput(medAbstractData *data, int channel)
{
    if (!data)
        return;
 
    DTK_D(itkMorphologicalFiltersProcessBase);
    
    QString identifier = data->identifier();
    
    d->output = dynamic_cast<medAbstractImageData*> (medAbstractDataFactory::instance()->create(identifier));
    d->input = dynamic_cast<medAbstractImageData*> (data);
}

void itkMorphologicalFiltersProcessBase::setParameter(double data, int channel)
{
	if (channel > 1)
        return;
	
    DTK_D(itkMorphologicalFiltersProcessBase);
	
    d->radiusInPixels = data;

    d->radius[0] = data;
    d->radius[1] = data;
    d->radius[2] = data;
	
    if (channel == 1) // data is in pixels
        d->isRadiusInPixels = true;
	
    if (channel == 0) //data is in mm
        d->isRadiusInPixels = false;
}

medAbstractData * itkMorphologicalFiltersProcessBase::output ( void )
{
    DTK_D(itkMorphologicalFiltersProcessBase);
    
    return ( d->output );
}

