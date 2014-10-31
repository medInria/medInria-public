/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSubtractImageFilter.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

#include <itkImage.h>
#include <itkCommand.h>
#include <itkSubtractImageFilter.h>
#include <itkExceptionObject.h>

#include <medMetaDataKeys.h>
#include <medAbstractDataFactory.h>


class medSubtractImageFilterPrivate
{
public:

    medSubtractImageFilterPrivate(medSubtractImageFilter *q) {parent=q;}
    virtual ~medSubtractImageFilterPrivate(void) {}

    medSubtractImageFilter* parent;
    //medDoubleParameter *subtractValueParam;
    QList<medAbstractParameter*> parameters;

    //TODO: template over image 1 and image 2 types ?
    template <class PixelType> int update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::SubtractImageFilter< ImageType, ImageType, ImageType >  SubtractFilterType;
        typename SubtractFilterType::Pointer subtractFilter = SubtractFilterType::New();

        subtractFilter->SetInput1 ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( parent->input<medAbstractData*>(0)->data() ) ) );
        subtractFilter->SetInput2 ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( parent->input<medAbstractData*>(1)->data() ) ) );

        itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) parent );
        parent->setCallBack(callback);

        subtractFilter->AddObserver ( itk::ProgressEvent(), callback );

        try
        {
            subtractFilter->Update();
        }
        catch (...)
        {
            qWarning() << "Exception caught in medSubtractImageFilter";
            return EXIT_FAILURE;
        }

        parent->output<medAbstractData*>(0)->setData ( subtractFilter->GetOutput() );

        //Set output description metadata
        QString newSeriesDescription = parent->input<medAbstractData*>(0)->metadata ( medMetaDataKeys::SeriesDescription.key() );
        QString seriesDescription2 = parent->input<medAbstractData*>(1)->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " minus " + seriesDescription2;

        parent->output<medAbstractData*>(0)->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );

        return EXIT_SUCCESS;
    }

};


medSubtractImageFilter::medSubtractImageFilter(itkFiltersProcessBase *parent):
    itkFiltersProcessBase(parent), d(new medSubtractImageFilterPrivate(this))
{
    medInputDataPort *input = new medInputDataPort;
    input->name = "Image to subtract";
    input->isOptional = false;
    input->input = NULL;

    this->appendInput(input);
}

medSubtractImageFilter::~medSubtractImageFilter()
{
    delete d;
}


//-------------------------------------------------------------------------------------------

bool medSubtractImageFilter::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("medSubtractImageFilter", createimedSubtractImageFilter, "medAbstractFilteringProcess");
}

//-------------------------------------------------------------------------------------------

QList<medAbstractParameter*> medSubtractImageFilter::parameters()
{
    return QList<medAbstractParameter*>();
}

//-------------------------------------------------------------------------------------------

int medSubtractImageFilter::update()
{
    if ( !this->input<medAbstractData*>(0) )
        return -1;

    QString id = this->input<medAbstractData*>(0)->identifier();

    medAbstractData *output = medAbstractDataFactory::instance()->create(id);
    this->setOutput<medAbstractData*>(output, 0);

    int res = EXIT_SUCCESS;

    if ( id == "itkDataImageChar3" )
    {
        res = d->update<char>();
    }
    else if ( id == "itkDataImageUChar3" )
    {
        res = d->update<unsigned char>();
    }
    else if ( id == "itkDataImageShort3" )
    {
        res = d->update<short>();
    }
    else if ( id == "itkDataImageUShort3" )
    {
        res = d->update<unsigned short>();
    }
    else if ( id == "itkDataImageInt3" )
    {
        res = d->update<int>();
    }
    else if ( id == "itkDataImageUInt3" )
    {
        res = d->update<unsigned int>();
    }
    else if ( id == "itkDataImageLong3" )
    {
        res = d->update<long>();
    }
    else if ( id== "itkDataImageULong3" )
    {
        res = d->update<unsigned long>();
    }
    else if ( id == "itkDataImageFloat3" )
    {
        res = d->update<float>();
    }
    else if ( id == "itkDataImageDouble3" )
    {
        res = d->update<double>();
    }
    else
    {
        qDebug() << "Error : pixel type not yet implemented ("
        << id
        << ")";
        res = EXIT_FAILURE;
    }

    return res;

}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createimedSubtractImageFilter()
{
    return new medSubtractImageFilter;
}
