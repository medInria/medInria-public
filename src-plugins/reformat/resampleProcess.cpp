#include "resampleProcess.h"
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractDataFactory.h>
#include <medAbstractProcess.h>
#include <medMetaDataKeys.h>
#include <medUtilities.h>

#include <itkResampleImageFilter.h>
#include <itkBSplineInterpolateImageFunction.h>

// /////////////////////////////////////////////////////////////////
// resampleProcessPrivate
// /////////////////////////////////////////////////////////////////
class resampleProcessPrivate
{
public:
    resampleProcess *parent;
    resampleProcessPrivate(resampleProcess *p)
    {
        parent = p;
    }
    medAbstractData* input;
    medAbstractData* output;
    int interpolator;
    int dimX,dimY,dimZ;
    double spacingX,spacingY,spacingZ;
};

// /////////////////////////////////////////////////////////////////
// resampleProcess
// /////////////////////////////////////////////////////////////////
resampleProcess::resampleProcess(void) : d(new resampleProcessPrivate(this))
{
    d->dimX=0;
    d->dimY=0;
    d->dimZ=0;
    d->spacingX = 0.0;
    d->spacingY = 0.0;
    d->spacingZ = 0.0;
}

resampleProcess::~resampleProcess(void)
{
    delete d;
    d = NULL;
}
bool resampleProcess::registered(void)
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("resampleProcess", createResampleProcess);
}
QString resampleProcess::description(void) const
{
    return "resampleProcess";
}
void resampleProcess::setInput ( medAbstractData *data)
{
    if ( !data )
        return;
    d->input = data;
}

void resampleProcess::setInput ( medAbstractData *data , int channel)
{
    setInput(data);
}

void resampleProcess::setParameter ( double data, int channel)
{
    switch (channel)
    {
    case 0:
        d->dimX = (int)data;
        break;
    case 1:
        d->dimY = (int)data;
        break;
    case 2:
        d->dimZ = (int)data;
        break;
    case 3:
        d->spacingX = data;
        break;
    case 4:
        d->spacingY = data;
        break;
    case 5:
        d->spacingZ = data;
        break;
    }
}

int resampleProcess::update ( void )
{
    if ( !d->input )
    {
        qDebug() << "in update method : d->input is NULL";
        return DTK_FAILURE;
    }
    QString type = QString (d->input->identifier());

    if ( type == "itkDataImageChar3" )
    {
        resample<itk::Image<char, 3> >("itkDataImageChar3");
    }
    else if ( type == "itkDataImageUChar3" )
    {
        resample<itk::Image<unsigned char, 3> >("itkDataImageUChar3");
    }
    else if ( type == "itkDataImageShort3" )
    {
        resample<itk::Image<short, 3> >("itkDataImageShort3");
    }
    else if ( type == "itkDataImageUShort3" )
    {
        resample<itk::Image<unsigned short, 3> >("itkDataImageUShort3");
    }
    else if ( type == "itkDataImageInt3" )
    {
        resample<itk::Image<int, 3> >("itkDataImageInt3");
    }
    else if ( type == "itkDataImageUInt3" )
    {
        resample<itk::Image<unsigned int, 3> >("itkDataImageUInt3");
    }
    else if ( type == "itkDataImageLong3" )
    {
        resample<itk::Image<long, 3> >("itkDataImageLong3");
    }
    else if ( type == "itkDataImageULong3" )
    {
        resample<itk::Image<unsigned long, 3> >("itkDataImageULong3");
    }
    else if ( type == "itkDataImageFloat3" )
    {
        resample<itk::Image<float, 3> >("itkDataImageFloat3");
    }
    else if ( type == "itkDataImageDouble3" )
    {
        resample<itk::Image<double, 3> >("itkDataImageDouble3" );
    }
    else if ( type == "itkDataImageChar4" )
    {
        resample<itk::Image<char, 4> >("itkDataImageChar4");
    }
    else if ( type == "itkDataImageUChar4" )
    {
        resample<itk::Image<unsigned char, 4> >("itkDataImageUChar4");
    }
    else if ( type == "itkDataImageShort4" )
    {
        resample<itk::Image<short, 4> >("itkDataImageShort4");
    }
    else if ( type == "itkDataImageUShort4" )
    {
        resample<itk::Image<unsigned short, 4> >("itkDataImageUShort4");
    }
    else if ( type == "itkDataImageInt4" )
    {
        resample<itk::Image<int, 4> >("itkDataImageInt4");
    }
    else if ( type == "itkDataImageUInt4" )
    {
        resample<itk::Image<unsigned int, 4> >("itkDataImageUInt4");
    }
    else if ( type == "itkDataImageLong4" )
    {
        resample<itk::Image<long, 4> >("itkDataImageLong4");
    }
    else if ( type == "itkDataImageULong4" )
    {
        resample<itk::Image<unsigned long, 4> >("itkDataImageULong4");
    }
    else if ( type == "itkDataImageFloat4" )
    {
        resample<itk::Image<float, 4> >("itkDataImageFloat4");
    }
    else if ( type == "itkDataImageDouble4" )
    {
        resample<itk::Image<double, 4> >("itkDataImageDouble4" );
    }
    return DTK_SUCCEED;
}

template <class ImageType> void resampleProcess::resample(const char *str)
{
    typename ImageType::Pointer inputImage =dynamic_cast<ImageType*>((itk::Object*)(d->input->data()));
    
    typedef typename itk::ResampleImageFilter<ImageType, ImageType,double> ResampleFilterType;

    typename ResampleFilterType::Pointer resampleFilter = ResampleFilterType::New();
    
    //// Fetch original image size.
    const typename ImageType::RegionType& inputRegion = inputImage->GetLargestPossibleRegion();
    const typename ImageType::SizeType& vnInputSize = inputRegion.GetSize();
    unsigned int nOldX = vnInputSize[0];
    unsigned int nOldY = vnInputSize[1];
    unsigned int nOldZ = vnInputSize[2];

    //// Fetch original image spacing.
    const typename ImageType::SpacingType& vfInputSpacing = inputImage->GetSpacing();
    double vfOutputSpacing[3]={d->spacingX,d->spacingY,d->spacingZ};
    if (d->dimX || d->dimY || d->dimZ)
    {
        vfOutputSpacing[0] = vfInputSpacing[0] * (double) nOldX / (double) d->dimX;
        vfOutputSpacing[1] = vfInputSpacing[1] * (double) nOldY / (double) d->dimY;
        vfOutputSpacing[2] = vfInputSpacing[2] * (double) nOldZ / (double) d->dimZ;
    }
    else
    {
        d->dimX =  floor((vfInputSpacing[0] * (double) nOldX / (double) vfOutputSpacing[0]) +0.5);
        d->dimY =  floor((vfInputSpacing[1] * (double) nOldY / (double) vfOutputSpacing[1]) +0.5);
        d->dimZ =  floor((vfInputSpacing[2] * (double) nOldZ / (double) vfOutputSpacing[2]) +0.5);
    }
    
    typename ImageType::SizeType vnOutputSize;
    vnOutputSize[0] = d->dimX;
    vnOutputSize[1] = d->dimY;
    vnOutputSize[2] = d->dimZ;
    
    resampleFilter->SetInput(inputImage);
    resampleFilter->SetSize(vnOutputSize);
    resampleFilter->SetOutputSpacing(vfOutputSpacing);
    resampleFilter->SetOutputOrigin( inputImage->GetOrigin() );
    resampleFilter->SetOutputDirection(inputImage->GetDirection() );
    resampleFilter->UpdateLargestPossibleRegion();
    d->output = medAbstractDataFactory::instance()->create(str);
    d->output->setData(resampleFilter->GetOutput());
    medUtilities::setDerivedMetaData(d->output, d->input, "resampled");
}

medAbstractData * resampleProcess::output ( void )
{
    return ( d->output );
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////
dtkAbstractProcess *createResampleProcess(void)
{
    return new resampleProcess;
}
