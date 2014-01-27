/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkFiltersProcessBase.h"

#include <medAbstractDataFactory.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

#include "itkFiltersProcessBase_p.h"

itkFiltersProcessBase::itkFiltersProcessBase(itkFiltersProcessBase *parent) 
    : dtkAbstractProcess(*new itkFiltersProcessBasePrivate(this), parent)
{
    DTK_D(itkFiltersProcessBase);
    
    d->input = NULL;
    d->output = NULL;
    
    d->description = "";
}


itkFiltersProcessBase::itkFiltersProcessBase(const itkFiltersProcessBase& other) 
    : dtkAbstractProcess(*new itkFiltersProcessBasePrivate(*other.d_func()), other)
{
    
}


itkFiltersProcessBase& itkFiltersProcessBase::operator = (const itkFiltersProcessBase& other)
{
    return *this;
}

itkFiltersProcessBase::~itkFiltersProcessBase()
{
    
}

void itkFiltersProcessBase::emitProgress(int progress)
{
    emit progressed(progress);
}

QString itkFiltersProcessBase::description()
{
    DTK_D(itkFiltersProcessBase);
    
    return d->description;
}


void itkFiltersProcessBase::setInput(medAbstractData *data)
{
    if (!data)
        return;
 
    DTK_D(itkFiltersProcessBase);
    
    QString identifier = data->identifier();
    
    d->output = medAbstractDataFactory::instance()->createSmartPointer(identifier);
    d->input = data;
}

medAbstractData * itkFiltersProcessBase::output ( void )
{
    DTK_D(itkFiltersProcessBase);
    
    return ( d->output );
}

