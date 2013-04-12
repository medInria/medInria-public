/*=========================================================================

  Program:   SphericalHarmonic ToolKit - SphericalHarmonicInria
  Module:    Jaime Garcia Guevara based on "itkTensorViewerFactory.h"
  Language:  C++
  Date:      $Date: 2011-04-08 12:28:58 +0200 (Fri, 08 Apr 2011) $
  Version:   $Revision: 151 $

  Copyright (c) INRIA 2010. All rights reserved.
  See LICENSE.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#pragma once

#include <vtkPointData.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkFloatArray.h>

#include <itkImage.h>
#include <itkImageToImageFilter.h>
#include <itkImageRegionIterator.h>
#include <itkImageRegionConstIteratorWithIndex.h>

#include <vtkSphericalHarmonicGlyph.h>
#include <itkSphericalHarmonicITKToVTKFilter.h>


namespace itk
{
/**
 * Constructor
 */
template < class TSHImage >
SphericalHarmonicITKToVTKFilter< TSHImage >
::SphericalHarmonicITKToVTKFilter()
{
  m_VTKSphericalHarmonic = vtkImageData::New();
  m_DirectionMatrix = vtkMatrix4x4::New();
  m_DirectionMatrix->Identity();
}

/**
 * Destructor
 */
template < class TSHImage >
SphericalHarmonicITKToVTKFilter< TSHImage >
::~SphericalHarmonicITKToVTKFilter()
{
  m_VTKSphericalHarmonic->Delete();
  m_DirectionMatrix->Delete();
}

/**
 * Set a vtkImageData as input
 */
template < class TSHImage >
void
SphericalHarmonicITKToVTKFilter< TSHImage >
::CopyVTKSphericalHarmonic( vtkImageData* p )
{
  p->DeepCopy(m_VTKSphericalHarmonic);
}

/**
 * Get an itk::Image as output
 */
template < class TSHImage >
typename SphericalHarmonicITKToVTKFilter<TSHImage>::SHImageType*
SphericalHarmonicITKToVTKFilter< TSHImage >
::GetOutput()
{
  return const_cast<TSHImage*>( this->GetInput() );
}

/**
 * Do the conversion
 */
template < class TSHImage >
void
SphericalHarmonicITKToVTKFilter< TSHImage >
::GenerateData()
{

  SHImageConstPointer inputSH = this->GetInput(0);
  if ( ! inputSH )
  {
    throw itk::ExceptionObject (__FILE__,__LINE__,"Error: No input ITK SphericalHarmonic image has been set.");
  }

  // conversion itkVectorImage -> vtkImageData
  //        vtkImageData *sh_points = vtkImageData::New();
  vtkFloatArray *sh_array        = vtkFloatArray::New();
  vtkFloatArray *aniso_array = vtkFloatArray::New();
  vtkFloatArray *rgb_array = vtkFloatArray::New();

  typename SHImageType::SizeType   size    = inputSH->GetLargestPossibleRegion().GetSize();
  typename SHImageType::SpacingType spacing = inputSH->GetSpacing();
  typename SHImageType::PointType  origin  = inputSH->GetOrigin();
  typename TSHImage::DirectionType directions = inputSH->GetDirection();

  int numVoxels=1;
  double pt[3], sp[3];
  for( unsigned int i=0; i<3; i++)
  {
    numVoxels *= size[i];
    pt[i] = 0; /*do not use origin[i] coz if the directions does not match it wont be useful better keep em both in m_DirectionMatrix*/
    sp[i] = spacing[i];
  }

  m_VTKSphericalHarmonic->SetDimensions(size[0], size[1], size[2]);
  m_VTKSphericalHarmonic->SetSpacing( sp );
  m_VTKSphericalHarmonic->SetOrigin( pt );

  for (int i=0; i<3; i++)
    for (int j=0; j<3; j++)
        m_DirectionMatrix->SetElement (i, j, directions(i,j));

  for (int i=0; i<3; i++)
   m_DirectionMatrix->SetElement (i, 3,origin[i]);

//    for (int l=0; l<4;l++)
//      std::cout << m_DirectionMatrix->GetElement(l,0) << m_DirectionMatrix->GetElement(l,1)
//                << m_DirectionMatrix->GetElement(l,2) << m_DirectionMatrix->GetElement(l,3)<<std::endl;

  sh_array->SetName( vtkSphericalHarmonicGlyph::GetSphericalHarmonicCoefficientsArrayName() );
  sh_array->SetNumberOfComponents( inputSH->GetNumberOfComponentsPerPixel() );
  sh_array->SetNumberOfTuples( size[0]*size[1]*size[2] );

  aniso_array->SetName(vtkSphericalHarmonicGlyph::GetAnisotropyMeasureArrayName());
  aniso_array->SetNumberOfComponents(1);
  aniso_array->SetNumberOfTuples(size[0]*size[1]*size[2]);

  rgb_array->SetName(vtkSphericalHarmonicGlyph::GetRGBArrayName());
  rgb_array->SetNumberOfComponents(1);
  rgb_array->SetNumberOfTuples(size[0]*size[1]*size[2]);

  typedef itk::ImageRegionConstIteratorWithIndex<SHImageType> IteratorType;
  IteratorType it (inputSH, inputSH->GetLargestPossibleRegion());

  this->UpdateProgress ( 0.0 );

  unsigned long  step = 15*numVoxels/100;
  int count = 0;

  while ( !it.IsAtEnd() )
  {
    SHType sphericalHarmonic = it.Get();

    double *c = new double[ sphericalHarmonic.Size() ];
    for( unsigned int i=0; i<sphericalHarmonic.Size(); i++)
    {
      c[i] = sphericalHarmonic[i];
    }

    sh_array->InsertTuple (count, c);

    float gfa = 0.0;
    float sum = 0.0;
    float c0 = sphericalHarmonic[0];

    for(unsigned int i = 0; i < sphericalHarmonic.Size(); i++)
    {
      sum += sphericalHarmonic[i]*sphericalHarmonic[i];
    }

    if(sum > 0.0)
    {
      gfa = std::sqrt(1 - c0*c0 / sum);
    }

    aniso_array->InsertTuple1(count, gfa);
    rgb_array->InsertTuple1(count, gfa);

    delete [] c;

    ++count;
    ++it;

    if( (count%step) == 0 )
    {
      this->UpdateProgress ( double(count)/double(numVoxels) );
    }
  }

  m_VTKSphericalHarmonic->GetPointData()->AddArray(sh_array);
  m_VTKSphericalHarmonic->GetPointData()->AddArray(aniso_array);
  m_VTKSphericalHarmonic->GetPointData()->AddArray(rgb_array);

  sh_array->Delete();
  aniso_array->Delete();
  rgb_array->Delete();
  m_VTKSphericalHarmonic->Update();
}


template < class TSHImage >
vtkMatrix4x4*
SphericalHarmonicITKToVTKFilter< TSHImage >
::GetDirectionMatrix() const
{
  return m_DirectionMatrix;
}

} // end namespace itk



