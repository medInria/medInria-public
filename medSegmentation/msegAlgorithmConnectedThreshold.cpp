#include "msegAlgorithmConnectedThreshold.h"

#include <itkConnectedThresholdImageFilter.h>
#include <itkImageFunction.h>

#include <dtkCore/dtkAbstractData.h>

namespace mseg {

template < typename TPixel, unsigned int N > struct ConnectedThresholdHandler : public HandlerFunc {
    ConnectedThresholdHandler( AlgorithmConnectedThreshold * self_ ) : self(self_) { }

    // Override base.
    int run( dtkAbstractData * data );
    AlgorithmConnectedThreshold * self;
};

template < typename TPixel, unsigned int N > 
int ConnectedThresholdHandler< TPixel,N > ::run( dtkAbstractData * data )
{
    return DTK_SUCCEED;
}

AlgorithmConnectedThreshold::AlgorithmConnectedThreshold()
{
    MSEG_ADD_HANDLERS_FOR_ALL_SCALAR_3D_TYPES( ConnectedThresholdHandler, this );
}

AlgorithmConnectedThreshold::~AlgorithmConnectedThreshold()
{

}

void AlgorithmConnectedThreshold::run()
{
    this->callHandler(this->input() );
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
