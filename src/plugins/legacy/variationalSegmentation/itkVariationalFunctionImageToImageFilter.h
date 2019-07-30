/*=========================================================================

  Program:   vtkINRIA3D
  Module:    $Id: itkVariationalFunctionImageToImageFilter.h 967 2008-09-30 09:40:16Z ntoussaint $
  Language:  C++
  Author:    $Author: ntoussaint $
  Date:      $Date: 2008-09-30 11:40:16 +0200 (Tue, 30 Sep 2008) $
  Version:   $Revision: 967 $

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

  =========================================================================*/
#ifndef __itkVariationalFunctionImageToImageFilter_h
#define __itkVariationalFunctionImageToImageFilter_h

#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIterator.h>
#include <itkImageToImageFilter.h>

namespace itk
{

template <class TInputImage>
class ITK_EXPORT VariationalFunctionImageToImageFilter:
        public ImageToImageFilter < TInputImage, TInputImage>
{
    
public:
    typedef VariationalFunctionImageToImageFilter         Self;
    typedef ImageToImageFilter<TInputImage, TInputImage> Superclass;
    typedef SmartPointer<Self>       Pointer;
    typedef SmartPointer<const Self> ConstPointer;

    itkNewMacro(Self)
    itkTypeMacro (VariationalFunctionImageToImageFilter, ImageToImageFilter)
    itkStaticConstMacro(ImageDimension, unsigned int, TInputImage::ImageDimension);
    
    /** Image typedefs support */
    typedef double ScalarType;
    typedef TInputImage                            ImageType;
    typedef typename TInputImage::PixelType        PixelType;
    typedef ImageRegionConstIterator<ImageType>    InputIteratorType;
    typedef ImageRegionIterator<ImageType>         OutputIteratorType;
    typedef typename InputIteratorType::IndexType  InputIndexType;
    typedef typename OutputIteratorType::IndexType OutputIndexType;
    typedef typename ImageType::RegionType         OutputImageRegionType;
    typedef typename ImageType::PointType          PointType;
    typedef Vector<PixelType, ImageDimension>      VectorType;

    typedef std::pair<PointType, ScalarType>       ConstraintType;
    typedef std::vector<ConstraintType>            ConstraintListType;
    typedef vnl_matrix< ScalarType >               InternalMatrixType;
    typedef vnl_vector< ScalarType >               InternalVectorType;
    typedef vnl_svd< ScalarType >                  SolverType;

    void SetConstraints (ConstraintListType constraints)
    {
        m_Constraints = constraints;
        this->Modified();
    }
    ConstraintType GetConstraints (void)
    {
        return m_Constraints;
    }

protected:
    VariationalFunctionImageToImageFilter();
    ~VariationalFunctionImageToImageFilter();
    void PrintSelf(std::ostream &os, Indent indent) const;

    void BeforeThreadedGenerateData(void);
    void ThreadedGenerateData(const OutputImageRegionType &outputRegionForThread, ThreadIdType threadId);

    InternalMatrixType EstimateConstraintMatrix(ConstraintListType constraints);
    ScalarType EstimatePhi(VectorType r);
    
private:
    VariationalFunctionImageToImageFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented

    ConstraintListType m_Constraints;
    InternalVectorType m_LinearSolution;
};


} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkVariationalFunctionImageToImageFilter.txx"
#endif

#endif
