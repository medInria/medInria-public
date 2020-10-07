#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkImage.h>
#include <itkImageToImageFilter.h>

class vtkImageData;
class vtkMatrix4x4;

namespace itk
{

/** \class ITKSphericalHarmonicToVTKFilter
 * \brief  Convert an ITK SphericalHarmonic image into a VTK structured points
 *
 * \ingroup   ImageFilter
 */
template < class TSHImage >
class SphericalHarmonicITKToVTKFilter
    :public ImageToImageFilter< TSHImage, TSHImage >
{
public:
  /** Some typedefs. */
  typedef TSHImage                             SHImageType;
  typedef typename  SHImageType::PixelType     SHType;
  typedef typename  SHType::ValueType          ScalarType;
  typedef typename  SHImageType::Pointer       SHImagePointer;
  typedef typename  SHImageType::ConstPointer  SHImageConstPointer;

  /** Standard class typedefs. */
  typedef SphericalHarmonicITKToVTKFilter                         Self;
  typedef ImageToImageFilter<SHImageType,SHImageType>  Superclass;
  typedef SmartPointer<Self>                                   Pointer;
  typedef SmartPointer<const Self>                             ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self)

  /** Run-time type information (and related methods). */
  itkTypeMacro(SphericalHarmonicITKToVTKFilter, ImageToImageFilter)

  /** Return the input (no ITK filtering is performed on the input
     *  image) */
  SHImageType *GetOutput();

  /** Get the VTK SphericalHarmonic image obtained from the input ITK SphericalHarmonic
     *  image */
  void CopyVTKSphericalHarmonic( vtkImageData* );

  vtkImageData* GetVTKSphericalHarmonic() const
  { return m_VTKSphericalHarmonic; }

  /** Get the VTKMatrix4x4 obtained from the Itk image transformation that
     *  defines the image coordinate system*/ vtkMatrix4x4*
  GetDirectionMatrix() const;

protected:
  SphericalHarmonicITKToVTKFilter();
  virtual ~SphericalHarmonicITKToVTKFilter();

  /** Method that actually read the ITK data and convert it into a
     *  VTK structured point */
  virtual void GenerateData();

private:
  SphericalHarmonicITKToVTKFilter(const Self&);   //purposely not implemented
  void operator=(const Self&);                 //purposely not implemented

  /** VTK output */
  vtkImageData* m_VTKSphericalHarmonic;

  vtkMatrix4x4 *m_DirectionMatrix;

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include <itkSphericalHarmonicITKToVTKFilter.txx>
#endif
