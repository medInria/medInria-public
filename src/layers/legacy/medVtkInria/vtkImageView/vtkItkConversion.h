#pragma once

#include <medVtkInriaExport.h>

#include <itkImage.h>
#include <itkRGBPixel.h>
#include <itkRGBAPixel.h>

#include <vtkType.h>
#include <vtkAlgorithmOutput.h>
#include <vtkMatrix4x4.h>

class MEDVTKINRIA_EXPORT vtkItkConversion
{
public:
    vtkItkConversion();
    ~vtkItkConversion();

    //////////////////////////////////////////////////////////////////////////
    //FloTODO split itk reading phase from vtkImage to an other class
    //////////////////////////////////////////////////////////////////////////

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

    bool GetConversion(int pi_iLayer, vtkAlgorithmOutput *& po_poAlgoOut, vtkMatrix4x4  *&po_poMatrix);

    virtual bool SetITKInput (itk::Image<double, 3>::Pointer input, int pi_iLayer);
    virtual bool SetITKInput (itk::Image<float, 3>::Pointer input, int pi_iLayer);
    virtual bool SetITKInput (itk::Image<int, 3>::Pointer input, int pi_iLayer);
    virtual bool SetITKInput (itk::Image<unsigned int, 3>::Pointer input, int pi_iLayer);
    virtual bool SetITKInput (itk::Image<short, 3>::Pointer input, int pi_iLayer);
    virtual bool SetITKInput (itk::Image<unsigned short, 3>::Pointer input, int pi_iLayer);
    virtual bool SetITKInput (itk::Image<long, 3>::Pointer input, int pi_iLayer);
    virtual bool SetITKInput (itk::Image<unsigned long, 3>::Pointer input, int pi_iLayer);
    virtual bool SetITKInput (itk::Image<char, 3>::Pointer input, int pi_iLayer);
    virtual bool SetITKInput (itk::Image<unsigned char, 3>::Pointer input, int pi_iLayer);
    virtual bool SetITKInput (itk::Image<RGBPixelType, 3>::Pointer input, int pi_iLayer);
    virtual bool SetITKInput (itk::Image<RGBAPixelType, 3>::Pointer input, int pi_iLayer);
    virtual bool SetITKInput (itk::Image<UCharVector3Type, 3>::Pointer input, int pi_iLayer);
    virtual bool SetITKInput (itk::Image<FloatVector3Type, 3>::Pointer input, int pi_iLayer);
    
    itk::ImageBase<3>*  GetITKInput (int layer = 0) const;



private:
    //! Template function which implements SetInput for all types.
    template < class T > bool SetITKInput(typename itk::Image<T, 3>::Pointer itkImage, int pi_iLayer);





    //! Template function which sets the time step.
    template < class T >
    void SetTimeIndex(vtkIdType timeIndex);
    //////////////////////////////////////////////////////////////////////////
    //End of FloTODO split itk reading phase from vtkImage to an other class
    //////////////////////////////////////////////////////////////////////////




private:
    /**
    This pointer is used to store internally a reference to the
    current ITK->VTK converter, in order to prevent the image buffer
    to be deleted unexpectedly. See the SetITKImageInXXX for more
    information.
    */
    std::map<int, itk::ProcessObject::Pointer> ImageConverter;
    std::map<int, vtkAlgorithmOutput *> ImageConverterOutput;
    std::vector< itk::ImageBase<3>::Pointer> ITKInputVector;
};

inline vtkItkConversion::vtkItkConversion()
{
}

inline vtkItkConversion::~vtkItkConversion()
{
}