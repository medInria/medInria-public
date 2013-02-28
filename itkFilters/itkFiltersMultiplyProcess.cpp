#include "itkFiltersMultiplyProcess.h"

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medMetaDataKeys.h>

#include "itkImage.h"
#include "itkCommand.h"
#include "itkMultiplyByConstantImageFilter.h"

class itkFiltersMultiplyProcessPrivate
{
public:
    itk::CStyleCommand::Pointer callback;
    itkFiltersMultiplyProcess *filter;
    
    dtkSmartPointer<dtkAbstractData> input;
    dtkSmartPointer<dtkAbstractData> output;
    
    double multiplyFactor;
    
    template <class PixelType> void update ( void );
    
    static void eventCallback ( itk::Object *caller, const itk::EventObject& event, void *clientData);
};

void itkFiltersMultiplyProcessPrivate::eventCallback ( itk::Object* caller, const itk::EventObject& event, void* clientData )
{
    itkFiltersMultiplyProcessPrivate * source = reinterpret_cast<itkFiltersMultiplyProcessPrivate *> ( clientData );
    itk::ProcessObject * processObject = ( itk::ProcessObject* ) caller;

    if ( !source )
        qDebug() << "Source est null";

    source->filter->emitProgress((int) (processObject->GetProgress() * 100));
}

template <class PixelType> void itkFiltersMultiplyProcessPrivate::update ( void )
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::MultiplyByConstantImageFilter< ImageType, double, ImageType >  MultiplyFilterType;
    typename MultiplyFilterType::Pointer multiplyFilter = MultiplyFilterType::New();

    multiplyFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
    multiplyFilter->SetConstant ( multiplyFactor );
    
    callback = itk::CStyleCommand::New();
    callback->SetClientData ( ( void * ) this );
    callback->SetCallback ( itkFiltersMultiplyProcessPrivate::eventCallback );

    multiplyFilter->AddObserver ( itk::ProgressEvent(), callback );

    multiplyFilter->Update();
    output->setData ( multiplyFilter->GetOutput() );

    //Set output description metadata
    QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
    newSeriesDescription += " add filter (" + QString::number(multiplyFactor) + ")";

    output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
}

//-------------------------------------------------------------------------------------------

itkFiltersMultiplyProcess::itkFiltersMultiplyProcess( void ) : itkFiltersProcessBase(), d(new itkFiltersMultiplyProcessPrivate)
{
    d->filter = this;
    d->output = NULL;
}

//-------------------------------------------------------------------------------------------

itkFiltersMultiplyProcess::~itkFiltersMultiplyProcess( void )
{
    delete d;
    d = NULL;
}

//-------------------------------------------------------------------------------------------

bool itkFiltersMultiplyProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkMultiplyProcess", createitkFiltersMultiplyProcess);
}

//-------------------------------------------------------------------------------------------

QString itkFiltersMultiplyProcess::description() const
{
    return tr("ITK multiply by constant filter");
}

//-------------------------------------------------------------------------------------------

void itkFiltersMultiplyProcess::setInput(dtkAbstractData *data)
{
    if (!data)
        return;
    
    QString identifier = data->identifier();
    
    d->output = dtkAbstractDataFactory::instance()->createSmartPointer(identifier);
    d->input = data;
}

//-------------------------------------------------------------------------------------------

void itkFiltersMultiplyProcess::setParameter(double data, int channel)
{
    if (channel != 0)
        return;
    
    d->multiplyFactor = data;
}

//-------------------------------------------------------------------------------------------

int itkFiltersMultiplyProcess::update ( void )
{
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


dtkAbstractData * itkFiltersMultiplyProcess::output ( void )
{
    return ( d->output );
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersMultiplyProcess ( void )
{
    return new itkFiltersMultiplyProcess;
}
