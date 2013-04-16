/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkSphericalHarmonicVisuManager.h>

#include <vtkSphericalHarmonicGlyph.h>
#include <vtkPolyDataNormals.h>
#include <vtkExtractVOI.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>

#include <vtkImageData.h>
#include <vtkLookupTable.h>
#include <vtkProperty.h>

vtkCxxRevisionMacro(vtkSphericalHarmonicVisuManager, "$Revision: 0 $");
vtkStandardNewMacro(vtkSphericalHarmonicVisuManager);

vtkSphericalHarmonicVisuManager::vtkSphericalHarmonicVisuManager()
{
  this->SHSource = vtkSphericalHarmonicSource::New();
  this->SHGlyph  = vtkSphericalHarmonicGlyph::New();
  this->Normals  = vtkPolyDataNormals::New();
  this->VOI    = vtkExtractVOI::New();
  this->Mapper = vtkPolyDataMapper::New();
  this->Actor  = vtkActor::New();

  this->SHSource->SetTesselationType( vtkSphericalHarmonicSource::Icosahedron );
  this->SHSource->SetTesselationBasis( vtkSphericalHarmonicSource::SHMatrix );

  this->SHSource->SetTesselation( 3 );
  this->SHSource->UpdateSphericalHarmonicSource();

  this->SHSource->SetNormalize(true);
  this->SHGlyph->SetSource ( this->SHSource->GetOutput() );
  this->SHGlyph->SetScaleFactor( 1.0 );
  this->SHGlyph->SetSphericalHarmonicSource( this->SHSource );
  this->SHGlyph->SetColorModeToDirections();
  this->SHGlyph->GetOutput()->GetPointData()
      ->SetActiveScalars(vtkSphericalHarmonicGlyph::GetRGBArrayName());

  this->VOI->ReleaseDataFlagOn();

  this->SHGlyph->SetInput( this->VOI->GetOutput() );

  this->Normals->SetInput( this->SHGlyph->GetOutput() );

  vtkLookupTable *lut = vtkLookupTable::New();
  //lut->SetHueRange(0.667, 0.0);
  lut->SetHueRange(0.0, 1.0);
  lut->SetRange(0.0, 1.0);
  lut->Build();

  this->Mapper->SetInput( this->Normals->GetOutput() );
  this->Mapper->ScalarVisibilityOn();
  this->Mapper->SetColorModeToMapScalars();
  this->Mapper->SetScalarModeToUsePointData();
  this->Mapper->SetLookupTable(lut);

  this->Mapper->SetScalarRange (0.0, 256);

  lut->Delete();

  this->MatrixT=0;

  this->Actor->SetMapper( this->Mapper );
  this->Actor->GetProperty()->SetSpecular(0.7);
  this->Actor->GetProperty()->SetSpecularPower(10.0);
  this->Actor->GetProperty()->SetInterpolationToGouraud();
  this->Actor->GetProperty()->SetOpacity(1);
}

vtkSphericalHarmonicVisuManager::~vtkSphericalHarmonicVisuManager()
{
  if(this->SHGlyph)
    this->SHGlyph->Delete();
  if(this->SHSource)
    this->SHSource->Delete();
  if(this->Normals)
    this->Normals->Delete();
  if(this->VOI)
    this->VOI->Delete();
  if(this->Mapper)
    this->Mapper->Delete();
  if(this->Actor)
    this->Actor->Delete();
  if(this->MatrixT)
    this->MatrixT->Delete();
}

void vtkSphericalHarmonicVisuManager::SetInput (vtkImageData* vtkSH)
{
  this->SHGlyph->SetTMatrix(MatrixT);

  if( !vtkSH )
  {
    return;
  }

  int number =
      vtkSH->GetPointData()
      ->GetArray(vtkSphericalHarmonicGlyph::GetSphericalHarmonicCoefficientsArrayName())
      ->GetNumberOfComponents();

  this->SHSource->SetNumberOfSphericalHarmonics( number );

  this->VOI->SetInput ( vtkSH );
}


void vtkSphericalHarmonicVisuManager::SetVOI(const int& imin, const int& imax,
                                             const int& jmin, const int& jmax,
                                             const int& kmin, const int& kmax)
{
  this->VOI->SetVOI(imin,imax,jmin,jmax,kmin,kmax);
}

void vtkSphericalHarmonicVisuManager::SetGlyphScale (const float& scale)
{
  this->SHGlyph->SetScaleFactor(scale);
}

void vtkSphericalHarmonicVisuManager::SetTesselationType (const int& type)
{
  this->SHSource->SetTesselationType (type);
  this->SHSource->UpdateSphericalHarmonicSource();
}

void vtkSphericalHarmonicVisuManager::SetTesselationBasis (const int& type)
{
  this->SHSource->SetTesselationBasis(type);
  this->SHSource->UpdateSphericalHarmonicSource();
}

void vtkSphericalHarmonicVisuManager::SetGlyphResolution (const int& res)
{
  this->SHSource->SetTesselation( res/2 );
  this->SHSource->UpdateSphericalHarmonicSource();
}

void vtkSphericalHarmonicVisuManager::SetOrder (const int& order)
{
  this->SHSource->SetOrder(order);
  this->SHSource->UpdateSphericalHarmonicSource();
}

void vtkSphericalHarmonicVisuManager::SetSampleRate (const int& n1,const int& n2, const int& n3)
{
  this->VOI->SetSampleRate(n1, n2, n3);
}

void vtkSphericalHarmonicVisuManager::FlipX (const int& a)
{
  if (a)
    this->SHSource->FlipXOn();
  else
    this->SHSource->FlipXOff();
  this->SHSource->UpdateSphericalHarmonicSource();
}

void vtkSphericalHarmonicVisuManager::FlipY (const int& a)
{
  if (a)
    this->SHSource->FlipYOn();
  else
    this->SHSource->FlipYOff();
  this->SHSource->UpdateSphericalHarmonicSource();
}

void vtkSphericalHarmonicVisuManager::FlipZ (const int& a)
{
  if( a )
    this->SHSource->FlipZOn();
  else
    this->SHSource->FlipZOff();
  this->SHSource->UpdateSphericalHarmonicSource();
}

void vtkSphericalHarmonicVisuManager::ColorGlyphs (const int& a)
{
  if( a )
    this->SHGlyph->SetColorGlyphs(true);
  else
    this->SHGlyph->SetColorGlyphs(false);
//  this->SHSource->UpdateSphericalHarmonicSource();
}

void vtkSphericalHarmonicVisuManager::SetNormalization (const int& a)
{
    if (a)
        this->SHSource->NormalizeOn();
      else
        this->SHSource->NormalizeOff();
//      this->SHSource->UpdateSphericalHarmonicSource();
}

