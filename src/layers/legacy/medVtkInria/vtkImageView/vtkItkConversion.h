#pragma once

#include <medVtkInriaExport.h>

#include <itkImage.h>
#include <itkImageToVTKImageFilter.h>
#include <itkExtractImageFilter.h>

#include <vtkAlgorithmOutput.h>
#include <vtkMatrix4x4.h>

#include <vector>

class medAbstractData;

class MEDVTKINRIA_EXPORT vtkItkConversionInterface
{
public:
    virtual ~vtkItkConversionInterface() {};

    /*virtual bool SetITKInput(itk::ImageBase<3>::Pointer input) = 0;
    virtual bool SetITKInput(itk::ImageBase<4>::Pointer input) = 0;*/
    virtual bool SetITKInput(itk::DataObject::Pointer input) = 0;
    virtual bool GetConversion(vtkAlgorithmOutput *& po_poAlgoOut, vtkMatrix4x4  *&po_poMatrix) = 0;

    virtual bool setTimeIndex(unsigned int pi_uiTimeIndex) = 0;
    virtual unsigned int getTimeIndex() = 0;
    virtual unsigned int getNumberOfVolumes() = 0;
    virtual float getTotalTime() = 0;

    static vtkItkConversionInterface * creatInstance(medAbstractData* data);

protected:
    vtkItkConversionInterface() {};
};


template <typename volumeType, unsigned int imageDim>
class vtkItkConversion final : public vtkItkConversionInterface
{
    typedef typename itk::Image<volumeType, 4> Image4DType;
    typedef typename itk::Image<volumeType, 3> Image3DType;
    typedef itk::ImageToVTKImageFilter< itk::Image<volumeType, 3> > ConverterType;
private:
    /**
    This pointer is used to store internally a reference to the
    current ITK->VTK converter, in order to prevent the image buffer
    to be deleted unexpectedly. See the SetITKImageInXXX for more
    information.
    */
    typename ConverterType::Pointer m_ImageConverter;
    typename itk::Image<volumeType, 3>::Pointer m_ItkInputImage;

    typename itk::Image<volumeType, 4>::Pointer m_ItkInputImage4D;
    std::vector<typename itk::Image<volumeType, 3>::Pointer> m_oVolumeVectorFrom4D;
    unsigned int m_uiCurrentTimeIndex;
    unsigned int m_uiNbVolume;
    float m_fTotalTime;

public:
    vtkItkConversion();
    virtual ~vtkItkConversion();

    /**
    When ITK is set at ON, we propose the following method to open
    directly an ITK image. Because of the templates, we have to setup
    a method for each type of image. Up to now are supported 3D images
    of types: double, float, (unsigned) int, (unsigned) short and
    (unsigned) char. This class does the ITK->VTK conversion using
    the filter itk::ImageToVTKImageFilter (taken from InsightApplications).
    The problem with this filter is that if it is not registered,
    the output is erased along with the filter (in fact the output has
    the same registered count as the input. For instance, if the input
    image comes from an ITK reader, its reference count is set to 1.
    After conversion, one would like to erase the reader. If the
    converter is not referenced, the output buffer sees its ref count
    goes to 0, which result in a godamn seg fault. To overcome this,
    we reference the current converter in the pointer ImageConverter.
    */

    typedef itk::RGBPixel<unsigned char>  RGBPixelType;
    typedef itk::RGBAPixel<unsigned char> RGBAPixelType;
    typedef itk::Vector<unsigned char, 3> UCharVector3Type;
    typedef itk::Vector<float, 3> FloatVector3Type;

    virtual bool SetITKInput(typename itk::DataObject::Pointer input);
    //virtual bool SetITKInput4D(itk::ImageBase<4>::Pointer input);

    virtual bool GetConversion(vtkAlgorithmOutput *& po_poAlgoOut, vtkMatrix4x4  *&po_poMatrix);

    virtual bool setTimeIndex(unsigned int pi_uiTimeIndex);
    virtual unsigned int getTimeIndex();
    virtual unsigned int getNumberOfVolumes();
    virtual float getTotalTime();

private:

    bool initializeImage(typename itk::ImageBase<imageDim>::Pointer &input);
    /*bool initializeImage(itk::ImageBase<3>::Pointer &input);
    bool initializeImage3D(itk::ImageBase<3>::Pointer &input);
    bool initializeImage4D(itk::ImageBase<4>::Pointer &input);*/
    bool volumeExtraction();
    void conversion();
};

#include <vtkItkConversion.tpp>