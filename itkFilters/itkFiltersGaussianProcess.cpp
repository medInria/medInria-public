#include "itkFiltersGaussianProcess.h"

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medMetaDataKeys.h>

#include "itkImage.h"
#include "itkCommand.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"

#include "itkFiltersGaussianProcess_p.h"

itkFiltersGaussianProcess::itkFiltersGaussianProcess(itkFiltersGaussianProcess *parent) 
    : itkFiltersProcessBase(*new itkFiltersGaussianProcessPrivate(this), parent)
{
    DTK_D(itkFiltersGaussianProcess);
    
    d->filter = this;
    d->output = NULL; 
}


itkFiltersGaussianProcess::itkFiltersGaussianProcess(const itkFiltersGaussianProcess& other) 
    : itkFiltersProcessBase(*new itkFiltersGaussianProcessPrivate(*other.d_func()), other)
{
}

itkFiltersGaussianProcess& itkFiltersGaussianProcess::operator = (const itkFiltersGaussianProcess& other)
{
    itkFiltersProcessBase::operator=(other);

    DTK_D(itkFiltersGaussianProcess);
    d->callback = other.d_func()->callback;
    d->filter = other.d_func()->filter;
    d->input = other.d_func()->input;
    d->output = other.d_func()->output;

    return *this;
}

itkFiltersGaussianProcess::~itkFiltersGaussianProcess( void )
{
    DTK_D(itkFiltersGaussianProcess);
    
    delete d;
}

//-------------------------------------------------------------------------------------------

bool itkFiltersGaussianProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkGaussianProcess", createitkFiltersGaussianProcess);
}

//-------------------------------------------------------------------------------------------

QString itkFiltersGaussianProcess::description() const
{
    return tr("ITK gaussian filter");
}

//-------------------------------------------------------------------------------------------

void itkFiltersGaussianProcess::setInput(dtkAbstractData *data)
{
    if (!data)
        return;
    
    DTK_D(itkFiltersGaussianProcess);
    
    QString identifier = data->identifier();
    
    d->output = dtkAbstractDataFactory::instance()->createSmartPointer(identifier);
    d->input = data;
}

//-------------------------------------------------------------------------------------------

void itkFiltersGaussianProcess::setParameter(double data, int channel)
{
    if (channel != 0)
        return;
    
    DTK_D(itkFiltersGaussianProcess);
    
    d->sigma = data;
}

//-------------------------------------------------------------------------------------------

int itkFiltersGaussianProcess::update ( void )
{
    DTK_D(itkFiltersGaussianProcess);
    
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


dtkAbstractData * itkFiltersGaussianProcess::output ( void )
{
    DTK_D(itkFiltersGaussianProcess);
    
    return ( d->output );
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersGaussianProcess ( void )
{
    return new itkFiltersGaussianProcess;
}
