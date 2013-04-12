#include "itkFiltersProcessBase.h"

#include <dtkCore/dtkAbstractDataFactory.h>
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


void itkFiltersProcessBase::setInput(dtkAbstractData *data)
{
    if (!data)
        return;
 
    DTK_D(itkFiltersProcessBase);
    
    QString identifier = data->identifier();
    
    d->output = dtkAbstractDataFactory::instance()->createSmartPointer(identifier);
    d->input = data;
}

dtkAbstractData * itkFiltersProcessBase::output ( void )
{
    DTK_D(itkFiltersProcessBase);
    
    return ( d->output );
}

