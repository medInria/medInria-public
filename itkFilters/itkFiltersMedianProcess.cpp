#include "itkFiltersMedianProcess.h"

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medMetaDataKeys.h>

#include "itkFiltersMedianProcess_p.h"

//-------------------------------------------------------------------------------------------

itkFiltersMedianProcess::itkFiltersMedianProcess(itkFiltersMedianProcess *parent) 
    : itkFiltersProcessBase(*new itkFiltersMedianProcessPrivate(this), parent)
{
    DTK_D(itkFiltersMedianProcess);
    
    d->filter = this;
    d->output = NULL;
    d->addValue = 100.0;    
}


itkFiltersMedianProcess::itkFiltersMedianProcess(const itkFiltersMedianProcess& other) 
    : itkFiltersProcessBase(*new itkFiltersMedianProcessPrivate(*other.d_func()), other)
{
}

itkFiltersMedianProcess& itkFiltersMedianProcess::operator = (const itkFiltersMedianProcess& other)
{
    itkFiltersProcessBase::operator=(other);

    DTK_D(itkFiltersMedianProcess);
    d->callback = other.d_func()->callback;
    d->filter = other.d_func()->filter;
    d->input = other.d_func()->input;
    d->output = other.d_func()->output;

    return *this;
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

QString itkFiltersMedianProcess::description() const
{
    return tr("ITK median filter");
}

//-------------------------------------------------------------------------------------------

void itkFiltersMedianProcess::setInput(dtkAbstractData *data)
{
    if (!data)
        return;
    
    QString identifier = data->identifier();
    
    DTK_D(itkFiltersMedianProcess);
    
    d->output = dtkAbstractDataFactory::instance()->createSmartPointer(identifier);
    d->input = data;
}

//-------------------------------------------------------------------------------------------

int itkFiltersMedianProcess::update ( void )
{
    DTK_D(itkFiltersMedianProcess);
    
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


dtkAbstractData * itkFiltersMedianProcess::output ( void )
{
    DTK_D(itkFiltersMedianProcess);
    
    return ( d->output );
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersMedianProcess ( void )
{
    return new itkFiltersMedianProcess;
}
