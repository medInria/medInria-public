#include "itkFiltersSubtractProcess.h"

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medMetaDataKeys.h>

#include "itkImage.h"
#include "itkCommand.h"
#include "itkSubtractConstantFromImageFilter.h"

class itkFiltersSubtractProcessPrivate
{
public:
    itk::CStyleCommand::Pointer callback;
    itkFiltersSubtractProcess *filter;
    
    dtkSmartPointer<dtkAbstractData> input;
    dtkSmartPointer<dtkAbstractData> output;
    
    double subtractValue;
    
    template <class PixelType> void update ( void );
    
    static void eventCallback ( itk::Object *caller, const itk::EventObject& event, void *clientData);  
    
};


void itkFiltersSubtractProcessPrivate::eventCallback ( itk::Object* caller, const itk::EventObject& event, void* clientData )
{
    itkFiltersSubtractProcessPrivate * source = reinterpret_cast<itkFiltersSubtractProcessPrivate *> ( clientData );
    itk::ProcessObject * processObject = ( itk::ProcessObject* ) caller;

    if ( !source )
        qDebug() << "Source est null";

    source->filter->emitProgress((int) (processObject->GetProgress() * 100));
}

template <class PixelType> void itkFiltersSubtractProcessPrivate::update ( void )
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::SubtractConstantFromImageFilter< ImageType, double, ImageType >  SubtractFilterType;
    typename SubtractFilterType::Pointer subtractFilter = SubtractFilterType::New();

    subtractFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
    subtractFilter->SetConstant ( subtractValue);
    
    callback = itk::CStyleCommand::New();
    callback->SetClientData ( ( void * ) this );
    callback->SetCallback ( itkFiltersSubtractProcessPrivate::eventCallback );

    subtractFilter->AddObserver ( itk::ProgressEvent(), callback );

    subtractFilter->Update();
    output->setData ( subtractFilter->GetOutput() );

    //Set output description metadata
    QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
    newSeriesDescription += " add filter (" + QString::number(subtractValue) + ")";

    output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
}

//-------------------------------------------------------------------------------------------

itkFiltersSubtractProcess::itkFiltersSubtractProcess( void ) : itkFiltersProcessBase(), d(new itkFiltersSubtractProcessPrivate)
{
    d->filter = this;
    d->output = NULL;
}

//-------------------------------------------------------------------------------------------

itkFiltersSubtractProcess::~itkFiltersSubtractProcess( void )
{
    delete d;
    d = NULL;
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
    
    d->output = dtkAbstractDataFactory::instance()->createSmartPointer(identifier);
    d->input = data;
}

//-------------------------------------------------------------------------------------------

void itkFiltersSubtractProcess::setParameter(double data, int channel)
{
    if (channel != 0)
        return;
    
    d->subtractValue = data;
}

//-------------------------------------------------------------------------------------------

int itkFiltersSubtractProcess::update ( void )
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


dtkAbstractData * itkFiltersSubtractProcess::output ( void )
{
    return ( d->output );
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersSubtractProcess ( void )
{
    return new itkFiltersSubtractProcess;
}


























