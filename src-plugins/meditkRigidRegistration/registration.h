#pragma once

#include "itkCastImageFilter.h"
#include "itkEllipseSpatialObject.h"
#include "itkImage.h"
#include "itkImageRegistrationMethod.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkRegularStepGradientDescentOptimizer.h"
#include "itkResampleImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkSpatialObjectToImageFilter.h"
#include "itkAffineTransform.h"

#include "medAbstractImageData.h"

#include <QMatrix4x4>


template <typename PixelType> void registration(medAbstractImageData* p_fixedImage,medAbstractImageData* p_movingImage,QMatrix4x4* mat)
{
    qDebug()<<Q_FUNC_INFO<<" "<<__LINE__;
    const    unsigned int    Dimension = 3;
    typedef itk::Image< PixelType, Dimension >  ImageType;

    if(!p_fixedImage || !p_movingImage)
        qDebug()<<"no image";

    ImageType *  fixedImage=( dynamic_cast<ImageType *> ((( itk::Object* )  p_fixedImage->data()  ) ) );
    ImageType * movingImage=( dynamic_cast<ImageType *> ((( itk::Object* )  p_movingImage->data() ) ) );
    if(!fixedImage || !movingImage)
        qDebug()<<"no image (after cast)";
    qDebug()<<Q_FUNC_INFO<<" "<<__LINE__;
    //  The transform that will map the fixed image into the moving image.
    typedef itk::AffineTransform< double, Dimension > TransformType;

    //  An optimizer is required to explore the parameter space of the transform
    //  in search of optimal values of the metric.
    typedef itk::RegularStepGradientDescentOptimizer       OptimizerType;

    //  The metric will compare how well the two images match each other. Metric
    //  types are usually parameterized by the image types as it can be seen in
    //  the following type declaration.
    typedef itk::MeanSquaresImageToImageMetric<
            ImageType,
            ImageType >    MetricType;

    //  Finally, the type of the interpolator is declared. The interpolator will
    //  evaluate the intensities of the moving image at non-grid positions.
    typedef itk:: LinearInterpolateImageFunction<
            ImageType,
            double          >    InterpolatorType;
    qDebug()<<Q_FUNC_INFO<<" "<<__LINE__;
    //  The registration method type is instantiated using the types of the
    //  fixed and moving images. This class is responsible for interconnecting
    //  all the components that we have described so far.
    typedef itk::ImageRegistrationMethod<
            ImageType,
            ImageType >    RegistrationType;

    // Create components
    typename MetricType::Pointer         metric        = MetricType::New();
    typename TransformType::Pointer      transform     = TransformType::New();
    typename OptimizerType::Pointer      optimizer     = OptimizerType::New();
    typename InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
    typename RegistrationType::Pointer   registration  = RegistrationType::New();
    qDebug()<<Q_FUNC_INFO<<" "<<__LINE__;
    // Each component is now connected to the instance of the registration method.
    registration->SetMetric(        metric        );
    registration->SetOptimizer(     optimizer     );
    registration->SetTransform(     transform     );
    registration->SetInterpolator(  interpolator  );
    qDebug()<<Q_FUNC_INFO<<" "<<__LINE__;
    // Set the registration inputs
    registration->SetFixedImage(fixedImage);
    registration->SetMovingImage(movingImage);
    qDebug()<<Q_FUNC_INFO<<" "<<__LINE__;
    registration->SetFixedImageRegion(
                fixedImage->GetLargestPossibleRegion() );
    qDebug()<<Q_FUNC_INFO<<" "<<__LINE__;
    //  Initialize the transform
    typedef typename RegistrationType::ParametersType ParametersType;
    ParametersType initialParameters( transform->GetNumberOfParameters() );
    qDebug()<<Q_FUNC_INFO<<" "<<__LINE__;
    // rotation matrix
    initialParameters[0] = 1.0;  // R(0,0)
    initialParameters[1] = 0.0;  // R(0,1)
    initialParameters[2] = 0.0;  // R(1,0)
    initialParameters[3] = 1.0;  // R(1,1)

    // translation vector
    initialParameters[4] = 0.0;
    initialParameters[5] = 0.0;
    qDebug()<<Q_FUNC_INFO<<" "<<__LINE__;
    registration->SetInitialTransformParameters( initialParameters );
    qDebug()<<Q_FUNC_INFO<<" "<<__LINE__;
    optimizer->SetMaximumStepLength( .1 ); // If this is set too high, you will get a
    //"itk::ERROR: MeanSquaresImageToImageMetric(0xa27ce70): Too many samples map outside moving image buffer: 1818 / 10000" error
    qDebug()<<Q_FUNC_INFO<<" "<<__LINE__;
    optimizer->SetMinimumStepLength( 0.01 );

    // Set a stopping criterion
    optimizer->SetNumberOfIterations( 200 );
    qDebug()<<Q_FUNC_INFO<<" "<<__LINE__;
    try
    {
        registration->Update();
    }
    catch( itk::ExceptionObject & err )
    {
        std::cerr << "ExceptionObject caught !" << std::endl;
        std::cerr << err << std::endl;
        //return EXIT_FAILURE;
    }

    //  The result of the registration process is an array of parameters that
    //  defines the spatial transformation in an unique way. This final result is
    //  obtained using the \code{GetLastTransformParameters()} method.
    qDebug()<<Q_FUNC_INFO<<" "<<__LINE__;
    ParametersType finalParameters = registration->GetLastTransformParameters();
    mat->setToIdentity();
    for(int i=0;i<finalParameters.size();i++)
        mat->operator()(i%4,i/4)=finalParameters[i];


    std::cout << "Final parameters: " << finalParameters << std::endl;

    // Print out results
    //
    std::cout << "Result = " << std::endl;

    //  It is common, as the last step of a registration task, to use the
    //  resulting transform to map the moving image into the fixed image space.
    //  This is easily done with the \doxygen{ResampleImageFilter}.

    typedef itk::ResampleImageFilter<
            ImageType,
            ImageType >    ResampleFilterType;
    qDebug()<<Q_FUNC_INFO<<" "<<__LINE__;
    typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();
    resampler->SetInput( movingImage);
    qDebug()<<Q_FUNC_INFO<<" "<<__LINE__;
    //  The Transform that is produced as output of the Registration method is
    //  also passed as input to the resampling filter. Note the use of the
    //  methods \code{GetOutput()} and \code{Get()}. This combination is needed
    //  here because the registration method acts as a filter whose output is a
    //  transform decorated in the form of a \doxygen{DataObject}. For details in
    //  this construction you may want to read the documentation of the
    //  \doxygen{DataObjectDecorator}.

    resampler->SetTransform( registration->GetOutput()->Get() );
    qDebug()<<Q_FUNC_INFO<<" "<<__LINE__;
    //  As described in Section \ref{sec:ResampleImageFilter}, the
    //  ResampleImageFilter requires additional parameters to be specified, in
    //  particular, the spacing, origin and size of the output image. The default
    //  pixel value is also set to a distinct gray level in order to highlight
    //  the regions that are mapped outside of the moving image.

    resampler->SetSize( fixedImage->GetLargestPossibleRegion().GetSize() );
    resampler->SetOutputOrigin(  fixedImage->GetOrigin() );
    resampler->SetOutputSpacing( fixedImage->GetSpacing() );
    resampler->SetOutputDirection( fixedImage->GetDirection() );
    resampler->SetDefaultPixelValue( 100 );
    qDebug()<<Q_FUNC_INFO<<" "<<__LINE__;
    //  The output of the filter is passed to a writer that will store the
    //  image in a file. An \doxygen{CastImageFilter} is used to convert the
    //  pixel type of the resampled image to the final type used by the
    //  writer. The cast and writer filters are instantiated below.

    typedef unsigned char OutputPixelType;
    typedef itk::Image< OutputPixelType, Dimension > OutputImageType;
    typedef itk::CastImageFilter<
            ImageType,
            ImageType > CastFilterType;
    typename CastFilterType::Pointer  caster =  CastFilterType::New();
    caster->SetInput( resampler->GetOutput() );
    caster->Update();
    qDebug()<<Q_FUNC_INFO<<" "<<__LINE__;
    //return EXIT_SUCCESS;
}
