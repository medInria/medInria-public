#include "itkFiltersNormalizeProcess.h"

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medMetaDataKeys.h>

#include "itkFiltersNormalizeProcess_p.h"

//-------------------------------------------------------------------------------------------

itkFiltersNormalizeProcess::itkFiltersNormalizeProcess(itkFiltersNormalizeProcess *parent) 
    : itkFiltersProcessBase(*new itkFiltersNormalizeProcessPrivate(this), parent)
{
    DTK_D(itkFiltersNormalizeProcess);
    
    d->filter = this;
    d->output = NULL;
}


itkFiltersNormalizeProcess::itkFiltersNormalizeProcess(const itkFiltersNormalizeProcess& other) 
    : itkFiltersProcessBase(*new itkFiltersNormalizeProcessPrivate(*other.d_func()), other)
{
}

itkFiltersNormalizeProcess& itkFiltersNormalizeProcess::operator = (const itkFiltersNormalizeProcess& other)
{
    itkFiltersProcessBase::operator=(other);

    DTK_D(itkFiltersNormalizeProcess);
    d->callback = other.d_func()->callback;
    d->filter = other.d_func()->filter;
    d->input = other.d_func()->input;
    d->output = other.d_func()->output;

    return *this;
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

QString itkFiltersNormalizeProcess::description() const
{
    return tr("ITK normalize filter");
}

//-------------------------------------------------------------------------------------------

void itkFiltersNormalizeProcess::setInput(dtkAbstractData *data)
{
    if (!data)
        return;
    
    QString identifier = data->identifier();
    
    DTK_D(itkFiltersNormalizeProcess);
    
    d->output = dtkAbstractDataFactory::instance()->createSmartPointer(identifier);
    d->input = data;
}

//-------------------------------------------------------------------------------------------

int itkFiltersNormalizeProcess::update ( void )
{
    DTK_D(itkFiltersNormalizeProcess);
    
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


dtkAbstractData * itkFiltersNormalizeProcess::output ( void )
{
    DTK_D(itkFiltersNormalizeProcess);
    
    return ( d->output );
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersNormalizeProcess ( void )
{
    return new itkFiltersNormalizeProcess;
}
