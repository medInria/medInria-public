/* itkProcessTensorEstimation.cpp ---
 *
 * Author: Pierre Fillard
 * Copyright (C) 2008 - Pierre Fillard, Inria.
 * Created: Thu May 28 20:22:54 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Apr  6 22:46:51 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 10
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include "itkProcessTensorLogEuclideanEstimation.h"

#include <dtkCore/dtkSmartPointer.h>
#include <medAbstractData.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractDataFactory.h>
#include <dtkLog/dtkLog.h>

#include <itkObject.h>
#include <itkCommand.h>
#include <itkProcessObject.h>
#include <itkEventObject.h>
#include <itkImage.h>
#include <itkExtractImageBufferFilter.h>
#include <itkTensor.h>
#include <itkDTIEstimatorTensorImageFilter.h>

#include "itkQtSignalAdaptor.h"

#include <QVector>

// /////////////////////////////////////////////////////////////////
// itkProcessTensorLogEuclideanEstimationPrivate
// /////////////////////////////////////////////////////////////////

class itkProcessTensorLogEuclideanEstimationPrivate
{
public:
    static const unsigned int dimension = 3;
    typedef float                              ScalarType;
    typedef itk::Tensor<ScalarType, dimension> TensorType;
    typedef itk::Image<TensorType, dimension>  TensorImageType;

    typedef itk::DTIEstimatorTensorImageFilter < itk::Image<unsigned char, 3>, TensorImageType >   TensorEstimatorUCharType;
    typedef itk::DTIEstimatorTensorImageFilter < itk::Image<char, 3>, TensorImageType >            TensorEstimatorCharType;
    typedef itk::DTIEstimatorTensorImageFilter < itk::Image<unsigned short, 3>, TensorImageType >  TensorEstimatorUShortType;
    typedef itk::DTIEstimatorTensorImageFilter < itk::Image<short, 3>, TensorImageType >           TensorEstimatorShortType;
    typedef itk::DTIEstimatorTensorImageFilter < itk::Image<unsigned int, 3>, TensorImageType >    TensorEstimatorUIntType;
    typedef itk::DTIEstimatorTensorImageFilter < itk::Image<int, 3>, TensorImageType >             TensorEstimatorIntType;
    typedef itk::DTIEstimatorTensorImageFilter < itk::Image<unsigned long, 3>, TensorImageType >   TensorEstimatorULongType;
    typedef itk::DTIEstimatorTensorImageFilter < itk::Image<long, 3>, TensorImageType >            TensorEstimatorLongType;
    typedef itk::DTIEstimatorTensorImageFilter < itk::Image<float, 3>, TensorImageType >           TensorEstimatorFloatType;
    typedef itk::DTIEstimatorTensorImageFilter < itk::Image<double, 3>, TensorImageType >          TensorEstimatorDoubleType;

    typedef TensorEstimatorDoubleType::GradientType     GradientType;
    typedef TensorEstimatorDoubleType::GradientListType GradientListType;

    itkProcessTensorLogEuclideanEstimationPrivate(void);
    ~itkProcessTensorLogEuclideanEstimationPrivate(void);

public:

    TensorEstimatorDoubleType::Pointer estimatorDouble;
    TensorEstimatorFloatType::Pointer  estimatorFloat;
    TensorEstimatorLongType::Pointer   estimatorLong;
    TensorEstimatorULongType::Pointer  estimatorULong;
    TensorEstimatorIntType::Pointer    estimatorInt;
    TensorEstimatorUIntType::Pointer   estimatorUInt;
    TensorEstimatorShortType::Pointer  estimatorShort;
    TensorEstimatorUShortType::Pointer estimatorUShort;
    TensorEstimatorCharType::Pointer   estimatorChar;
    TensorEstimatorUCharType::Pointer  estimatorUChar;
    GradientListType                   gradientList;

    dtkSmartPointer<medAbstractData>   output;
};

itkProcessTensorLogEuclideanEstimationPrivate::itkProcessTensorLogEuclideanEstimationPrivate()
{
}

#define itkProcessTensorLogEuclideanEstimationSetInput(type)                 \
    typedef itk::ExtractImageBufferFilter<Image4DType, ImageType> ExtractorType; \
    Image4DType::SizeType  size  = image->GetLargestPossibleRegion().GetSize();  \
    unsigned int imageCount = size[3];                                           \
    size[3] = 0; \
    Image4DType::IndexType index = {{0,0,0,0}}; \
    Image4DType::RegionType region; \
    region.SetSize (size); \
    for (unsigned int i=0; i<imageCount; i++) { \
        index[3] = i; \
        region.SetIndex (index); \
        ExtractorType::Pointer extractor = ExtractorType::New(); \
        extractor->SetExtractionRegion (region); \
        extractor->SetInput (image); \
        extractor->SetDirectionCollapseToGuess(); \
        try { \
            extractor->Update(); \
        } \
        catch (itk::ExceptionObject &e) { \
            qDebug() << e.GetDescription(); \
            return; \
        } \
       d->estimator##type->SetInput(i, extractor->GetOutput()); \
    } \

itkProcessTensorLogEuclideanEstimationPrivate::~itkProcessTensorLogEuclideanEstimationPrivate()
{
}

// /////////////////////////////////////////////////////////////////
// itkProcessTensorLogEuclideanEstimation
// /////////////////////////////////////////////////////////////////

itkProcessTensorLogEuclideanEstimation::itkProcessTensorLogEuclideanEstimation(void) : medAbstractDiffusionProcess(), d (new itkProcessTensorLogEuclideanEstimationPrivate)
{
    d->estimatorDouble = itkProcessTensorLogEuclideanEstimationPrivate::TensorEstimatorDoubleType::New();
    d->estimatorFloat  = itkProcessTensorLogEuclideanEstimationPrivate::TensorEstimatorFloatType::New();
    d->estimatorLong   = itkProcessTensorLogEuclideanEstimationPrivate::TensorEstimatorLongType::New();
    d->estimatorULong  = itkProcessTensorLogEuclideanEstimationPrivate::TensorEstimatorULongType::New();
    d->estimatorInt    = itkProcessTensorLogEuclideanEstimationPrivate::TensorEstimatorIntType::New();
    d->estimatorUInt   = itkProcessTensorLogEuclideanEstimationPrivate::TensorEstimatorUIntType::New();
    d->estimatorShort  = itkProcessTensorLogEuclideanEstimationPrivate::TensorEstimatorShortType::New();
    d->estimatorUShort = itkProcessTensorLogEuclideanEstimationPrivate::TensorEstimatorUShortType::New();
    d->estimatorChar   = itkProcessTensorLogEuclideanEstimationPrivate::TensorEstimatorCharType::New();
    d->estimatorUChar  = itkProcessTensorLogEuclideanEstimationPrivate::TensorEstimatorUCharType::New();

    itk::QtSignalAdaptor *adaptor = new itk::QtSignalAdaptor;
    d->estimatorDouble->AddObserver (itk::ProgressEvent(), adaptor->GetCommand());
    d->estimatorFloat->AddObserver  (itk::ProgressEvent(), adaptor->GetCommand());
    d->estimatorLong->AddObserver   (itk::ProgressEvent(), adaptor->GetCommand());
    d->estimatorULong->AddObserver  (itk::ProgressEvent(), adaptor->GetCommand());
    d->estimatorInt->AddObserver    (itk::ProgressEvent(), adaptor->GetCommand());
    d->estimatorUInt->AddObserver   (itk::ProgressEvent(), adaptor->GetCommand());
    d->estimatorShort->AddObserver  (itk::ProgressEvent(), adaptor->GetCommand());
    d->estimatorUShort->AddObserver (itk::ProgressEvent(), adaptor->GetCommand());
    d->estimatorChar->AddObserver   (itk::ProgressEvent(), adaptor->GetCommand());
    d->estimatorUChar->AddObserver  (itk::ProgressEvent(), adaptor->GetCommand());

    connect (adaptor, SIGNAL(Signal (int)), this, SIGNAL (progressed (int)));

    d->output = medAbstractDataFactory::instance()->createSmartPointer ("itkDataTensorImageFloat3");

}

itkProcessTensorLogEuclideanEstimation::~itkProcessTensorLogEuclideanEstimation(void)
{
    delete d;
    d = 0;
}

bool itkProcessTensorLogEuclideanEstimation::registered(void)
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkProcessTensorLogEuclideanEstimation", createItkProcessTensorLogEuclideanEstimation);
}

QString itkProcessTensorLogEuclideanEstimation::identifier(void) const
{
    return "itkProcessTensorLogEuclideanEstimation";
}

QString itkProcessTensorLogEuclideanEstimation::description(void) const
{
    return "itkProcessTensorLogEuclideanEstimation";
}

void itkProcessTensorLogEuclideanEstimation::setInputImage(medAbstractData *data)
{
    if (!data)
        return;

    // check if data has the gradients has metadata
    if (data->hasMetaData("DiffusionGradientList")) {

        QStringList diffusionGradients = data->metaDataValues ("DiffusionGradientList");
        if (diffusionGradients.count()%3==0) {
            int gradientCount = diffusionGradients.count() / 3;
            for (int i=0; i<gradientCount; i++) {
                double grad[3];
                grad[0] = diffusionGradients[3*i+0].toDouble();
                grad[1] = diffusionGradients[3*i+1].toDouble();
                grad[2] = diffusionGradients[3*i+2].toDouble();
                this->setParameter (grad, 3, i);
            }
        }
        else
            qDebug() << "Diffusion gradient list is not a multiple of 3";
    }
    else
        qDebug() << "DiffusionGradientList is not set";

    if (data->identifier()=="itkDataImageUChar4") {

        typedef itk::Image<unsigned char, 4> Image4DType;
        typedef itk::Image<unsigned char, 3> ImageType;

        if (Image4DType *image=dynamic_cast<Image4DType *>( (itk::Object*)(data->data()) ) ) {

            itkProcessTensorLogEuclideanEstimationSetInput(UChar);

            if (d->output)
                d->output->setData ( d->estimatorUChar->GetOutput() );
        }
        else {
            dtkWarn() << "Cannot cast data into required type";
        }
    }
    else if (data->identifier()=="itkDataImageChar4") {

        typedef itk::Image<char, 4> Image4DType;
        typedef itk::Image<char, 3> ImageType;

        if (Image4DType *image=dynamic_cast<Image4DType *>( (itk::Object*)(data->data()) ) ) {

            itkProcessTensorLogEuclideanEstimationSetInput(Char);

            if (d->output)
                d->output->setData ( d->estimatorChar->GetOutput() );
        }
        else {
            dtkWarn() << "Cannot cast data into required type";
        }
    }
    else if (data->identifier()=="itkDataImageUShort4") {

        typedef itk::Image<unsigned short, 4> Image4DType;
        typedef itk::Image<unsigned short, 3> ImageType;

        if (Image4DType *image=dynamic_cast<Image4DType *>( (itk::Object*)(data->data()) ) ) {

            itkProcessTensorLogEuclideanEstimationSetInput(UShort);

            if (d->output)
                d->output->setData ( d->estimatorUShort->GetOutput() );
        }
        else {
            dtkWarn() << "Cannot cast data into required type";
        }
    }
    else if (data->identifier()=="itkDataImageShort4") {

        typedef itk::Image<short, 4> Image4DType;
        typedef itk::Image<short, 3> ImageType;

        if (Image4DType *image=dynamic_cast<Image4DType *>( (itk::Object*)(data->data()) ) ) {

            itkProcessTensorLogEuclideanEstimationSetInput(Short);

            if (d->output)
                d->output->setData ( d->estimatorShort->GetOutput() );
        }
        else {
            dtkWarn() << "Cannot cast data into required type";
        }
    }
    else if (data->identifier()=="itkDataImageUInt4") {

        typedef itk::Image<unsigned int, 4> Image4DType;
        typedef itk::Image<unsigned int, 3> ImageType;

        if (Image4DType *image=dynamic_cast<Image4DType *>( (itk::Object*)(data->data()) ) ) {

            itkProcessTensorLogEuclideanEstimationSetInput(UInt);

            if (d->output)
                d->output->setData ( d->estimatorUInt->GetOutput() );
        }
        else {
            dtkWarn() << "Cannot cast data into required type";
        }
    }
    else if (data->identifier()=="itkDataImageInt4") {

        typedef itk::Image<int, 4> Image4DType;
        typedef itk::Image<int, 3> ImageType;

        if (Image4DType *image=dynamic_cast<Image4DType *>( (itk::Object*)(data->data()) ) ) {

            itkProcessTensorLogEuclideanEstimationSetInput(Int);

            if (d->output)
                d->output->setData ( d->estimatorInt->GetOutput() );
        }
        else {
            dtkWarn() << "Cannot cast data into required type";
        }
    }
    else if (data->identifier()=="itkDataImageULong4") {

        typedef itk::Image<unsigned long, 4> Image4DType;
        typedef itk::Image<unsigned long, 3> ImageType;

        if (Image4DType *image=dynamic_cast<Image4DType *>( (itk::Object*)(data->data()) ) ) {

            itkProcessTensorLogEuclideanEstimationSetInput(ULong);

            if (d->output)
                d->output->setData ( d->estimatorULong->GetOutput() );
        }
        else {
            dtkWarn() << "Cannot cast data into required type";
        }
    }
    else if (data->identifier()=="itkDataImageLong4") {

        typedef itk::Image<long, 4> Image4DType;
        typedef itk::Image<long, 3> ImageType;

        if (Image4DType *image=dynamic_cast<Image4DType *>( (itk::Object*)(data->data()) ) ) {

            itkProcessTensorLogEuclideanEstimationSetInput(Long);

            if (d->output)
                d->output->setData ( d->estimatorLong->GetOutput() );
        }
        else {
            dtkWarn() << "Cannot cast data into required type";
        }
    }
    else if (data->identifier()=="itkDataImageFloat4") {

        typedef itk::Image<float, 4> Image4DType;
        typedef itk::Image<float, 3> ImageType;

        if (Image4DType *image=dynamic_cast<Image4DType *>( (itk::Object*)(data->data()) ) ) {

            itkProcessTensorLogEuclideanEstimationSetInput(Float);

            if (d->output)
                d->output->setData ( d->estimatorFloat->GetOutput() );
        }
        else {
            dtkWarn() << "Cannot cast data into required type";
        }
    }
    else if (data->identifier()=="itkDataImageDouble4") {

        typedef itk::Image<double, 4> Image4DType;
        typedef itk::Image<double, 3> ImageType;

        if (Image4DType *image=dynamic_cast<Image4DType *>( (itk::Object*)(data->data()) ) ) {

            itkProcessTensorLogEuclideanEstimationSetInput(Double);

            if (d->output)
                d->output->setData ( d->estimatorDouble->GetOutput() );
        }
        else {
            dtkWarn() << "Cannot cast data into required type";
        }
    }
    else
        dtkWarn() << "Unsupported input type: " << data->identifier();

}

medAbstractData *itkProcessTensorLogEuclideanEstimation::output(void)
{
    return d->output;
}

int itkProcessTensorLogEuclideanEstimation::update(void)
{
    try {
        if( d->estimatorDouble->GetNumberOfIndexedInputs() ) {
            d->estimatorDouble->UpdateLargestPossibleRegion();
        }
        else if ( d->estimatorFloat->GetNumberOfIndexedInputs() ) {
            d->estimatorFloat->UpdateLargestPossibleRegion();
        }
        else if ( d->estimatorLong->GetNumberOfIndexedInputs() ) {
            d->estimatorLong->UpdateLargestPossibleRegion();
        }
        else if ( d->estimatorULong->GetNumberOfIndexedInputs() ) {
            d->estimatorULong->UpdateLargestPossibleRegion();
        }
        else if ( d->estimatorInt->GetNumberOfIndexedInputs() ) {
            d->estimatorInt->UpdateLargestPossibleRegion();
        }
        else if ( d->estimatorUInt->GetNumberOfIndexedInputs() ) {
            d->estimatorUInt->UpdateLargestPossibleRegion();
        }
        else if ( d->estimatorShort->GetNumberOfIndexedInputs() ) {
            d->estimatorShort->UpdateLargestPossibleRegion();
        }
        else if ( d->estimatorUShort->GetNumberOfIndexedInputs() ) {
            d->estimatorUShort->UpdateLargestPossibleRegion();
        }
        else if ( d->estimatorChar->GetNumberOfIndexedInputs() ) {
            d->estimatorChar->UpdateLargestPossibleRegion();
        }
        else if ( d->estimatorUChar->GetNumberOfIndexedInputs() ) {
            d->estimatorUChar->UpdateLargestPossibleRegion();
        }
    }
    catch (itk::ExceptionObject &e) {
        d->estimatorUChar->AbortGenerateDataOff();
	d->estimatorUChar->ResetPipeline();
	d->estimatorUChar->GetOutput()->Initialize();

	d->estimatorChar->AbortGenerateDataOff();
	d->estimatorChar->ResetPipeline();
	d->estimatorChar->GetOutput()->Initialize();

	d->estimatorUShort->AbortGenerateDataOff();
	d->estimatorUShort->ResetPipeline();
	d->estimatorUShort->GetOutput()->Initialize();

	d->estimatorShort->AbortGenerateDataOff();
	d->estimatorShort->ResetPipeline();
	d->estimatorShort->GetOutput()->Initialize();

	d->estimatorUInt->AbortGenerateDataOff();
	d->estimatorUInt->ResetPipeline();
	d->estimatorUInt->GetOutput()->Initialize();

	d->estimatorInt->AbortGenerateDataOff();
	d->estimatorInt->ResetPipeline();
	d->estimatorInt->GetOutput()->Initialize();

	d->estimatorULong->AbortGenerateDataOff();
	d->estimatorULong->ResetPipeline();
	d->estimatorULong->GetOutput()->Initialize();

	d->estimatorLong->AbortGenerateDataOff();
	d->estimatorLong->ResetPipeline();
	d->estimatorLong->GetOutput()->Initialize();

	d->estimatorFloat->AbortGenerateDataOff();
	d->estimatorFloat->ResetPipeline();
	d->estimatorFloat->GetOutput()->Initialize();

	d->estimatorDouble->AbortGenerateDataOff();
	d->estimatorDouble->ResetPipeline();
	d->estimatorDouble->GetOutput()->Initialize();

        qDebug() << e.GetDescription();
	emit progressed (100);
	return -1;
    }
    
    qDebug() << "Done updating";

    emit progressed (100);
    return 0;
}

void itkProcessTensorLogEuclideanEstimation::onCanceled (void)
{
    d->estimatorUChar->AbortGenerateDataOn();
    d->estimatorChar->AbortGenerateDataOn();
    d->estimatorUShort->AbortGenerateDataOn();
    d->estimatorShort->AbortGenerateDataOn();
    d->estimatorUInt->AbortGenerateDataOn();
    d->estimatorInt->AbortGenerateDataOn();
    d->estimatorULong->AbortGenerateDataOn();
    d->estimatorLong->AbortGenerateDataOn();
    d->estimatorFloat->AbortGenerateDataOn();
    d->estimatorDouble->AbortGenerateDataOn();
}

void itkProcessTensorLogEuclideanEstimation::setParameter (double value)
{
    this->onBSTValueChanged (value);
}


void itkProcessTensorLogEuclideanEstimation::setParameter (double value, int channel)
{
    this->setParameter (value);
}

void itkProcessTensorLogEuclideanEstimation::setParameter (double* data, int count, int channel)
{
    if( channel>=0 ){
        if( count != 3 ){
            qDebug() << "itkProcessTensorLogEuclideanEstimation only accepts three components per gradient.";
            return;
        }

        if( (unsigned int)channel+1 >= d->gradientList.size() ){
            itkProcessTensorLogEuclideanEstimationPrivate::GradientListType newList ( channel+1 );
            for( unsigned int i=0; i<d->gradientList.size(); i++)
                newList[i] = d->gradientList[i];
            d->gradientList = newList;
        }

        d->gradientList[channel] = itkProcessTensorLogEuclideanEstimationPrivate::GradientType ( data );

        d->estimatorDouble->SetGradientList( d->gradientList );
        d->estimatorFloat->SetGradientList( d->gradientList );
        d->estimatorLong->SetGradientList( d->gradientList );
        d->estimatorULong->SetGradientList( d->gradientList );
        d->estimatorInt->SetGradientList( d->gradientList );
        d->estimatorUInt->SetGradientList( d->gradientList );
        d->estimatorShort->SetGradientList( d->gradientList );
        d->estimatorUShort->SetGradientList( d->gradientList );
        d->estimatorChar->SetGradientList( d->gradientList );
        d->estimatorUChar->SetGradientList( d->gradientList );
    }
}

void itkProcessTensorLogEuclideanEstimation::onBSTValueChanged(double value)
{
    d->estimatorDouble->SetBST(value);
    d->estimatorFloat->SetBST(value);
    d->estimatorLong->SetBST(value);
    d->estimatorULong->SetBST(value);
    d->estimatorInt->SetBST(value);
    d->estimatorUInt->SetBST(value);
    d->estimatorShort->SetBST(value);
    d->estimatorUShort->SetBST(value);
    d->estimatorChar->SetBST(value);
    d->estimatorUChar->SetBST(value);
}

// /////////////////////////////////////////////////////////////////
// Process type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createItkProcessTensorLogEuclideanEstimation(void)
{
    return new itkProcessTensorLogEuclideanEstimation;
}
