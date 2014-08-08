/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkSphericalHarmonicVisuManager.h>

#include <vtkSphericalHarmonicGlyph.h>
#include <vtkExtractVOI.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>

#include <vtkImageData.h>
#include <vtkLookupTable.h>
#include <vtkProperty.h>

vtkCxxRevisionMacro(vtkSphericalHarmonicVisuManager, "$Revision: 0 $");
vtkStandardNewMacro(vtkSphericalHarmonicVisuManager);

vtkSphericalHarmonicVisuManager::vtkSphericalHarmonicVisuManager() {

    SHSource = vtkSphericalHarmonicSource::New();
    SHGlyph  = vtkSphericalHarmonicGlyph::New();
    VOI    = vtkExtractVOI::New();
    Mapper = vtkPolyDataMapper::New();
    Actor  = vtkActor::New();

    SHSource->SetTesselationType(vtkSphericalHarmonicSource::Icosahedron);
    SHSource->SetTesselationBasis(vtkSphericalHarmonicSource::SHMatrix);
    SHSource->SetTesselation(2);
    SHSource->UpdateSphericalHarmonicSource();

    SHSource->SetNormalize(true);

    SHGlyph->SetSourceConnection(SHSource->GetOutputPort());
    SHGlyph->SetScaleFactor(3.0);
    SHGlyph->SetSphericalHarmonicSource(SHSource);
    SHGlyph->SetColorModeToDirections();
    SHGlyph->GetOutput()->GetPointData()
        ->SetActiveScalars(vtkSphericalHarmonicGlyph::GetRGBArrayName());

    VOI->ReleaseDataFlagOn();

    SHGlyph->SetInputConnection(VOI->GetOutputPort());

    vtkLookupTable *lut = vtkLookupTable::New();
    //lut->SetHueRange(0.667, 0.0);
    lut->SetHueRange(0.0, 1.0);
    lut->SetRange(0.0, 1.0);
    lut->Build();

    Mapper->SetInputConnection(SHGlyph->GetOutputPort());
    Mapper->ScalarVisibilityOn();
    Mapper->SetColorModeToMapScalars();
    Mapper->SetScalarModeToUsePointData();
    Mapper->SetLookupTable(lut);

    Mapper->SetScalarRange (0.0, 256);

    lut->Delete();

    MatrixT=0;

    Actor->SetMapper(Mapper);
    Actor->GetProperty()->SetSpecular(0.7);
    Actor->GetProperty()->SetSpecularPower(10.0);
    Actor->GetProperty()->SetInterpolationToGouraud();
    Actor->GetProperty()->SetOpacity(1);
}

vtkSphericalHarmonicVisuManager::~vtkSphericalHarmonicVisuManager() {

    if (SHGlyph)
        SHGlyph->Delete();
    if (SHSource)
        SHSource->Delete();
    if (VOI)
        VOI->Delete();
    if (Mapper)
        Mapper->Delete();
    if (Actor)
        Actor->Delete();
    if (MatrixT)
        MatrixT->Delete();
}

void vtkSphericalHarmonicVisuManager::SetInput (vtkImageData* vtkSH) {

    SHGlyph->SetTMatrix(MatrixT);

    if (!vtkSH)
        return;

    const int number = vtkSH->GetPointData()->
                       GetArray(vtkSphericalHarmonicGlyph::GetSphericalHarmonicCoefficientsArrayName())->
                       GetNumberOfComponents();

    SHSource->SetNumberOfSphericalHarmonics(number);

    VOI->SetInputData (vtkSH);
    VOI->Update();
}


void vtkSphericalHarmonicVisuManager::SetVOI(const int& imin, const int& imax,
                                             const int& jmin, const int& jmax,
                                             const int& kmin, const int& kmax)
{
    VOI->SetVOI(imin,imax,jmin,jmax,kmin,kmax);
}

void vtkSphericalHarmonicVisuManager::SetGlyphScale (const float& scale) {
    SHGlyph->SetScaleFactor(scale);
}

void vtkSphericalHarmonicVisuManager::SetTesselationType (const int& type) {
    SHSource->SetTesselationType (type);
    SHSource->UpdateSphericalHarmonicSource();
}

void vtkSphericalHarmonicVisuManager::SetTesselationBasis (const int& type) {
    SHSource->SetTesselationBasis(type);
    SHSource->UpdateSphericalHarmonicSource();
}

void vtkSphericalHarmonicVisuManager::SetGlyphResolution (const int& res) {
    SHSource->SetTesselation(res);
    SHSource->UpdateSphericalHarmonicSource();
}

void vtkSphericalHarmonicVisuManager::SetOrder (const int& order) {
    SHSource->SetOrder(order);
    SHSource->UpdateSphericalHarmonicSource();
}

void vtkSphericalHarmonicVisuManager::SetSampleRate (const int& n1,const int& n2,const int& n3) {
    VOI->SetSampleRate(n1,n2,n3);
}

void vtkSphericalHarmonicVisuManager::FlipX (const int& a) {
    bool* flip = SHSource->GetFlipVector();
    flip[0] = a;
    SHSource->SetFlipVector(flip);
    SHSource->UpdateSphericalHarmonicSource();
}

void vtkSphericalHarmonicVisuManager::FlipY (const int& a) {
    bool* flip = SHSource->GetFlipVector();
    flip[1] = a;
    SHSource->SetFlipVector(flip);
    SHSource->UpdateSphericalHarmonicSource();
}

void vtkSphericalHarmonicVisuManager::FlipZ (const int& a) {
    bool* flip = SHSource->GetFlipVector();
    flip[2] = a;
    SHSource->SetFlipVector(flip);
    SHSource->UpdateSphericalHarmonicSource();
}

void vtkSphericalHarmonicVisuManager::ColorGlyphs (const int& a) {
    SHGlyph->SetColorGlyphs(a);
}

void vtkSphericalHarmonicVisuManager::SetNormalization (const int& a) {
    SHSource->SetNormalize(a);
}
