/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkVectorManager.h"

#include <vtkObjectFactory.h>
#include <vtkMath.h>


vtkStandardNewMacro(vtkVectorManager)


vtkVectorManager::vtkVectorManager()
{
    this->RenderWindowInteractor = 0;

    this->Input = 0;

    this->Renderer = 0;

    this->VectorVisuManagerAxial    = vtkVectorVisuManager::New();
    this->VectorVisuManagerSagittal = vtkVectorVisuManager::New();
    this->VectorVisuManagerCoronal  = vtkVectorVisuManager::New();

    this->DirectionMatrix = 0;

    this->PhysicalToVoxelCoordinatesTransformMatrix = vtkMatrix4x4::New();
    this->PhysicalToVoxelCoordinatesTransformMatrix->Identity();

    for( int i=0; i<3; i++)
    {
        this->CurrentPosition[i] = 0;
    }
}

vtkVectorManager::~vtkVectorManager()
{
    this->Initialize();

    if( this->RenderWindowInteractor )
    {
        this->RenderWindowInteractor->Delete();
    }

    if( this->Input )
    {
        this->Input->Delete();
    }

    this->VectorVisuManagerAxial->Delete();
    this->VectorVisuManagerSagittal->Delete();
    this->VectorVisuManagerCoronal->Delete();

    if (this->DirectionMatrix)
      this->DirectionMatrix->Delete();

    this->PhysicalToVoxelCoordinatesTransformMatrix->Delete();

}

void vtkVectorManager::SetRenderWindowInteractor (vtkRenderWindowInteractor* rwin, vtkRenderer* ren)
{
    if( rwin != this->RenderWindowInteractor )
    {

        if( this->RenderWindowInteractor != NULL )
        {
            this->Initialize();
            this->RenderWindowInteractor->UnRegister (this);
        }

        this->RenderWindowInteractor = rwin;
        if( this->RenderWindowInteractor )
        {
            this->RenderWindowInteractor->Register(this);
        }
    }

    if (ren)
    {
        this->Renderer = ren;
    }
    else if (this->RenderWindowInteractor)
    {

        this->RenderWindowInteractor->GetRenderWindow()->GetRenderers()->InitTraversal();
        vtkRenderer* first_renderer = this->RenderWindowInteractor->GetRenderWindow()->GetRenderers()->GetNextItem();

        int numLayers = this->RenderWindowInteractor->GetRenderWindow()->GetNumberOfLayers();
        this->RenderWindowInteractor->GetRenderWindow()->SetNumberOfLayers ( numLayers + 1 );

        this->Renderer = vtkRenderer::New();
        this->Renderer->SetLayer ( numLayers );
        if (first_renderer)
            this->Renderer->SetActiveCamera ( first_renderer->GetActiveCamera() );

        this->RenderWindowInteractor->GetRenderWindow()->AddRenderer ( this->Renderer );
    }

}

void vtkVectorManager::Initialize()
{
    if( this->RenderWindowInteractor )
    {

        if ( this->Renderer )
        {
            this->Renderer->RemoveActor( this->VectorVisuManagerAxial->GetActor() );
            this->Renderer->RemoveActor( this->VectorVisuManagerSagittal->GetActor() );
            this->Renderer->RemoveActor( this->VectorVisuManagerCoronal->GetActor() );
        }
    }
}


void vtkVectorManager::SetCurrentPosition (const int& X, const int& Y, const int& Z)
{
    if( !this->Input )
    {
        return;
    }

    int* dims = this->Input->GetDimensions();

    // clamping
    int XX = X>(dims[0]-1)?(dims[0]-1):X;
    int YY = Y>(dims[1]-1)?(dims[1]-1):Y;
    int ZZ = Z>(dims[2]-1)?(dims[2]-1):Z;

    // synchronize with VOI
    this->VectorVisuManagerSagittal->SetVOI (XX, XX, 0, dims[1]-1, 0, dims[2]-1);
    this->VectorVisuManagerCoronal ->SetVOI (0, dims[0]-1, YY, YY, 0, dims[2]-1);
    this->VectorVisuManagerAxial   ->SetVOI (0, dims[0]-1, 0, dims[1]-1, ZZ, ZZ);

    this->CurrentPosition[0] = X;
    this->CurrentPosition[1] = Y;
    this->CurrentPosition[2] = Z;
}


void vtkVectorManager::SetCurrentPosition (int pos[3])
{
    this->SetCurrentPosition (pos[0], pos[1], pos[2]);
}


void vtkVectorManager::SetCurrentPosition (const double& X, const double& Y, const double& Z)
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
        vox_pos[i] = vtkMath::Round( (pos[i]-origin[i])/spacing[i] );

    this->SetCurrentPosition(vox_pos[0], vox_pos[1], vox_pos[2]);
}


void vtkVectorManager::SetCurrentPosition (double pos[3])
{
    this->SetCurrentPosition (pos[0], pos[1], pos[2]);
}


void vtkVectorManager::ResetPosition()
{
    if( !this->Input )
    {
        return;
    }

    int* dims = this->Input->GetDimensions();

    int X = (int)(dims[0]/2);
    int Y = (int)(dims[1]/2);
    int Z = (int)(dims[2]/2);

    this->SetCurrentPosition (X,Y,Z);
}


void vtkVectorManager::Update()
{
    if( !this->Input || !this->RenderWindowInteractor )
    {
        return;
    }

    this->Initialize();

    int* dims = this->Input->GetDimensions();

    int X = this->CurrentPosition[0];
    int Y = this->CurrentPosition[1];
    int Z = this->CurrentPosition[2];

    // clamping
    X = X>(dims[0]-1)?(dims[0]-1):X;
    Y = Y>(dims[1]-1)?(dims[1]-1):Y;
    Z = Z>(dims[2]-1)?(dims[2]-1):Z;

    // set the data
    this->VectorVisuManagerAxial->SetInput    (this->Input, this->DirectionMatrix);
    this->VectorVisuManagerSagittal->SetInput (this->Input, this->DirectionMatrix);
    this->VectorVisuManagerCoronal->SetInput  (this->Input, this->DirectionMatrix);

    // synchronize with VOI
    this->VectorVisuManagerSagittal->SetVOI (X, X, 0, dims[1]-1, 0, dims[2]-1);
    this->VectorVisuManagerCoronal ->SetVOI (0, dims[0]-1, Y, Y, 0, dims[2]-1);
    this->VectorVisuManagerAxial   ->SetVOI (0, dims[0]-1, 0, dims[1]-1, Z, Z);

    if ( this->Renderer )
    {
        this->Renderer->AddActor ( this->VectorVisuManagerAxial->GetActor() );
        this->Renderer->AddActor ( this->VectorVisuManagerSagittal->GetActor() );
        this->Renderer->AddActor ( this->VectorVisuManagerCoronal->GetActor() );
    }
}

void vtkVectorManager::SetDirectionMatrix(vtkMatrix4x4 *mat)
{
    if (!mat)
        return;

    vtkSetObjectBodyMacro(DirectionMatrix, vtkMatrix4x4, mat);

    vtkMatrix4x4::Invert(this->DirectionMatrix, this->PhysicalToVoxelCoordinatesTransformMatrix);
}


void vtkVectorManager::SetGlyphScale (const float& f)
{
  this->VectorVisuManagerAxial   ->SetGlyphScale (f);
  this->VectorVisuManagerSagittal->SetGlyphScale (f);
  this->VectorVisuManagerCoronal ->SetGlyphScale (f);
}


void vtkVectorManager::SetSampleRate (const int& n1,const int& n2, const int& n3)
{
  this->VectorVisuManagerAxial   ->SetSampleRate (n1, n2, n3);
  this->VectorVisuManagerSagittal->SetSampleRate (n1, n2, n3);
  this->VectorVisuManagerCoronal ->SetSampleRate (n1, n2, n3);
}

void vtkVectorManager::SetAxialSliceVisibility (int i)
{
    this->VectorVisuManagerAxial->GetActor ()->SetVisibility (i);
}

void vtkVectorManager::SetSagittalSliceVisibility (int i)
{
    this->VectorVisuManagerSagittal->GetActor ()->SetVisibility (i);
}

void vtkVectorManager::SetCoronalSliceVisibility (int i)
{
    this->VectorVisuManagerCoronal->GetActor ()->SetVisibility (i);
}

void vtkVectorManager::SetColorMode(int mode)
{
    this->VectorVisuManagerAxial   ->SetColorMode ((vtkVectorVisuManager::ColorMode)mode);
    this->VectorVisuManagerSagittal->SetColorMode ((vtkVectorVisuManager::ColorMode)mode);
    this->VectorVisuManagerCoronal ->SetColorMode ((vtkVectorVisuManager::ColorMode)mode);
}

void vtkVectorManager::SetProjection(bool enable)
{
    this->VectorVisuManagerAxial   ->SetProjection(enable);
    this->VectorVisuManagerSagittal->SetProjection(enable);
    this->VectorVisuManagerCoronal ->SetProjection(enable);
}

