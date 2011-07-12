#include "msegAlgorithmConnectedThreshold.h"

#include "msegAlgorithmConnectedThresholdParametersWidget.h"
#include "msegController.h"

#include <itkConnectedThresholdImageFilter.h>
#include <itkImageFunction.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractProcessFactory.h>


namespace mseg {

template < typename TPixel, unsigned int VDimension > class AlgorithmConnectedThresholdPrivate : public HandlerFunc {
public:
    AlgorithmConnectedThresholdPrivate( AlgorithmConnectedThreshold * self_ ) : self(self_) { }

    // Override base.
    int run( dtkAbstractData * inData ) MED_OVERRIDE;
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

QString AlgorithmConnectedThreshold::s_typeName()
{
    return AlgorithmConnectedThreshold::staticMetaObject.className();
}

dtkAbstractProcess * AlgorithmConnectedThreshold::create()
{
    return new AlgorithmConnectedThreshold;
}

QString AlgorithmConnectedThreshold::localizedName()
{
    return AlgorithmConnectedThreshold::s_localizedName(this);
}

QString AlgorithmConnectedThreshold::s_localizedName(const QObject * trObj)
{
    if (!trObj) 
        trObj = qApp;

    return trObj->tr( "Connected Threshold" );
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

    ConnectedThresholdImageFilterType::Pointer ctiFilter( ConnectedThresholdImageFilterType::New() );

    foreach (const QVector3D & seedPoint, self->m_seedPoints ){
        do {
            point[0] = seedPoint.x();
            if ( NDim < 2 ) break;
            point[1] = seedPoint.y();
            if ( NDim < 3 ) break;
            point[2] = seedPoint.z();
        } while (false);

        ImageFunctionType::IndexType index;
        bool isInside = image->TransformPhysicalPointToIndex (point, index);

        ctiFilter->AddSeed( index );
    }

    ctiFilter->SetInput( image );
    ctiFilter->Update();

    OutputImageType::Pointer outputImage( ctiFilter->GetOutput() );
    QString outputTypeName = QString("itkDataImageChar%1").arg(VDimension,1);

    dtkSmartPointer< dtkAbstractData> newData( dtkAbstractDataFactory::instance()->create( outputTypeName ) );
    newData->setData( image.GetPointer() );

    self->setOutput( newData );

    return DTK_SUCCEED;
}


} //namespace mseg
