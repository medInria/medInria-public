/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkSphericalHarmonicManager.h>

#include <vtkObjectFactory.h>

vtkCxxRevisionMacro(vtkSphericalHarmonicManager, "$Revision: 477 $");
vtkStandardNewMacro(vtkSphericalHarmonicManager);

vtkSphericalHarmonicManager::vtkSphericalHarmonicManager() {
    this->RenderWindowInteractor = 0;

    this->Input    = 0;
    this->MatrixT  = 0;
    this->Renderer = 0;

    this->SHVisuManagerAxial    = vtkSphericalHarmonicVisuManager::New();
    this->SHVisuManagerSagittal = vtkSphericalHarmonicVisuManager::New();
    this->SHVisuManagerCoronal  = vtkSphericalHarmonicVisuManager::New();

    for(int i=0;i<3;++i)
        this->CurrentPosition[i] = 0;
}

vtkSphericalHarmonicManager::~vtkSphericalHarmonicManager() {

    if (this->RenderWindowInteractor)
        this->RenderWindowInteractor->Delete();

    if (this->Input)
        this->Input->Delete();

    if (this->MatrixT)
        this->MatrixT->Delete();

    if (this->SHVisuManagerAxial)
        this->SHVisuManagerAxial->Delete();

    if (this->SHVisuManagerSagittal)
        this->SHVisuManagerSagittal->Delete();

    if (this->SHVisuManagerCoronal)
        this->SHVisuManagerCoronal->Delete();
}

void
vtkSphericalHarmonicManager::SetRenderWindowInteractor(vtkRenderWindowInteractor* rwin,vtkRenderer* ren) {

    if (rwin!=this->RenderWindowInteractor) {
        if (this->RenderWindowInteractor!=NULL) {
            this->Initialize();
            this->RenderWindowInteractor->UnRegister(this);
        }

        this->RenderWindowInteractor = rwin;
        if (this->RenderWindowInteractor)
            this->RenderWindowInteractor->Register(this);
    }

    if (ren) {
        this->Renderer = ren;
        return;
    }

    if (this->RenderWindowInteractor) {
        this->RenderWindowInteractor->GetRenderWindow()->GetRenderers()->InitTraversal();
        vtkRenderer* first_renderer = this->RenderWindowInteractor->GetRenderWindow()
            ->GetRenderers()->GetNextItem();

        int numLayers = this->RenderWindowInteractor->GetRenderWindow()->GetNumberOfLayers();
        this->RenderWindowInteractor->GetRenderWindow()->SetNumberOfLayers(numLayers + 1);

        this->Renderer = vtkRenderer::New();
        this->Renderer->SetLayer(numLayers);
        if (first_renderer)
          this->Renderer->SetActiveCamera(first_renderer->GetActiveCamera());

        this->RenderWindowInteractor->GetRenderWindow()->AddRenderer(this->Renderer);

        this->Renderer->Delete();
    }
}

void vtkSphericalHarmonicManager::Initialize() {

    if (this->Renderer && this->RenderWindowInteractor) {
        this->Renderer->RemoveActor(this->SHVisuManagerAxial->GetActor());
        this->Renderer->RemoveActor(this->SHVisuManagerSagittal->GetActor());
        this->Renderer->RemoveActor(this->SHVisuManagerCoronal->GetActor());
    }
}

void vtkSphericalHarmonicManager::Update() {

    if (!this->Input || !this->RenderWindowInteractor)
        return;

    this->Initialize();
    int* dims = this->Input->GetDimensions();

    const int X = clamp(this->CurrentPosition[0],dims[0]);
    const int Y = clamp(this->CurrentPosition[1],dims[1]);
    const int Z = clamp(this->CurrentPosition[2],dims[2]);

    this->SHVisuManagerAxial->SetMatrixT(this->MatrixT);
    this->SHVisuManagerSagittal->SetMatrixT(this->MatrixT);
    this->SHVisuManagerCoronal->SetMatrixT(this->MatrixT);

    // synchronize with VOI
    this->SHVisuManagerSagittal->SetVOI(X, X, 0, dims[1]-1, 0, dims[2]-1);
    this->SHVisuManagerCoronal ->SetVOI(0, dims[0]-1, Y, Y, 0, dims[2]-1);
    this->SHVisuManagerAxial   ->SetVOI(0, dims[0]-1, 0, dims[1]-1, Z, Z);

    // finally set the data
    this->SHVisuManagerAxial->SetInput(this->Input);
    this->SHVisuManagerSagittal->SetInput(this->Input);
    this->SHVisuManagerCoronal->SetInput(this->Input);

    if (this->Renderer) {
        this->Renderer->AddActor(this->SHVisuManagerAxial->GetActor());
        this->Renderer->AddActor(this->SHVisuManagerSagittal->GetActor());
        this->Renderer->AddActor(this->SHVisuManagerCoronal->GetActor());
    }
}

void
vtkSphericalHarmonicManager::SetCurrentPosition(const int& X, const int& Y, const int& Z) {

    if (!this->Input)
        return;

    int* dims = this->Input->GetDimensions();

    // clamping

    const int XX = clamp(X,dims[0]);
    const int YY = clamp(Y,dims[1]);
    const int ZZ = clamp(Z,dims[2]);

    // synchronize with VOI
    this->SHVisuManagerSagittal->SetVOI(XX, XX, 0, dims[1]-1, 0, dims[2]-1);
    this->SHVisuManagerCoronal ->SetVOI(0, dims[0]-1, YY, YY, 0, dims[2]-1);
    this->SHVisuManagerAxial   ->SetVOI(0, dims[0]-1, 0, dims[1]-1, ZZ, ZZ);

    this->CurrentPosition[0] = X;
    this->CurrentPosition[1] = Y;
    this->CurrentPosition[2] = Z;
}

void vtkSphericalHarmonicManager::SetCurrentPosition(const int pos[3]) {
    this->SetCurrentPosition(pos[0], pos[1], pos[2]);
}

void vtkSphericalHarmonicManager::ResetPosition() {
    if (!this->Input)
        return;

    int* dims = this->Input->GetDimensions();

    const int X = dims[0]/2;
    const int Y = dims[1]/2;
    const int Z = dims[2]/2;

    this->SetCurrentPosition(X,Y,Z);
}

void vtkSphericalHarmonicManager::SetSampleRate(const int& n1,const int& n2, const int& n3) {
    this->SHVisuManagerAxial   ->SetSampleRate(n1, n2, n3);
    this->SHVisuManagerSagittal->SetSampleRate(n1, n2, n3);
    this->SHVisuManagerCoronal ->SetSampleRate(n1, n2, n3);
}

void vtkSphericalHarmonicManager::SetGlyphResolution(const int res) {
    this->SHVisuManagerAxial   ->SetGlyphResolution(res);
    this->SHVisuManagerSagittal->SetGlyphResolution(res);
    this->SHVisuManagerCoronal ->SetGlyphResolution(res);
}

void vtkSphericalHarmonicManager::SetTesselationType(const int& type) {
    this->SHVisuManagerAxial   ->SetTesselationType(type);
    this->SHVisuManagerSagittal->SetTesselationType(type);
    this->SHVisuManagerCoronal ->SetTesselationType(type);
}

void vtkSphericalHarmonicManager::SetTesselationBasis(const int& type) {
    this->SHVisuManagerAxial   ->SetTesselationBasis(type);
    this->SHVisuManagerSagittal->SetTesselationBasis(type);
    this->SHVisuManagerCoronal ->SetTesselationBasis(type);
}

void vtkSphericalHarmonicManager::SetOrder(const int order) {
    this->SHVisuManagerAxial   ->SetOrder(order);
    this->SHVisuManagerSagittal->SetOrder(order);
    this->SHVisuManagerCoronal ->SetOrder(order);
}

void vtkSphericalHarmonicManager::FlipX(const bool a) {
    this->SHVisuManagerAxial   ->FlipX(a);
    this->SHVisuManagerSagittal->FlipX(a);
    this->SHVisuManagerCoronal ->FlipX(a);
}

void vtkSphericalHarmonicManager::FlipY(const bool a) {
    this->SHVisuManagerAxial   ->FlipY(a);
    this->SHVisuManagerSagittal->FlipY(a);
    this->SHVisuManagerCoronal ->FlipY(a);
}

void vtkSphericalHarmonicManager::FlipZ(const bool a) {
    this->SHVisuManagerAxial   ->FlipZ(a);
    this->SHVisuManagerSagittal->FlipZ(a);
    this->SHVisuManagerCoronal ->FlipZ(a);
}

void vtkSphericalHarmonicManager:: Normalization(const bool a) {
    this->SHVisuManagerAxial   ->SetNormalization(a);
    this->SHVisuManagerSagittal->SetNormalization(a);
    this->SHVisuManagerCoronal ->SetNormalization(a);
}

void vtkSphericalHarmonicManager::ColorGlyphs(const bool a) {
    this->SHVisuManagerAxial   ->ColorGlyphs(a);
    this->SHVisuManagerSagittal->ColorGlyphs(a);
    this->SHVisuManagerCoronal ->ColorGlyphs(a);
}

void vtkSphericalHarmonicManager::GetSphericalHarmonicDimensions(int * dims) {
    if (this->Input!=0)
        this->Input->GetDimensions(dims);
}

void vtkSphericalHarmonicManager::SetAxialSliceVisibility(const int i) {
    this->SHVisuManagerAxial->GetActor()->SetVisibility(i);
}

void vtkSphericalHarmonicManager::SetSagittalSliceVisibility(const int i) {
    this->SHVisuManagerSagittal->GetActor()->SetVisibility(i);
}

void vtkSphericalHarmonicManager::SetCoronalSliceVisibility(const int i) {
    this->SHVisuManagerCoronal->GetActor()->SetVisibility(i);
}

void vtkSphericalHarmonicManager::SetGlyphScale(const float& scale) {
    this->SHVisuManagerAxial   ->SetGlyphScale(scale);
    this->SHVisuManagerSagittal->SetGlyphScale(scale);
    this->SHVisuManagerCoronal ->SetGlyphScale(scale);
}
