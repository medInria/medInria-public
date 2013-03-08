#include "itkFiltersSubtractProcess.h"

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medMetaDataKeys.h>


#include "itkFiltersSubtractProcess_p.h"

//-------------------------------------------------------------------------------------------

itkFiltersSubtractProcess::itkFiltersSubtractProcess(itkFiltersSubtractProcess *parent) 
    : itkFiltersProcessBase(*new itkFiltersSubtractProcessPrivate(this), parent)
{
    DTK_D(itkFiltersSubtractProcess);
    
    d->filter = this;
    d->output = NULL;
}


itkFiltersSubtractProcess::itkFiltersSubtractProcess(const itkFiltersSubtractProcess& other) 
    : itkFiltersProcessBase(*new itkFiltersSubtractProcessPrivate(*other.d_func()), other)
{
}

itkFiltersSubtractProcess& itkFiltersSubtractProcess::operator = (const itkFiltersSubtractProcess& other)
{
    itkFiltersProcessBase::operator=(other);

    DTK_D(itkFiltersSubtractProcess);
    d->callback = other.d_func()->callback;
    d->filter = other.d_func()->filter;
    d->input = other.d_func()->input;
    d->output = other.d_func()->output;

    return *this;
}

//-------------------------------------------------------------------------------------------

itkFiltersSubtractProcess::~itkFiltersSubtractProcess( void )
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersSubtractProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkSubtractProcess", createitkFiltersSubtractProcess);
}

//-------------------------------------------------------------------------------------------

QString itkFiltersSubtractProcess::description() const
{
    return tr("ITK subtract constant filter");
}

//-------------------------------------------------------------------------------------------

void itkFiltersSubtractProcess::setInput(dtkAbstractData *data)
{
    if (!data)
        return;
    
    QString identifier = data->identifier();
    
    DTK_D(itkFiltersSubtractProcess);
    
    d->output = dtkAbstractDataFactory::instance()->createSmartPointer(identifier);
    d->input = data;
}

//-------------------------------------------------------------------------------------------

void itkFiltersSubtractProcess::setParameter(double data, int channel)
{
    if (channel != 0)
        return;
    
    DTK_D(itkFiltersSubtractProcess);
    
    d->subtractValue = data;
}

//-------------------------------------------------------------------------------------------

int itkFiltersSubtractProcess::update ( void )
{
    DTK_D(itkFiltersSubtractProcess);
    
    if ( !d->input )
        return -1;

    QString id = d->input->identifier();

    qDebug() << "itkFilters, update : " << id;

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

//-------------------------------------------------------------------------------------------


dtkAbstractData * itkFiltersSubtractProcess::output ( void )
{
    DTK_D(itkFiltersSubtractProcess);
    
    return ( d->output );
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersSubtractProcess ( void )
{
    return new itkFiltersSubtractProcess;
}


























