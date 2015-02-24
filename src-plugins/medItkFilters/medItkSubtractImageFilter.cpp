/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkSubtractImageFilter.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

#include <itkImage.h>
#include <itkCommand.h>
#include <itkSubtractImageFilter.h>
#include <itkExceptionObject.h>

#include <medMetaDataKeys.h>
#include <medAbstractDataFactory.h>


class medItkSubtractImageFilterPrivate
{
public:

    medItkSubtractImageFilterPrivate(medItkSubtractImageFilter *q) {parent=q;}
    virtual ~medItkSubtractImageFilterPrivate(void) {}

    medItkSubtractImageFilter* parent;
    //medDoubleParameter *subtractValueParam;
    QList<medAbstractParameter*> parameters;
    QString res;

    //TODO: template over image 1 and image 2 types ?
    template <class PixelType> int update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::SubtractImageFilter< ImageType, ImageType, ImageType >  SubtractFilterType;
        typename SubtractFilterType::Pointer subtractFilter = SubtractFilterType::New();

        subtractFilter->SetInput1 ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( parent->input<medAbstractData>(0)->data() ) ) );
        subtractFilter->SetInput2 ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( parent->input<medAbstractData>(1)->data() ) ) );

        itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) parent );
        parent->setCallBack(callback);

        subtractFilter->AddObserver ( itk::ProgressEvent(), callback );

        try
        {
            subtractFilter->Update();
        }
        catch (itk::ExceptionObject err)
        {
            res = err.GetDescription();
            return EXIT_FAILURE;
        }

        parent->output<medAbstractData>(0)->setData ( subtractFilter->GetOutput() );

        //Set output description metadata
        QString newSeriesDescription = parent->input<medAbstractData>(0)->metadata ( medMetaDataKeys::SeriesDescription.key() );
        QString seriesDescription2 = parent->input<medAbstractData>(1)->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " minus " + seriesDescription2;

        parent->output<medAbstractData>(0)->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );

        return EXIT_SUCCESS;
    }



};


medItkSubtractImageFilter::medItkSubtractImageFilter(medItkFiltersProcessBase *parent):
    medItkFiltersProcessBase(parent), d(new medItkSubtractImageFilterPrivate(this))
{
    medProcessInput<medAbstractImageData> *input = new medProcessInput<medAbstractImageData>("Image to subtract", false);
    //input->setInput(NULL);

    this->appendDataInput(input);
}

medItkSubtractImageFilter::~medItkSubtractImageFilter()
{
    delete d;
}


//-------------------------------------------------------------------------------------------

bool medItkSubtractImageFilter::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("medItkSubtractImageFilter", createimedItkSubtractImageFilter, "medAbstractFilteringProcess");
}

//-------------------------------------------------------------------------------------------

QList<medAbstractParameter*> medItkSubtractImageFilter::parameters()
{
    return QList<medAbstractParameter*>();
}

//-------------------------------------------------------------------------------------------

int medItkSubtractImageFilter::update()
{
    if ( !this->input<medAbstractData>(0) )
        return -1;

    QString id = this->input<medAbstractData>(0)->identifier();

    medAbstractData *output = medAbstractDataFactory::instance()->create(id);
    this->setOutput<medAbstractData>(output, 0);

    int res = EXIT_SUCCESS;

    if ( id == "medItkChar3ImageData" )
    {
        res = d->update<char>();
    }
    else if ( id == "medItkUChar3ImageData" )
    {
        res = d->update<unsigned char>();
    }
    else if ( id == "medItkShort3ImageData" )
    {
        res = d->update<short>();
    }
    else if ( id == "medItkUShort3ImageData" )
    {
        res = d->update<unsigned short>();
    }
    else if ( id == "medItkInt3ImageData" )
    {
        res = d->update<int>();
    }
    else if ( id == "medItkUInt3ImageData" )
    {
        res = d->update<unsigned int>();
    }
    else if ( id == "medItkLong3ImageData" )
    {
        res = d->update<long>();
    }
    else if ( id== "medItkULong3ImageData" )
    {
        res = d->update<unsigned long>();
    }
    else if ( id == "medItkFloat3ImageData" )
    {
        res = d->update<float>();
    }
    else if ( id == "medItkDouble3ImageData" )
    {
        res = d->update<double>();
    }
    else
    {
        d->res = "Error : pixel type not yet implemented (" + id + ")";
        res = EXIT_FAILURE;
    }
    if(!d->res.isEmpty())
        emit showError(d->res);

    return res;

}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createimedItkSubtractImageFilter()
{
    return new medItkSubtractImageFilter;
}
