/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2017. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include <cmath>

#include "vtkImageView.h"
#include "vtkObjectFactory.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkMatrixToLinearTransform.h"

#include "vtkImageData.h"
#include "vtkPointSet.h"
#include <vtkInformation.h>
#include <vtkStreamingDemandDrivenPipeline.h>

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyle.h"
#include "vtkImageViewCornerAnnotation.h"
#include "vtkTextProperty.h"
#include "vtkCamera.h"
#include "vtkDataSetCollection.h"
#include "vtkProp3DCollection.h"

#include "vtkLookupTable.h"
#include "vtkColorTransferFunction.h"
#include "vtkPiecewiseFunction.h"
#include "vtkImageMapToColors.h"
#include "vtkScalarBarActor.h"
#include <vtkImageReslice.h>
#include <vtkSmartPointer.h>

#include "vtkCommand.h"
#include "vtkImageView2DCommand.h"

#include <itkExtractImageBufferFilter.h>
#include <itkImageToVTKImageFilter.h>
#include <itkMatrix.h>
#include <vtkRendererCollection.h>
#include <vtkImageReader2.h>

//#include <vnl/algo/vnl_qr.h>

#ifdef WIN32
#define snprintf _snprintf_s
#endif









// Enumeration for the supported pixel types
// NT: why not using the vtk IO definitions ?
namespace {
    enum ImageViewType {
        IMAGE_VIEW_NONE = 0,
        IMAGE_VIEW_DOUBLE = 1,
        IMAGE_VIEW_FLOAT,
        IMAGE_VIEW_INT,
        IMAGE_VIEW_UNSIGNED_INT,
        IMAGE_VIEW_SHORT,
        IMAGE_VIEW_UNSIGNED_SHORT,
        IMAGE_VIEW_LONG,
        IMAGE_VIEW_UNSIGNED_LONG,
        IMAGE_VIEW_CHAR,
        IMAGE_VIEW_UNSIGNED_CHAR,
        IMAGE_VIEW_RGBPIXELTYPE,
        IMAGE_VIEW_RGBAPIXELTYPE,
        IMAGE_VIEW_UCHARVECTOR3TYPE,
        IMAGE_VIEW_FLOATVECTOR3TYPE
    };
}


// pIMPL class for vtkImageView
class vtkImageView::vtkImageViewImplementation {
public:
    //! Default constructor
    vtkImageViewImplementation() : TemporalFilterType(IMAGE_VIEW_NONE) {}

    std::map<int, itk::ProcessObject::Pointer> ImageConverter;
    // should not be handled in this class.
    std::map<int, itk::ProcessObject::Pointer> ImageTemporalFilter;
    int TemporalFilterType;

public:
    template < typename T > static ImageViewType GetImageViewType();
public:
};

// Template specialisations return the enumeration corresponding to the pixel type.
// NT: Use the VTK_DOUBLE/SHORT/etc definitions ?... or the itkImageIO:ComponentType since we have ITK there
template <> ImageViewType vtkImageView::vtkImageViewImplementation::GetImageViewType < double >() { return     IMAGE_VIEW_DOUBLE; }
template <> ImageViewType vtkImageView::vtkImageViewImplementation::GetImageViewType < float >() { return     IMAGE_VIEW_FLOAT; }
template <> ImageViewType vtkImageView::vtkImageViewImplementation::GetImageViewType < int >() { return     IMAGE_VIEW_INT; }
template <> ImageViewType vtkImageView::vtkImageViewImplementation::GetImageViewType < unsigned int >() { return     IMAGE_VIEW_UNSIGNED_INT; }
template <> ImageViewType vtkImageView::vtkImageViewImplementation::GetImageViewType < short >() { return     IMAGE_VIEW_SHORT; }
template <> ImageViewType vtkImageView::vtkImageViewImplementation::GetImageViewType < unsigned short >() { return     IMAGE_VIEW_UNSIGNED_SHORT; }
template <> ImageViewType vtkImageView::vtkImageViewImplementation::GetImageViewType < long >() { return     IMAGE_VIEW_LONG; }
template <> ImageViewType vtkImageView::vtkImageViewImplementation::GetImageViewType < unsigned long >() { return     IMAGE_VIEW_UNSIGNED_LONG; }
template <> ImageViewType vtkImageView::vtkImageViewImplementation::GetImageViewType < char >() { return     IMAGE_VIEW_CHAR; }
template <> ImageViewType vtkImageView::vtkImageViewImplementation::GetImageViewType < unsigned char >() { return     IMAGE_VIEW_UNSIGNED_CHAR; }
template <> ImageViewType vtkImageView::vtkImageViewImplementation::GetImageViewType < vtkImageView::RGBPixelType >() { return     IMAGE_VIEW_RGBPIXELTYPE; }
template <> ImageViewType vtkImageView::vtkImageViewImplementation::GetImageViewType < vtkImageView::RGBAPixelType >() { return     IMAGE_VIEW_RGBAPIXELTYPE; }
template <> ImageViewType vtkImageView::vtkImageViewImplementation::GetImageViewType < vtkImageView::UCharVector3Type >() { return     IMAGE_VIEW_UCHARVECTOR3TYPE; }
template <> ImageViewType vtkImageView::vtkImageViewImplementation::GetImageViewType < vtkImageView::FloatVector3Type >() { return     IMAGE_VIEW_FLOATVECTOR3TYPE; }








//----------------------------------------------------------------------------
template < class T >
inline void vtkImageView::SetITKInput (typename itk::Image<T, 3>::Pointer itkImage, int layer)
{
    if(itkImage.IsNull())
        return;

    itkImage->UpdateOutputInformation();

    if (layer < (int)(this->ITKInputVector.size()) && (this->ITKInputVector[layer]==itkImage) )
        return;

    typedef itk::ImageToVTKImageFilter< itk::Image<T, 3> > ConverterType;
    typename ConverterType::Pointer myConverter = ConverterType::New();
    myConverter->SetInput ( itkImage );
    myConverter->UpdateOutputInformation();
    /**
     The origin in ITK pipeline is taken into account in a different
     way than in the VTK equivalent.
     A first hack would be to force the vtkImageData instance to have
     a null origin, and put the ITK origin in the 4th column of the
     OrientationMatrix instance. BUT, when the ITK pipeline is updated,
     then the ITK origin is put back in place in the vtkImageData instance.

     Therefore we need to keep the vtkImageData's origin the same as the
     ITK one. And, we need to correct for this misbehaviour through a hack
     in the OrientationMatrix 4th column, a sort of corrected origin.
  */
    typename itk::ImageBase<3>::DirectionType directions = itkImage->GetDirection();
    typename itk::ImageBase<3>::PointType origin = itkImage->GetOrigin();
    vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
    matrix->Identity();
    for (int i=0; i<3; i++)
        for (int j=0; j<3; j++)
            matrix->SetElement (i, j, directions (i,j));
    double v_origin[4], v_origin2[4];
    for (int i=0; i<3; i++)
        v_origin[i] = origin[i];
    v_origin[3] = 1.0;
    matrix->MultiplyPoint (v_origin, v_origin2);
    for (int i=0; i<3; i++)
        matrix->SetElement (i, 3, v_origin[i]-v_origin2[i]);
    /**
     The matrix instance is from now on corrected.
  */
    myConverter->GetImporter()->UpdateInformation();
    myConverter->GetImporter()->Update();

    this->SetInput(myConverter->GetImporter()->GetOutputPort(), matrix, layer);
    this->Impl->ImageConverter[layer] = myConverter;

    if((int)(this->ITKInputVector.size()) <= layer) {
        this->ITKInputVector.resize (layer+1);
        this->ITKInputVector[layer] = itkImage;
    }
    else
        this->ITKInputVector[layer] = itkImage;
    this->Modified();
    matrix->Delete();
}


template < class T >
inline void vtkImageView::AddITKInput (typename itk::Image<T, 3>::Pointer itkImage)
{
    int layer = GetNumberOfLayers();
    SetITKInput<T>(itkImage, layer);
}

template < class T >
inline void vtkImageView::SetITKInput4 (typename itk::Image<T, 4>::Pointer itkImage, int layer)
{
    if( itkImage.IsNull() )
    {
        return;
    }

    // if (this->ITKInput4==itkImage) // if SetITKInput is called after SetITKInput4, pointer is not changed
    // return;                        // and image is not displayed

    typedef typename itk::Image<T, 4> ImageType4d;
    typedef typename itk::Image<T, 3> ImageType3d;
    typedef typename itk::ExtractImageBufferFilter< ImageType4d, ImageType3d > ExtractFilterType;

    typename ExtractFilterType::Pointer extractor (ExtractFilterType::New());
    // extractor->SetNumberOfThreads(1); // not thread safe, to correct // extractor is now not multi-threaded
    typename ImageType4d::SizeType size = itkImage->GetLargestPossibleRegion().GetSize();

    unsigned int timeIndex = this->TimeIndex;
    unsigned int timeLimit = itkImage->GetLargestPossibleRegion().GetSize()[3]-1;

    if (timeIndex>timeLimit)
        timeIndex = timeLimit;

    typename ImageType4d::IndexType index = {{0,0,0, timeIndex}};

    size[3] = 0;
    typename ImageType4d::RegionType region;
    region.SetSize (size);
    region.SetIndex (index);

    extractor->SetExtractionRegion (region);
    extractor->SetDirectionCollapseToGuess();
    extractor->SetInput ( itkImage );

    this->Impl->ImageTemporalFilter[layer] = extractor;
    this->Impl->TemporalFilterType = vtkImageView::vtkImageViewImplementation::GetImageViewType <T> ();
    this->ITKInput4 = itkImage;
    typename ImageType3d::Pointer itkImage3 = extractor->GetOutput ();
    extractor->UpdateLargestPossibleRegion();
    this->SetITKInput ( itkImage3, layer);
    // itkImage3->DisconnectPipeline();
}

template < class T >
inline void vtkImageView::AddITKInput4 (typename itk::Image<T, 4>::Pointer itkImage)
{
    int layer = GetNumberOfLayers();
    SetITKInput4<T>(itkImage, layer);
}

#define vtkImplementSetITKInputMacro(type)				\
    void vtkImageView::SetITKInput (itk::Image<type, 3>::Pointer itkImage, int layer) \
{									\
    SetITKInput < type > (itkImage, layer);					\
    }
vtkImplementSetITKInputMacro (double);
vtkImplementSetITKInputMacro (float);
vtkImplementSetITKInputMacro (int);
vtkImplementSetITKInputMacro (unsigned int);
vtkImplementSetITKInputMacro (short);
vtkImplementSetITKInputMacro (unsigned short);
vtkImplementSetITKInputMacro (long);
vtkImplementSetITKInputMacro (unsigned long);
vtkImplementSetITKInputMacro (char);
vtkImplementSetITKInputMacro (unsigned char);
vtkImplementSetITKInputMacro (RGBPixelType);
vtkImplementSetITKInputMacro (RGBAPixelType);
vtkImplementSetITKInputMacro (UCharVector3Type);
vtkImplementSetITKInputMacro (FloatVector3Type);

#define vtkImplementAddITKInputMacro(type)				\
    void vtkImageView::AddITKInput (itk::Image<type, 3>::Pointer itkImage) \
{									\
    AddITKInput < type > (itkImage);					\
    }
vtkImplementAddITKInputMacro (double);
vtkImplementAddITKInputMacro (float);
vtkImplementAddITKInputMacro (int);
vtkImplementAddITKInputMacro (unsigned int);
vtkImplementAddITKInputMacro (short);
vtkImplementAddITKInputMacro (unsigned short);
vtkImplementAddITKInputMacro (long);
vtkImplementAddITKInputMacro (unsigned long);
vtkImplementAddITKInputMacro (char);
vtkImplementAddITKInputMacro (unsigned char);
vtkImplementAddITKInputMacro (RGBPixelType);
vtkImplementAddITKInputMacro (RGBAPixelType);
vtkImplementAddITKInputMacro (UCharVector3Type);
vtkImplementAddITKInputMacro (FloatVector3Type);

itk::ImageBase<3>* vtkImageView::GetITKInput (int layer) const
{
    if (layer < (int)(this->ITKInputVector.size()))
        return this->ITKInputVector[layer];
    return NULL;
}

#define vtkImplementSetITKInput4Macro(type)				\
    void vtkImageView::SetITKInput4 (itk::Image<type, 4>::Pointer itkImage, int layer) \
{									\
    SetITKInput4 < type > (itkImage, layer);					\
    }

vtkImplementSetITKInput4Macro (double);
vtkImplementSetITKInput4Macro (float);
vtkImplementSetITKInput4Macro (int);
vtkImplementSetITKInput4Macro (unsigned int);
vtkImplementSetITKInput4Macro (short);
vtkImplementSetITKInput4Macro (unsigned short);
vtkImplementSetITKInput4Macro (long);
vtkImplementSetITKInput4Macro (unsigned long);
vtkImplementSetITKInput4Macro (char);
vtkImplementSetITKInput4Macro (unsigned char);
vtkImplementSetITKInput4Macro (RGBPixelType);
vtkImplementSetITKInput4Macro (RGBAPixelType);
vtkImplementSetITKInput4Macro (UCharVector3Type);
vtkImplementSetITKInput4Macro (FloatVector3Type);

#define vtkImplementAddITKInput4Macro(type)				\
    void vtkImageView::AddITKInput4 (itk::Image<type, 4>::Pointer itkImage) \
{									\
    AddITKInput4 < type > (itkImage);					\
    }

vtkImplementAddITKInput4Macro (double);
vtkImplementAddITKInput4Macro (float);
vtkImplementAddITKInput4Macro (int);
vtkImplementAddITKInput4Macro (unsigned int);
vtkImplementAddITKInput4Macro (short);
vtkImplementAddITKInput4Macro (unsigned short);
vtkImplementAddITKInput4Macro (long);
vtkImplementAddITKInput4Macro (unsigned long);
vtkImplementAddITKInput4Macro (char);
vtkImplementAddITKInput4Macro (unsigned char);
vtkImplementAddITKInput4Macro (RGBPixelType);
vtkImplementAddITKInput4Macro (RGBAPixelType);
vtkImplementAddITKInput4Macro (UCharVector3Type);
vtkImplementAddITKInput4Macro (FloatVector3Type);

itk::ImageBase<4>* vtkImageView::GetTemporalITKInput() const
{
    return this->ITKInput4;
}


