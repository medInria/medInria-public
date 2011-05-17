#include "msegAlgorithmConnectedThreshold.h"

#include <itkConnectedThresholdImageFilter.h>
#include <itkImageFunction.h>

#include <dtkCore/dtkAbstractData.h>

namespace mseg {


AlgorithmConnectedThreshold::AlgorithmConnectedThreshold()
{

}

AlgorithmConnectedThreshold::~AlgorithmConnectedThreshold()
{

}

void AlgorithmConnectedThreshold::run()
{
    dtkAbstractData * absData;
}

template < typename TPixel, unsigned int VDimension > 
void AlgorithmConnectedThreshold::doFilter( dtkAbstractData * inData ){

    enum { NDim = VDimension } ;

    typedef itk::Image<TPixel, VDimension>      InputImageType;
    typedef itk::Image<signed char, VDimension> OutputImageType;
    typedef double                              OutputType;

    inData->data();

    typedef itk::ConnectedThresholdImageFilter<InputImageType, OutputImageType> ConnectedThresholdImageFilterType;
    typedef itk::ImageFunction<InputImageType, OutputType> ImageFunctionType;


    ImageFunctionType::Pointer imageFunc( ImageFunctionType::New() );
    ImageFunctionType::PointType point;

    do {
        point[0] = this->m_seedPoint.x();
        if ( NDim < 2 ) break;
        point[1] = this->m_seedPoint.y();
        if ( NDim < 3 ) break;
        point[2] = this->m_seedPoint.z();
    } while (false);

    ImageFunctionType::IndexType index;
    imageFunc->ConvertPointToNearestIndex (point, index);

    imageFunc->SetInputImage( inDataItk );

    ConnectedThresholdImageFilterType::Pointer ctiFilter( ConnectedThresholdImageFilterType::New() );

}

}
