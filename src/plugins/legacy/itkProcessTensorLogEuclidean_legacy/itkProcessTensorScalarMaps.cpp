/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkProcessTensorScalarMaps.h"

#include <dtkCore/dtkSmartPointer.h>
#include <medAbstractImageData.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractDataFactory.h>

#include "itkQtSignalAdaptor.h"

#include <itkRGBAPixel.h>
#include "itkTensor.h"
#include "itkTensorToScalarTensorImageFilter.h"
#include "itkTensorToScalarFunction.h"
#include "itkTensorToFAFunction.h"
#include "itkTensorToColorFAFunction.h"
#include "itkTensorToLogFAFunction.h"
#include "itkTensorToADCFunction.h"
#include "itkTensorToClFunction.h"
#include "itkTensorToCpFunction.h"
#include "itkTensorToCsFunction.h"
#include "itkTensorToRAFunction.h"
#include "itkTensorToVRFunction.h"
#include "itkTensorToVolumeFunction.h"
#include "itkTensorToLambdaFunction.h"

#include <medMetaDataKeys.h>

// /////////////////////////////////////////////////////////////////
// itkProcessTensorScalarMapsPrivate
// /////////////////////////////////////////////////////////////////

class itkProcessTensorScalarMapsPrivate
{
public:
    dtkSmartPointer <medAbstractData> input;
    dtkSmartPointer <medAbstractData> output;
    dtkSmartPointer <dtkAbstractProcess> parent;

    template <class ScalarType> int update();

    // Parameters
    QString mapRequested;
};

template <class ScalarType>
int
itkProcessTensorScalarMapsPrivate::update()
{
    typedef itk::Tensor<ScalarType, 3>    TensorType;
    typedef itk::Image<TensorType, 3>     TensorImageType;

    if (mapRequested == "ColorFA")
    {
        typedef itk::RGBAPixel<unsigned char> ColorType;
        typedef itk::Image<ColorType, 3>      ColorImageType;
        typedef itk::TensorToScalarTensorImageFilter<TensorImageType, ColorImageType> ColorFilterType;

        typename ColorFilterType::Pointer mainFilter = ColorFilterType::New();

        output = medAbstractDataFactory::instance()->createSmartPointer ("itkDataImageRGBA3");
        output->setData(mainFilter->GetOutput());

        itk::QtSignalAdaptor *adaptor = new itk::QtSignalAdaptor;
        mainFilter->AddObserver (itk::ProgressEvent(), adaptor->GetCommand());
        QObject::connect (adaptor, SIGNAL(Signal (int)), parent, SIGNAL (progressed (int)));

        TensorImageType* tensors = dynamic_cast<TensorImageType*>( (itk::Object*)( input->data() ) );
        mainFilter->SetInput(tensors);

        try
        {
            typedef itk::TensorToColorFAFunction<TensorType, ColorType> FunctionType;
            mainFilter->SetTensorToScalarFunction (FunctionType::New());
            FunctionType *colorFunction = dynamic_cast <FunctionType *> (mainFilter->GetTensorToScalarFunction().GetPointer());
            colorFunction->SetTransformColorWithDirection(true);
            colorFunction->SetDirection(mainFilter->GetInput()->GetDirection());

            mainFilter->UpdateLargestPossibleRegion();
        }
        catch ( itk::ExceptionObject &e)
        {
            mainFilter->AbortGenerateDataOff();
            mainFilter->ResetPipeline();
            mainFilter->GetOutput()->Initialize();

            delete adaptor;

            qDebug() << e.GetDescription();
            return -1;
        }

        delete adaptor;
        return 0;
    }

    typedef itk::Image<ScalarType, 3>     ImageType;
    typedef itk::TensorToScalarTensorImageFilter<TensorImageType, ImageType>      FilterType;

    typename FilterType::Pointer filter = FilterType::New();
    if (input->identifier().contains("Double"))
        output = medAbstractDataFactory::instance()->createSmartPointer ("itkDataImageDouble3");
    else
        output = medAbstractDataFactory::instance()->createSmartPointer ("itkDataImageFloat3");

    output->setData(filter->GetOutput());

    itk::QtSignalAdaptor *adaptor = new itk::QtSignalAdaptor;
    filter->AddObserver (itk::ProgressEvent(), adaptor->GetCommand());
    QObject::connect (adaptor, SIGNAL(Signal (int)), parent, SIGNAL (progressed (int)));

    TensorImageType* tensors = dynamic_cast<TensorImageType*>( (itk::Object*)( input->data() ) );
    filter->SetInput(tensors);

    try
    {
        if( mapRequested=="FA" )
            filter->SetTensorToScalarFunction( itk::TensorToFAFunction<TensorType, ScalarType>::New() );
        else if( mapRequested=="LogFA" )
            filter->SetTensorToScalarFunction( itk::TensorToLogFAFunction<TensorType, ScalarType>::New() );
        else if( mapRequested=="ADC" )
            filter->SetTensorToScalarFunction( itk::TensorToADCFunction<TensorType, ScalarType>::New() );
        else if( mapRequested=="Cl" )
            filter->SetTensorToScalarFunction( itk::TensorToClFunction<TensorType, ScalarType>::New() );
        else if( mapRequested=="Cp" )
            filter->SetTensorToScalarFunction( itk::TensorToCpFunction<TensorType, ScalarType>::New() );
        else if( mapRequested=="Cs" )
            filter->SetTensorToScalarFunction( itk::TensorToCsFunction<TensorType, ScalarType>::New() );
        else if( mapRequested=="RA" )
            filter->SetTensorToScalarFunction( itk::TensorToRAFunction<TensorType, ScalarType>::New() );
        else if( mapRequested=="VR" )
            filter->SetTensorToScalarFunction( itk::TensorToVRFunction<TensorType, ScalarType>::New() );
        else if( mapRequested=="Lambda1" )
        {
            typename itk::TensorToLambdaFunction<TensorType, ScalarType>::Pointer function = itk::TensorToLambdaFunction<TensorType, ScalarType>::New();
            function->SetLambdaIndex (2);
            filter->SetTensorToScalarFunction( function );
        }
        else if( mapRequested=="Lambda2" )
        {
            typename itk::TensorToLambdaFunction<TensorType, ScalarType>::Pointer function = itk::TensorToLambdaFunction<TensorType, ScalarType>::New();
            function->SetLambdaIndex (1);
            filter->SetTensorToScalarFunction( function );
        }
        else if( mapRequested=="Lambda3" )
        {
            typename itk::TensorToLambdaFunction<TensorType, ScalarType>::Pointer function = itk::TensorToLambdaFunction<TensorType, ScalarType>::New();
            function->SetLambdaIndex (0);
            filter->SetTensorToScalarFunction( function );
        }

        filter->UpdateLargestPossibleRegion();
    }
    catch ( itk::ExceptionObject &e)
    {
        filter->AbortGenerateDataOff();
        filter->ResetPipeline();
        filter->GetOutput()->Initialize();

        delete adaptor;

        qDebug() << e.GetDescription();
        return -1;
    }

    delete adaptor;
    return 0;
}

// /////////////////////////////////////////////////////////////////
// itkProcessTensorScalarMaps
// /////////////////////////////////////////////////////////////////

itkProcessTensorScalarMaps::itkProcessTensorScalarMaps() : medAbstractDiffusionProcess(), d(new itkProcessTensorScalarMapsPrivate)
{
    d->mapRequested = "FA";
    d->parent = this;
}

itkProcessTensorScalarMaps::~itkProcessTensorScalarMaps()
{
    delete d;
    d = 0;
}

bool itkProcessTensorScalarMaps::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkProcessTensorScalarMaps", createItkProcessTensorScalarMaps);
}

QString itkProcessTensorScalarMaps::identifier() const
{
    return "itkProcessTensorScalarMaps";
}

QString itkProcessTensorScalarMaps::description() const
{
    return "itkProcessTensorScalarMaps";
}

void itkProcessTensorScalarMaps::setInputImage(medAbstractData *input)
{
    if (!input)
        return;
    
    d->input = input;
}

void itkProcessTensorScalarMaps::setTensorScalarMapRequested(QString map)
{
    d->mapRequested = map;
}

int itkProcessTensorScalarMaps::update()
{
    if ( !d->input )
        return -1;

    medAbstractImageData *imageData = dynamic_cast <medAbstractImageData *> (d->input.data());

    if (!imageData)
        return -1;

    QString type = imageData->identifier();

    int returnCode = 0;
    if (type == "itkDataTensorImageFloat3")
    {
        returnCode = d->update <float> ();
    }
    else if (type == "itkDataTensorImageDouble3")
    {
        returnCode = d->update <double> ();
    }
    else
        return -1;

    QString newSeriesDescription = d->input->metadata ( medMetaDataKeys::SeriesDescription.key() );
    newSeriesDescription += " ";
    newSeriesDescription += d->mapRequested;
    d->output->setMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    
    foreach ( QString metaData, d->input->metaDataList() )
    {
        if (!d->output->hasMetaData(metaData))
            d->output->addMetaData ( metaData, d->input->metaDataValues ( metaData ) );
    }
    
    foreach ( QString property, d->input->propertyList() )
    d->output->addProperty ( property,d->input->propertyValues ( property ) );
    
    QString generatedID = QUuid::createUuid().toString().replace("{","").replace("}","");
    d->output->setMetaData ( medMetaDataKeys::SeriesID.key(), generatedID );

    emit progressed(100);
    
    return returnCode;
}

medAbstractData *itkProcessTensorScalarMaps::output()
{
    return d->output;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createItkProcessTensorScalarMaps()
{
    return new itkProcessTensorScalarMaps;
}
