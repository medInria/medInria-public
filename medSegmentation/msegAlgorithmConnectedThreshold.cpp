#include "msegAlgorithmConnectedThreshold.h"

#include <itkConnectedThresholdImageFilter.h>
#include <itkImageFunction.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

namespace mseg {

template < typename TPixel, unsigned int VDimension > class AlgorithmConnectedThresholdPrivate : public HandlerFunc {
public:
    AlgorithmConnectedThresholdPrivate( AlgorithmConnectedThreshold * self_ ) : self(self_) { }

    // Override base.
    int run( dtkAbstractData * inData );
private:
    AlgorithmConnectedThreshold * self;
};

AlgorithmConnectedThreshold::AlgorithmConnectedThreshold()
{
    MSEG_ADD_HANDLERS_FOR_ALL_SCALAR_3D_TYPES( AlgorithmConnectedThresholdPrivate, this );
}

AlgorithmConnectedThreshold::~AlgorithmConnectedThreshold()
{

}

void AlgorithmConnectedThreshold::run()
{
    this->callHandler(this->input() );
}

template < typename TPixel, unsigned int VDimension > 
int AlgorithmConnectedThresholdPrivate< TPixel,VDimension > ::run( dtkAbstractData * inData )
{
    enum { NDim = VDimension } ;

    typedef itk::Image<TPixel, VDimension>      InputImageType;
    typedef itk::Image<signed char, VDimension> OutputImageType;
    typedef double                              OutputType;

    typename InputImageType::Pointer image( static_cast< InputImageType *>( inData->data() ));

    typedef itk::ConnectedThresholdImageFilter<InputImageType, OutputImageType> ConnectedThresholdImageFilterType;
    typedef itk::ImageFunction<InputImageType, OutputType> ImageFunctionType;

    InputImageType::PointType point;

    do {
        point[0] = self->m_seedPoint.x();
        if ( NDim < 2 ) break;
        point[1] = self->m_seedPoint.y();
        if ( NDim < 3 ) break;
        point[2] = self->m_seedPoint.z();
    } while (false);

    ImageFunctionType::IndexType index;
    bool isInside = image->TransformPhysicalPointToIndex (point, index);

    ConnectedThresholdImageFilterType::Pointer ctiFilter( ConnectedThresholdImageFilterType::New() );
    ctiFilter->AddSeed( index );
    ctiFilter->SetInput( image );
    ctiFilter->Update();

    OutputImageType::Pointer outputImage( ctiFilter->GetOutput() );
    QString outputTypeName = QString("itkDataImageChar%1").arg(VDimension,1);

    dtkSmartPointer< dtkAbstractData> newData( dtkAbstractDataFactory::instance()->create( outputTypeName ) );
    newData->setData( image.GetPointer() );

    self->setOutput( newData );

    return DTK_SUCCEED;
}


}
