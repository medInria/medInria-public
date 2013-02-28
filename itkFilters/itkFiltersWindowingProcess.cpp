#include "itkFiltersWindowingProcess.h"


#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medMetaDataKeys.h>

#include "itkImage.h"
#include "itkCommand.h"
#include "itkIntensityWindowingImageFilter.h"

class itkFiltersWindowingProcessPrivate
{
public:
    itk::CStyleCommand::Pointer callback;
    itkFiltersWindowingProcess *filter;
    
    dtkSmartPointer<dtkAbstractData> input;
    dtkSmartPointer<dtkAbstractData> output;
    
    double minimumIntensityValue;
    double maximumIntensityValue;
    double minimumOutputIntensityValue;
    double maximumOutputIntensityValue;
    
    template <class PixelType> void update ( void );
    
    static void eventCallback ( itk::Object *caller, const itk::EventObject& event, void *clientData);
};

void itkFiltersWindowingProcessPrivate::eventCallback ( itk::Object* caller, const itk::EventObject& event, void* clientData )
{
    itkFiltersWindowingProcessPrivate * source = reinterpret_cast<itkFiltersWindowingProcessPrivate *> ( clientData );
    itk::ProcessObject * processObject = ( itk::ProcessObject* ) caller;

    if ( !source )
        qDebug() << "Source est null";

    source->filter->emitProgress((int) (processObject->GetProgress() * 100));
}

template <class PixelType> void itkFiltersWindowingProcessPrivate::update ( void )
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::IntensityWindowingImageFilter< ImageType, ImageType >  WindowingFilterType;
    typename WindowingFilterType::Pointer windowingFilter = WindowingFilterType::New();

    windowingFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
    windowingFilter->SetWindowMinimum ( ( PixelType ) minimumIntensityValue );
    windowingFilter->SetWindowMaximum ( ( PixelType ) maximumIntensityValue );
    windowingFilter->SetOutputMinimum ( ( PixelType ) minimumOutputIntensityValue );
    windowingFilter->SetOutputMaximum ( ( PixelType ) maximumOutputIntensityValue );
    
    callback = itk::CStyleCommand::New();
    callback->SetClientData ( ( void * ) this );
    callback->SetCallback ( itkFiltersWindowingProcessPrivate::eventCallback );

    windowingFilter->AddObserver ( itk::ProgressEvent(), callback );

    windowingFilter->Update();
    output->setData ( windowingFilter->GetOutput() );

    //Set output description metadata
    QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
    newSeriesDescription += " intensity filter";
    
    output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
}

//-------------------------------------------------------------------------------------------

itkFiltersWindowingProcess::itkFiltersWindowingProcess( void ) : itkFiltersProcessBase(), d(new itkFiltersWindowingProcessPrivate)
{
    d->filter = this;
    d->output = NULL;
}

//-------------------------------------------------------------------------------------------

itkFiltersWindowingProcess::~itkFiltersWindowingProcess( void )
{
    delete d;
    d = NULL;
}

//-------------------------------------------------------------------------------------------

bool itkFiltersWindowingProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkWindowingProcess", createitkFiltersWindowingProcess);
}

//-------------------------------------------------------------------------------------------

QString itkFiltersWindowingProcess::description() const
{
    return tr("ITK intensity windowing filter");
}

//-------------------------------------------------------------------------------------------

void itkFiltersWindowingProcess::setInput(dtkAbstractData *data)
{
    if (!data)
        return;
    
    QString identifier = data->identifier();
    
    d->output = dtkAbstractDataFactory::instance()->createSmartPointer(identifier);
    d->input = data;
}

//-------------------------------------------------------------------------------------------

void itkFiltersWindowingProcess::setParameter(double data, int channel)
{
    if (channel != 0)
        return;
    switch ( channel )
    {
    case 0:
        d->minimumIntensityValue = data;
        break;
    case 1:
        d->maximumIntensityValue = data;
        break;
    case 2:
        d->minimumOutputIntensityValue = data;
        break;
    case 3:
        d->maximumOutputIntensityValue = data;
        break;
    }
}

//-------------------------------------------------------------------------------------------

int itkFiltersWindowingProcess::update ( void )
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


dtkAbstractData * itkFiltersWindowingProcess::output ( void )
{
    return ( d->output );
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersWindowingProcess ( void )
{
    return new itkFiltersWindowingProcess;
}
