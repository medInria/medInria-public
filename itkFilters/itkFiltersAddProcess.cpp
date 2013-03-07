#include "itkFiltersAddProcess.h"

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medMetaDataKeys.h>

#include "itkImage.h"
#include "itkCommand.h"
#include "itkAddConstantToImageFilter.h"

#include "itkFiltersAddProcess_p.h"

//-------------------------------------------------------------------------------------------

itkFiltersAddProcess::itkFiltersAddProcess(itkFiltersAddProcess *parent) 
    : itkFiltersProcessBase(*new itkFiltersAddProcessPrivate(this), parent)
{
    DTK_D(itkFiltersAddProcess);
    
    d->filter = this;
    d->output = NULL;
    d->addValue = 100.0;    
}


itkFiltersAddProcess::itkFiltersAddProcess(const itkFiltersAddProcess& other) 
    : itkFiltersProcessBase(*new itkFiltersAddProcessPrivate(*other.d_func()), other)
{
}

itkFiltersAddProcess& itkFiltersAddProcess::operator = (const itkFiltersAddProcess& other)
{
    itkFiltersProcessBase::operator=(other);

    DTK_D(itkFiltersAddProcess);
    d->callback = other.d_func()->callback;
    d->filter = other.d_func()->filter;
    d->input = other.d_func()->input;
    d->output = other.d_func()->output;

    return *this;
}

//-------------------------------------------------------------------------------------------

itkFiltersAddProcess::~itkFiltersAddProcess( void )
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersAddProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkAddProcess", createitkFiltersAddProcess);
}

//-------------------------------------------------------------------------------------------

QString itkFiltersAddProcess::description() const
{
    return tr("ITK add constant filter");
}

//-------------------------------------------------------------------------------------------

void itkFiltersAddProcess::setInput(dtkAbstractData *data)
{
    if (!data)
        return;
 
    DTK_D(itkFiltersAddProcess);
    
    QString identifier = data->identifier();
    
    d->output = dtkAbstractDataFactory::instance()->createSmartPointer(identifier);
    d->input = data;
}

//-------------------------------------------------------------------------------------------

void itkFiltersAddProcess::setParameter(double data, int channel)
{
    if (channel != 0)
        return;
    
    DTK_D(itkFiltersAddProcess);
    
    d->addValue = data;
}

//-------------------------------------------------------------------------------------------

int itkFiltersAddProcess::update ( void )
{
    DTK_D(itkFiltersAddProcess);
    
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


dtkAbstractData * itkFiltersAddProcess::output ( void )
{
    DTK_D(itkFiltersAddProcess);
    
    return ( d->output );
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersAddProcess ( void )
{
    return new itkFiltersAddProcess;
}
