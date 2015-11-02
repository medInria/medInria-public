/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkSphericalHarmonicManager.h>
#include <vtkObjectFactory.h>
#include <vtkMath.h>


vtkStandardNewMacro(vtkSphericalHarmonicManager);

vtkSphericalHarmonicManager::vtkSphericalHarmonicManager() {
    RenderWindowInteractor = 0;

    Input    = 0;
    DirectionMatrix = 0;
    Renderer = 0;

    SHVisuManagerAxial    = vtkSphericalHarmonicVisuManager::New();
    SHVisuManagerSagittal = vtkSphericalHarmonicVisuManager::New();
    SHVisuManagerCoronal  = vtkSphericalHarmonicVisuManager::New();

    this->PhysicalToVoxelCoordinatesTransformMatrix = vtkMatrix4x4::New();
    this->PhysicalToVoxelCoordinatesTransformMatrix->Identity();

    for(int i=0;i<3;++i)
        CurrentPosition[i] = 0;
}

vtkSphericalHarmonicManager::~vtkSphericalHarmonicManager() {

    this->Initialize();

    if (RenderWindowInteractor)
        RenderWindowInteractor->Delete();

    if (Input)
        Input->Delete();

    if (DirectionMatrix)
        DirectionMatrix->Delete();

    if (SHVisuManagerAxial)
        SHVisuManagerAxial->Delete();

    if (SHVisuManagerSagittal)
        SHVisuManagerSagittal->Delete();

    if (SHVisuManagerCoronal)
        SHVisuManagerCoronal->Delete();
}

void
vtkSphericalHarmonicManager::SetRenderWindowInteractor(vtkRenderWindowInteractor* rwin,vtkRenderer* ren) {

    if (rwin!=RenderWindowInteractor) {
        if (RenderWindowInteractor!=NULL) {
            Initialize();
            RenderWindowInteractor->UnRegister(this);
        }

        RenderWindowInteractor = rwin;
        if (RenderWindowInteractor)
            RenderWindowInteractor->Register(this);
    }

    if (ren) {
        Renderer = ren;
        return;
    }

    if (RenderWindowInteractor) {
        RenderWindowInteractor->GetRenderWindow()->GetRenderers()->InitTraversal();
        vtkRenderer* first_renderer = RenderWindowInteractor->GetRenderWindow()
            ->GetRenderers()->GetNextItem();

        int numLayers = RenderWindowInteractor->GetRenderWindow()->GetNumberOfLayers();
        RenderWindowInteractor->GetRenderWindow()->SetNumberOfLayers(numLayers + 1);

        Renderer = vtkRenderer::New();
        Renderer->SetLayer(numLayers);
        if (first_renderer)
            Renderer->SetActiveCamera(first_renderer->GetActiveCamera());

        RenderWindowInteractor->GetRenderWindow()->AddRenderer(Renderer);

        Renderer->Delete();
    }
}

void vtkSphericalHarmonicManager::Initialize() {

    if (Renderer && RenderWindowInteractor) {
        Renderer->RemoveActor(SHVisuManagerAxial->GetActor());
        Renderer->RemoveActor(SHVisuManagerSagittal->GetActor());
        Renderer->RemoveActor(SHVisuManagerCoronal->GetActor());
    }
}

void vtkSphericalHarmonicManager::Update() {

    if (!Input || !RenderWindowInteractor)
        return;

    Initialize();
    int* dims = Input->GetDimensions();

    const int X = clamp(CurrentPosition[0],dims[0]);
    const int Y = clamp(CurrentPosition[1],dims[1]);
    const int Z = clamp(CurrentPosition[2],dims[2]);

    SHVisuManagerAxial->SetMatrixT(DirectionMatrix);
    SHVisuManagerSagittal->SetMatrixT(DirectionMatrix);
    SHVisuManagerCoronal->SetMatrixT(DirectionMatrix);

    // synchronize with VOI
    SHVisuManagerSagittal->SetVOI(X, X, 0, dims[1]-1, 0, dims[2]-1);
    SHVisuManagerCoronal ->SetVOI(0, dims[0]-1, Y, Y, 0, dims[2]-1);
    SHVisuManagerAxial   ->SetVOI(0, dims[0]-1, 0, dims[1]-1, Z, Z);

    // finally set the data
    SHVisuManagerAxial->SetInput(Input);
    SHVisuManagerSagittal->SetInput(Input);
    SHVisuManagerCoronal->SetInput(Input);

    if (Renderer) {
        Renderer->AddActor(SHVisuManagerAxial->GetActor());
        Renderer->AddActor(SHVisuManagerSagittal->GetActor());
        Renderer->AddActor(SHVisuManagerCoronal->GetActor());
    }
}

void vtkSphericalHarmonicManager::SetDirectionMatrix(vtkMatrix4x4 *mat)
{
    if (!mat)
        return;

    vtkSetObjectBodyMacro(DirectionMatrix, vtkMatrix4x4, mat);

    vtkMatrix4x4::Invert(this->DirectionMatrix, this->PhysicalToVoxelCoordinatesTransformMatrix);
}

void
vtkSphericalHarmonicManager::SetCurrentPosition(const int& X, const int& Y, const int& Z) {

    if (!Input)
        return;

    int* dims = Input->GetDimensions();

    // clamping

    const int XX = clamp(X,dims[0]);
    const int YY = clamp(Y,dims[1]);
    const int ZZ = clamp(Z,dims[2]);

    // synchronize with VOI
    SHVisuManagerSagittal->SetVOI(XX, XX, 0, dims[1]-1, 0, dims[2]-1);
    SHVisuManagerCoronal ->SetVOI(0, dims[0]-1, YY, YY, 0, dims[2]-1);
    SHVisuManagerAxial   ->SetVOI(0, dims[0]-1, 0, dims[1]-1, ZZ, ZZ);

    CurrentPosition[0] = X;
    CurrentPosition[1] = Y;
    CurrentPosition[2] = Z;
}

void vtkSphericalHarmonicManager::SetCurrentPosition(const int pos[3]) {
    SetCurrentPosition(pos[0], pos[1], pos[2]);
}

void vtkSphericalHarmonicManager::SetCurrentPosition (const double pos[3])
{
  this->SetCurrentPosition (pos[0], pos[1], pos[2]);
}

void vtkSphericalHarmonicManager::SetCurrentPosition(const double& X, const double& Y, const double& Z)
{
    if( !this->Input )
    {
      return;
    }

    double *spacing = this->Input->GetSpacing();
    double *origin  = this->Input->GetOrigin();

    double pos[4]={X, Y, Z, 1.0};

    this->PhysicalToVoxelCoordinatesTransformMatrix->MultiplyPoint(pos, pos);

    int vox_pos[3];
    for (int i=0; i<3; i++)
        vox_pos[i] = vtkMath::Round((pos[i]-origin[i])/spacing[i]);

    this->SetCurrentPosition(vox_pos[0], vox_pos[1], vox_pos[2]);
}

void vtkSphericalHarmonicManager::ResetPosition() {
    if (!Input)
        return;

    int* dims = Input->GetDimensions();

    const int X = dims[0]/2;
    const int Y = dims[1]/2;
    const int Z = dims[2]/2;

    SetCurrentPosition(X,Y,Z);
}

void vtkSphericalHarmonicManager::SetSampleRate(const int& n1,const int& n2, const int& n3) {
    SHVisuManagerAxial   ->SetSampleRate(n1, n2, n3);
    SHVisuManagerSagittal->SetSampleRate(n1, n2, n3);
    SHVisuManagerCoronal ->SetSampleRate(n1, n2, n3);
}

void vtkSphericalHarmonicManager::SetGlyphResolution(const int res) {
    SHVisuManagerAxial   ->SetGlyphResolution(res);
    SHVisuManagerSagittal->SetGlyphResolution(res);
    SHVisuManagerCoronal ->SetGlyphResolution(res);
}

void vtkSphericalHarmonicManager::SetTesselationType(const int& type) {
    SHVisuManagerAxial   ->SetTesselationType(type);
    SHVisuManagerSagittal->SetTesselationType(type);
    SHVisuManagerCoronal ->SetTesselationType(type);
}

void vtkSphericalHarmonicManager::SetTesselationBasis(const int& type) {
    SHVisuManagerAxial   ->SetTesselationBasis(type);
    SHVisuManagerSagittal->SetTesselationBasis(type);
    SHVisuManagerCoronal ->SetTesselationBasis(type);
}

void vtkSphericalHarmonicManager::SetOrder(const int order) {
    SHVisuManagerAxial   ->SetOrder(order);
    SHVisuManagerSagittal->SetOrder(order);
    SHVisuManagerCoronal ->SetOrder(order);
}

void vtkSphericalHarmonicManager::FlipX(const bool a) {
    SHVisuManagerAxial   ->FlipX(a);
    SHVisuManagerSagittal->FlipX(a);
    SHVisuManagerCoronal ->FlipX(a);
}

void vtkSphericalHarmonicManager::FlipY(const bool a) {
    SHVisuManagerAxial   ->FlipY(a);
    SHVisuManagerSagittal->FlipY(a);
    SHVisuManagerCoronal ->FlipY(a);
}

void vtkSphericalHarmonicManager::FlipZ(const bool a) {
    SHVisuManagerAxial   ->FlipZ(a);
    SHVisuManagerSagittal->FlipZ(a);
    SHVisuManagerCoronal ->FlipZ(a);
}

void vtkSphericalHarmonicManager:: Normalization(const bool a) {
    SHVisuManagerAxial   ->SetNormalization(a);
    SHVisuManagerSagittal->SetNormalization(a);
    SHVisuManagerCoronal ->SetNormalization(a);
}

void vtkSphericalHarmonicManager::ColorGlyphs(const bool a) {
    SHVisuManagerAxial   ->ColorGlyphs(a);
    SHVisuManagerSagittal->ColorGlyphs(a);
    SHVisuManagerCoronal ->ColorGlyphs(a);
}

void vtkSphericalHarmonicManager::GetSphericalHarmonicDimensions(int * dims) {
    if (Input!=0)
        Input->GetDimensions(dims);
}

void vtkSphericalHarmonicManager::SetAxialSliceVisibility(const int i) {
    SHVisuManagerAxial->GetActor()->SetVisibility(i);
}

void vtkSphericalHarmonicManager::SetSagittalSliceVisibility(const int i) {
    SHVisuManagerSagittal->GetActor()->SetVisibility(i);
}

void vtkSphericalHarmonicManager::SetCoronalSliceVisibility(const int i) {
    SHVisuManagerCoronal->GetActor()->SetVisibility(i);
}

void vtkSphericalHarmonicManager::SetGlyphScale(const float& scale) {
    SHVisuManagerAxial   ->SetGlyphScale(scale);
    SHVisuManagerSagittal->SetGlyphScale(scale);
    SHVisuManagerCoronal ->SetGlyphScale(scale);
}
