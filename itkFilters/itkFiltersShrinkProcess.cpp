#include "itkFiltersShrinkProcess.h"

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medMetaDataKeys.h>

#include "itkImage.h"
#include "itkCommand.h"
#include "itkShrinkImageFilter.h"

class itkFiltersShrinkProcessPrivate
{
public:
    itk::CStyleCommand::Pointer callback;
    itkFiltersShrinkProcess *filter;
    
    dtkSmartPointer<dtkAbstractData> input;
    dtkSmartPointer<dtkAbstractData> output;
    
    unsigned int shrinkFactors[3];
    
    template <class PixelType> void update ( void );
    
    static void eventCallback ( itk::Object *caller, const itk::EventObject& event, void *clientData);
};

void itkFiltersShrinkProcessPrivate::eventCallback ( itk::Object* caller, const itk::EventObject& event, void* clientData )
{
    itkFiltersShrinkProcessPrivate * source = reinterpret_cast<itkFiltersShrinkProcessPrivate *> ( clientData );
    itk::ProcessObject * processObject = ( itk::ProcessObject* ) caller;

    if ( !source )
        qDebug() << "Source est null";

    source->filter->emitProgress((int) (processObject->GetProgress() * 100));
}

template <class PixelType> void itkFiltersShrinkProcessPrivate::update ( void )
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::ShrinkImageFilter< ImageType, ImageType >  ShrinkFilterType;
    typename ShrinkFilterType::Pointer shrinkFilter = ShrinkFilterType::New();

    shrinkFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
    shrinkFilter->SetShrinkFactors ( shrinkFactors );
    
    callback = itk::CStyleCommand::New();
    callback->SetClientData ( ( void * ) this );
    callback->SetCallback ( itkFiltersShrinkProcessPrivate::eventCallback );

    shrinkFilter->AddObserver ( itk::ProgressEvent(), callback );

    shrinkFilter->Update();
    output->setData ( shrinkFilter->GetOutput() );

    //Set output description metadata
    QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
    newSeriesDescription += " shrink filter (" + QString::number(shrinkFactors[0]) + "," + QString::number(shrinkFactors[1]) + "," + QString::number(shrinkFactors[2]) + ")";
    
    output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
}

//-------------------------------------------------------------------------------------------

itkFiltersShrinkProcess::itkFiltersShrinkProcess( void ) : itkFiltersProcessBase(), d(new itkFiltersShrinkProcessPrivate)
{
    d->filter = this;
    d->output = NULL;
}

//-------------------------------------------------------------------------------------------

itkFiltersShrinkProcess::~itkFiltersShrinkProcess( void )
{
    delete d;
    d = NULL;
}

//-------------------------------------------------------------------------------------------

bool itkFiltersShrinkProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkShrinkProcess", createitkFiltersShrinkProcess);
}

//-------------------------------------------------------------------------------------------

QString itkFiltersShrinkProcess::description() const
{
    return tr("ITK shrink filter");
}

//-------------------------------------------------------------------------------------------

void itkFiltersShrinkProcess::setInput(dtkAbstractData *data)
{
    if (!data)
        return;
    
    QString identifier = data->identifier();
    
    d->output = dtkAbstractDataFactory::instance()->createSmartPointer(identifier);
    d->input = data;
}

//-------------------------------------------------------------------------------------------

void itkFiltersShrinkProcess::setParameter(double data, int channel)
{
    if (channel != 0)
        return;
    switch ( channel )
    {
    case 0:
        d->shrinkFactors[0] = ( unsigned int ) data;
        break;
    case 1:
        d->shrinkFactors[1] = ( unsigned int ) data;
        break;
    case 2:
        d->shrinkFactors[2] = ( unsigned int ) data;
        break;
    }
}

//-------------------------------------------------------------------------------------------

int itkFiltersShrinkProcess::update ( void )
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


dtkAbstractData * itkFiltersShrinkProcess::output ( void )
{
    return ( d->output );
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersShrinkProcess ( void )
{
    return new itkFiltersShrinkProcess;
}
