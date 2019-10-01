/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: itkVariationalFunctionImageToImageFilter.txx 1403 2010-01-18 16:11:13Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2010-01-18 17:11:13 +0100 (Mon, 18 Jan 2010) $
Version:   $Revision: 1403 $

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkVariationalFunctionImageToImageFilter_txx
#define _itkVariationalFunctionImageToImageFilter_txx

#include <itkVariationalFunctionImageToImageFilter.h>
#include <itkImageRegionIterator.h>
#include <itkImageRegionConstIterator.h>

namespace itk
{

// ----------------------------------------------------------------------
// Constructor
template <class TInputImage>
VariationalFunctionImageToImageFilter<TInputImage>
::VariationalFunctionImageToImageFilter()
{
    this->DynamicMultiThreadingOff();
}

// ----------------------------------------------------------------------
// Destructor
template <class TInputImage>
VariationalFunctionImageToImageFilter<TInputImage>
::~VariationalFunctionImageToImageFilter()
{
}

// ----------------------------------------------------------------------
// PrintSelf
template <class TInputImage>
void VariationalFunctionImageToImageFilter<TInputImage>
::PrintSelf(std::ostream &os, Indent indent) const
{
    Superclass::PrintSelf(os, indent);
}

// ----------------------------------------------------------------------
// BeforeThreadedGenerateData
template <class TInputImage>
void VariationalFunctionImageToImageFilter<TInputImage>
::BeforeThreadedGenerateData()
{
    // Preliminary test
    if( ImageDimension != 3 )
    {
        throw itk::ExceptionObject (__FILE__,__LINE__,"Error: Dimension not supported.");
    }

    // Images
    const ImageType *input = this->GetInput();
    ImageType      *output = this->GetOutput();

    // Copy image geometry from input to output image
    output->SetRegions(   input->GetLargestPossibleRegion() );
    output->SetOrigin(    input->GetOrigin() );
    output->SetSpacing(   input->GetSpacing() );
    output->SetDirection( input->GetDirection() );
    
    // We create the linear equation system Ax=b
    unsigned int n_constraints = m_Constraints.size();
    unsigned int system_size = n_constraints + 4;
    InternalMatrixType A = this->EstimateConstraintMatrix(m_Constraints);
    InternalVectorType b(system_size);
    b.fill(0.0);
    
    for (unsigned int i = 0; i < n_constraints; i++)
    {
        b[i] = m_Constraints[i].second;
    }
    
    SolverType solver (A);
    m_LinearSolution = solver.solve (b);
}


// ----------------------------------------------------------------------
// ThreadedGenerateData
template <class TInputImage>
void VariationalFunctionImageToImageFilter<TInputImage>
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, ThreadIdType threadId)
{
    typename ImageType::Pointer output = this->GetOutput();

    OutputIteratorType itOut(const_cast<ImageType*>(this->GetOutput()), outputRegionForThread);
    PointType point, constraint;
    OutputIndexType index;
    PixelType value;

    unsigned int n_constraints = m_Constraints.size();
    unsigned int system_size = n_constraints + 4;
    
    while(!itOut.IsAtEnd())
    {
        index = itOut.GetIndex();
        output->TransformIndexToPhysicalPoint (index, point);
        value = 0;

        for (unsigned int i = 0; i < n_constraints; i++)
        {
            constraint = m_Constraints[i].first;
            value += m_LinearSolution[i] * this->EstimatePhi(point - constraint);
        }

        value += 1.0 * m_LinearSolution[system_size-4]
                + point[0] * m_LinearSolution[system_size-3]
                + point[1] * m_LinearSolution[system_size-2]
                + point[2] * m_LinearSolution[system_size-1];

        itOut.Set (value);
        ++itOut;
    }
    
}

// ----------------------------------------------------------------------
// EstimateConstraintMatrix
template <class TInputImage>
typename VariationalFunctionImageToImageFilter<TInputImage>::InternalMatrixType
VariationalFunctionImageToImageFilter<TInputImage>
::EstimateConstraintMatrix (ConstraintListType constraints)
{
    unsigned int n_constraints = constraints.size();
    unsigned int system_size = n_constraints + 4;
    
    InternalMatrixType ret (system_size, system_size);
    
    for (unsigned int i = 0; i < n_constraints; i++)
    {
        PointType pi = constraints[i].first;
        for (unsigned int j = 0; j < n_constraints; j++)
        {
            PointType pj = constraints[j].first;
            ret[i][j] = this->EstimatePhi(pi - pj);
        }
        ret[i][system_size-4] = 1.0;
        ret[i][system_size-3] = pi[0];
        ret[i][system_size-2] = pi[1];
        ret[i][system_size-1] = pi[2];
    }

    for (unsigned int i = 0; i < n_constraints; i++)
    {
        ret[system_size-4][i] = 1.0;
    }

    ret[system_size-4][system_size-4] = 0.0;
    ret[system_size-4][system_size-3] = 0.0;
    ret[system_size-4][system_size-2] = 0.0;
    ret[system_size-4][system_size-1] = 0.0;

    for (unsigned int i = 3; i > 0; i--)
    {
        for (unsigned int j = 0; j < n_constraints; j++)
        {
            PointType pj = constraints[j].first;
            ret[system_size-i][j] = pj[3-i];
        }
        ret[system_size-i][system_size-4] = 0.0;
        ret[system_size-i][system_size-3] = 0.0;
        ret[system_size-i][system_size-2] = 0.0;
        ret[system_size-i][system_size-1] = 0.0;
    }

    return ret;
}

// ----------------------------------------------------------------------
// EstimatePhi
template <class TInputImage>
typename VariationalFunctionImageToImageFilter<TInputImage>::ScalarType
VariationalFunctionImageToImageFilter<TInputImage>
::EstimatePhi (VectorType r)
{
    ScalarType a = r[0]*r[0]+r[1]*r[1]+r[2]*r[2];
    return ( a * std::sqrt(a) );
}

} // end namespace itk

#endif
