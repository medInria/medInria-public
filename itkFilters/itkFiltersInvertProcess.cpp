#include "itkFiltersInvertProcess.h"

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medMetaDataKeys.h>

#include "itkImage.h"
#include "itkCommand.h"
#include "itkInvertIntensityImageFilter.h"

class itkFiltersInvertProcessPrivate
{
public:
    itk::CStyleCommand::Pointer callback;
    itkFiltersInvertProcess *filter;
    
    dtkSmartPointer<dtkAbstractData> input;
    dtkSmartPointer<dtkAbstractData> output;
    
    template <class PixelType> void update ( void );
    
    static void eventCallback ( itk::Object *caller, const itk::EventObject& event, void *clientData);
};

void itkFiltersInvertProcessPrivate::eventCallback ( itk::Object* caller, const itk::EventObject& event, void* clientData )
{
    itkFiltersInvertProcessPrivate * source = reinterpret_cast<itkFiltersInvertProcessPrivate *> ( clientData );
    itk::ProcessObject * processObject = ( itk::ProcessObject* ) caller;

    if ( !source )
        qDebug() << "Source est null";

    source->filter->emitProgress((int) (processObject->GetProgress() * 100));
}

template <class PixelType> void itkFiltersInvertProcessPrivate::update ( void )
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::InvertIntensityImageFilter< ImageType, ImageType >  InvertFilterType;
    typename InvertFilterType::Pointer invertFilter = InvertFilterType::New();

    invertFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
    
    callback = itk::CStyleCommand::New();
    callback->SetClientData ( ( void * ) this );
    callback->SetCallback ( itkFiltersInvertProcessPrivate::eventCallback );

    invertFilter->AddObserver ( itk::ProgressEvent(), callback );

    invertFilter->Update();
    output->setData ( invertFilter->GetOutput() );

    //Set output description metadata
    QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
    newSeriesDescription += " invert filter";

    output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
}

//-------------------------------------------------------------------------------------------

itkFiltersInvertProcess::itkFiltersInvertProcess( void ) : itkFiltersProcessBase(), d(new itkFiltersInvertProcessPrivate)
{
    d->filter = this;
    d->output = NULL;
}

//-------------------------------------------------------------------------------------------

itkFiltersInvertProcess::~itkFiltersInvertProcess( void )
{
    delete d;
    d = NULL;
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
    
    d->output = dtkAbstractDataFactory::instance()->createSmartPointer(identifier);
    d->input = data;
}

//-------------------------------------------------------------------------------------------

int itkFiltersInvertProcess::update ( void )
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


dtkAbstractData * itkFiltersInvertProcess::output ( void )
{
    return ( d->output );
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersInvertProcess ( void )
{
    return new itkFiltersInvertProcess;
}

