#include "itkFiltersMedianProcess.h"

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medMetaDataKeys.h>

#include "itkImage.h"
#include "itkCommand.h"
#include "itkMedianImageFilter.h"

class itkFiltersMedianProcessPrivate
{
public:
    itk::CStyleCommand::Pointer callback;
    itkFiltersMedianProcess *filter;
    
    dtkSmartPointer<dtkAbstractData> input;
    dtkSmartPointer<dtkAbstractData> output;
    
    template <class PixelType> void update ( void );
    
    static void eventCallback ( itk::Object *caller, const itk::EventObject& event, void *clientData);
};

void itkFiltersMedianProcessPrivate::eventCallback ( itk::Object* caller, const itk::EventObject& event, void* clientData )
{
    itkFiltersMedianProcessPrivate * source = reinterpret_cast<itkFiltersMedianProcessPrivate *> ( clientData );
    itk::ProcessObject * processObject = ( itk::ProcessObject* ) caller;

    if ( !source )
        qDebug() << "Source est null";

    source->filter->emitProgress((int) (processObject->GetProgress() * 100));
}

template <class PixelType> void itkFiltersMedianProcessPrivate::update ( void )
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::MedianImageFilter< ImageType, ImageType >  MedianFilterType;
    typename MedianFilterType::Pointer medianFilter = MedianFilterType::New();

    medianFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
    
    callback = itk::CStyleCommand::New();
    callback->SetClientData ( ( void * ) this );
    callback->SetCallback ( itkFiltersMedianProcessPrivate::eventCallback );

    medianFilter->AddObserver ( itk::ProgressEvent(), callback );

    medianFilter->Update();
    output->setData ( medianFilter->GetOutput() );

    //Set output description metadata
    QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
    newSeriesDescription += " median filter";

    output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
}

//-------------------------------------------------------------------------------------------

itkFiltersMedianProcess::itkFiltersMedianProcess( void ) : itkFiltersProcessBase(), d(new itkFiltersMedianProcessPrivate)
{
    d->filter = this;
    d->output = NULL;
}

//-------------------------------------------------------------------------------------------

itkFiltersMedianProcess::~itkFiltersMedianProcess( void )
{
    delete d;
    d = NULL;
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
    
    d->output = dtkAbstractDataFactory::instance()->createSmartPointer(identifier);
    d->input = data;
}

//-------------------------------------------------------------------------------------------

int itkFiltersMedianProcess::update ( void )
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


dtkAbstractData * itkFiltersMedianProcess::output ( void )
{
    return ( d->output );
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersMedianProcess ( void )
{
    return new itkFiltersMedianProcess;
}
