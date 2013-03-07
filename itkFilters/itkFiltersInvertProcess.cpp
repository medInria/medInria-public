#include "itkFiltersInvertProcess.h"

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medMetaDataKeys.h>

#include "itkFiltersInvertProcess_p.h"

itkFiltersInvertProcess::itkFiltersInvertProcess(itkFiltersInvertProcess *parent) 
    : itkFiltersProcessBase(*new itkFiltersInvertProcessPrivate(this), parent)
{
    DTK_D(itkFiltersInvertProcess);
    
    d->filter = this;
    d->output = NULL;
}


itkFiltersInvertProcess::itkFiltersInvertProcess(const itkFiltersInvertProcess& other) 
    : itkFiltersProcessBase(*new itkFiltersInvertProcessPrivate(*other.d_func()), other)
{
}

itkFiltersInvertProcess& itkFiltersInvertProcess::operator = (const itkFiltersInvertProcess& other)
{
    itkFiltersProcessBase::operator=(other);

    DTK_D(itkFiltersInvertProcess);
    d->callback = other.d_func()->callback;
    d->filter = other.d_func()->filter;
    d->input = other.d_func()->input;
    d->output = other.d_func()->output;

    return *this;
}

//-------------------------------------------------------------------------------------------

itkFiltersInvertProcess::~itkFiltersInvertProcess( void )
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersInvertProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkInvertProcess", createitkFiltersInvertProcess);
}

//-------------------------------------------------------------------------------------------

QString itkFiltersInvertProcess::description() const
{
    return tr("ITK invert intensity filter");
}

//-------------------------------------------------------------------------------------------

void itkFiltersInvertProcess::setInput(dtkAbstractData *data)
{
    if (!data)
        return;
    
    QString identifier = data->identifier();
    
    DTK_D(itkFiltersInvertProcess);
    d->output = dtkAbstractDataFactory::instance()->createSmartPointer(identifier);
    d->input = data;
}

//-------------------------------------------------------------------------------------------

int itkFiltersInvertProcess::update ( void )
{
    DTK_D(itkFiltersInvertProcess);
    
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


dtkAbstractData * itkFiltersInvertProcess::output ( void )
{
    DTK_D(itkFiltersInvertProcess);
    
    return ( d->output );
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersInvertProcess ( void )
{
    return new itkFiltersInvertProcess;
}

