/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkTensorManager.h"

#include <vtkObjectFactory.h>
#include <vtkMath.h>

vtkCxxRevisionMacro(vtkTensorManager, "$Revision: 1016 $");
vtkStandardNewMacro(vtkTensorManager);

vtkTensorManager::vtkTensorManager()
{
  this->RenderWindowInteractor = 0;

  this->Input = 0;

  this->Renderer = 0;
  
  this->TensorVisuManagerAxial    = vtkTensorVisuManager::New();
  this->TensorVisuManagerSagittal = vtkTensorVisuManager::New();
  this->TensorVisuManagerCoronal  = vtkTensorVisuManager::New();

  this->DirectionMatrix = 0;

  this->PhysicalToVoxelCoordinatesTransformMatrix = vtkMatrix4x4::New();
  this->PhysicalToVoxelCoordinatesTransformMatrix->Identity();

  for( int i=0; i<3; i++)
  {
    this->CurrentPosition[i] = 0;
  }
}

vtkTensorManager::~vtkTensorManager()
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

  this->TensorVisuManagerAxial->Delete();
  this->TensorVisuManagerSagittal->Delete();
  this->TensorVisuManagerCoronal->Delete();

  if (this->DirectionMatrix)
    this->DirectionMatrix->Delete();

  this->PhysicalToVoxelCoordinatesTransformMatrix->Delete();
}

void vtkTensorManager::SetRenderWindowInteractor (vtkRenderWindowInteractor* rwin, vtkRenderer* ren)
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
    
    // this->Renderer->Delete(); // no need since renderer is smart pointed
  }
  
}

void vtkTensorManager::Initialize()
{
  if( this->RenderWindowInteractor )
  {
    /*
      this->RenderWindowInteractor->GetRenderWindow()->GetRenderers()->InitTraversal();
      vtkRenderer* first_renderer = this->RenderWindowInteractor->GetRenderWindow()->GetRenderers()->GetNextItem();
      if (first_renderer)
      {
      first_renderer->RemoveActor( this->TensorVisuManagerAxial->GetActor() );
      first_renderer->RemoveActor( this->TensorVisuManagerSagittal->GetActor() );
      first_renderer->RemoveActor( this->TensorVisuManagerCoronal->GetActor() );
      }
    */
    if ( this->Renderer )
    {
      //this->RenderWindowInteractor->GetRenderWindow()->RemoveRenderer ( this->Renderer );
      //int numLayers = this->RenderWindowInteractor->GetRenderWindow()->GetNumberOfLayers();
      //this->RenderWindowInteractor->GetRenderWindow()->SetNumberOfLayers ( numLayers-1 );
      this->Renderer->RemoveActor( this->TensorVisuManagerAxial->GetActor() );
      this->Renderer->RemoveActor( this->TensorVisuManagerSagittal->GetActor() );
      this->Renderer->RemoveActor( this->TensorVisuManagerCoronal->GetActor() );
      //this->Renderer->Delete();
      //this->Renderer = 0;
    }
  } 
}

void vtkTensorManager::Update()
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
  
  // synchronize with VOI
  this->TensorVisuManagerSagittal->SetVOI (X, X, 0, dims[1]-1, 0, dims[2]-1);
  this->TensorVisuManagerCoronal ->SetVOI (0, dims[0]-1, Y, Y, 0, dims[2]-1);
  this->TensorVisuManagerAxial   ->SetVOI (0, dims[0]-1, 0, dims[1]-1, Z, Z);
  

  // finally set the data
  this->TensorVisuManagerAxial->SetInput    (this->Input, this->DirectionMatrix);
  this->TensorVisuManagerSagittal->SetInput (this->Input, this->DirectionMatrix);
  this->TensorVisuManagerCoronal->SetInput  (this->Input, this->DirectionMatrix);
  
  if ( this->Renderer )
  {
    this->Renderer->AddActor ( this->TensorVisuManagerAxial->GetActor() );
    this->Renderer->AddActor ( this->TensorVisuManagerSagittal->GetActor() );
    this->Renderer->AddActor ( this->TensorVisuManagerCoronal->GetActor() );
  }   
}

void vtkTensorManager::SetDirectionMatrix(vtkMatrix4x4 *mat)
{
    if (!mat)
        return;

    vtkSetObjectBodyMacro(DirectionMatrix, vtkMatrix4x4, mat);

    vtkMatrix4x4::Invert(this->DirectionMatrix, this->PhysicalToVoxelCoordinatesTransformMatrix);
}

void vtkTensorManager::SetCurrentPosition (const int& X, const int& Y, const int& Z)
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
  this->TensorVisuManagerSagittal->SetVOI (XX, XX, 0, dims[1]-1, 0, dims[2]-1);
  this->TensorVisuManagerCoronal ->SetVOI (0, dims[0]-1, YY, YY, 0, dims[2]-1);
  this->TensorVisuManagerAxial   ->SetVOI (0, dims[0]-1, 0, dims[1]-1, ZZ, ZZ);

  this->CurrentPosition[0] = X;
  this->CurrentPosition[1] = Y;
  this->CurrentPosition[2] = Z;
  
}



void vtkTensorManager::SetCurrentPosition (int pos[3])
{
  this->SetCurrentPosition (pos[0], pos[1], pos[2]);
}


void vtkTensorManager::SetCurrentPosition (const double& X, const double& Y, const double& Z)
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


void vtkTensorManager::SetCurrentPosition (double pos[3])
{
  this->SetCurrentPosition (pos[0], pos[1], pos[2]);
}


void vtkTensorManager::ResetPosition()
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


void vtkTensorManager::SetGlyphShape (int i)
{
  this->TensorVisuManagerAxial   ->SetGlyphShape (i);
  this->TensorVisuManagerSagittal->SetGlyphShape (i);
  this->TensorVisuManagerCoronal ->SetGlyphShape (i);
}


void vtkTensorManager::SetGlyphShapeToLine ()
{
  this->TensorVisuManagerAxial   ->SetGlyphShapeToLine ();
  this->TensorVisuManagerSagittal->SetGlyphShapeToLine ();
  this->TensorVisuManagerCoronal ->SetGlyphShapeToLine ();
}


void vtkTensorManager::SetGlyphShapeToDisk ()
{
  this->TensorVisuManagerAxial   ->SetGlyphShapeToDisk ();
  this->TensorVisuManagerSagittal->SetGlyphShapeToDisk ();
  this->TensorVisuManagerCoronal ->SetGlyphShapeToDisk ();
}


void vtkTensorManager::SetGlyphShapeToArrow ()
{
  this->TensorVisuManagerAxial   ->SetGlyphShapeToArrow ();
  this->TensorVisuManagerSagittal->SetGlyphShapeToArrow ();
  this->TensorVisuManagerCoronal ->SetGlyphShapeToArrow ();
}


void vtkTensorManager::SetGlyphShapeToCube ()
{
  this->TensorVisuManagerAxial   ->SetGlyphShapeToCube ();
  this->TensorVisuManagerSagittal->SetGlyphShapeToCube ();
  this->TensorVisuManagerCoronal ->SetGlyphShapeToCube ();
}


void vtkTensorManager::SetGlyphShapeToCylinder ()
{
  this->TensorVisuManagerAxial   ->SetGlyphShapeToCylinder ();
  this->TensorVisuManagerSagittal->SetGlyphShapeToCylinder ();
  this->TensorVisuManagerCoronal ->SetGlyphShapeToCylinder ();
}


void vtkTensorManager::SetGlyphShapeToSphere ()
{
  this->TensorVisuManagerAxial   ->SetGlyphShapeToSphere ();
  this->TensorVisuManagerSagittal->SetGlyphShapeToSphere ();
  this->TensorVisuManagerCoronal ->SetGlyphShapeToSphere ();
}


void vtkTensorManager::SetGlyphShapeToSuperquadric ()
{
  this->TensorVisuManagerAxial   ->SetGlyphShapeToSuperquadric ();
  this->TensorVisuManagerSagittal->SetGlyphShapeToSuperquadric ();
  this->TensorVisuManagerCoronal ->SetGlyphShapeToSuperquadric ();
}


void vtkTensorManager::SetGlyphScale (const float& f)
{
  this->TensorVisuManagerAxial   ->SetGlyphScale (f);
  this->TensorVisuManagerSagittal->SetGlyphScale (f);
  this->TensorVisuManagerCoronal ->SetGlyphScale (f);
}


void vtkTensorManager::SetMaxGlyphSize (const float& f)
{
  this->TensorVisuManagerAxial   ->SetMaxGlyphSize (f);
  this->TensorVisuManagerSagittal->SetMaxGlyphSize (f);
  this->TensorVisuManagerCoronal ->SetMaxGlyphSize (f);
}


void vtkTensorManager::SetSampleRate (const int& n1,const int& n2, const int& n3)
{
  this->TensorVisuManagerAxial   ->SetSampleRate (n1, n2, n3);
  this->TensorVisuManagerSagittal->SetSampleRate (n1, n2, n3);
  this->TensorVisuManagerCoronal ->SetSampleRate (n1, n2, n3);
}


void vtkTensorManager::SetGlyphResolution (int res)
{
  this->TensorVisuManagerAxial   ->SetGlyphResolution (res);
  this->TensorVisuManagerSagittal->SetGlyphResolution (res);
  this->TensorVisuManagerCoronal ->SetGlyphResolution (res);
}


void vtkTensorManager::SetScalarRange (const float& min, const float& max)
{
  this->TensorVisuManagerAxial   ->SetScalarRange (min, max);
  this->TensorVisuManagerSagittal->SetScalarRange (min, max);
  this->TensorVisuManagerCoronal ->SetScalarRange (min, max);
}


void vtkTensorManager::SetColorModeToEigenvector (const int& v)
{
  this->TensorVisuManagerAxial   ->SetColorModeToEigenvector (v);
  this->TensorVisuManagerSagittal->SetColorModeToEigenvector (v);
  this->TensorVisuManagerCoronal ->SetColorModeToEigenvector (v);
}


void vtkTensorManager::SetColorModeToEigenvalue (const int& v)
{
  this->TensorVisuManagerAxial   ->SetColorModeToEigenvalue (v);
  this->TensorVisuManagerSagittal->SetColorModeToEigenvalue (v);
  this->TensorVisuManagerCoronal ->SetColorModeToEigenvalue (v);
}


void vtkTensorManager::SetColorModeToVolume ()
{
  this->TensorVisuManagerAxial   ->SetColorModeToVolume ();
  this->TensorVisuManagerSagittal->SetColorModeToVolume ();
  this->TensorVisuManagerCoronal ->SetColorModeToVolume ();
}


void vtkTensorManager::SetColorModeToTrace ()
{
  this->TensorVisuManagerAxial   ->SetColorModeToTrace ();
  this->TensorVisuManagerSagittal->SetColorModeToTrace ();
  this->TensorVisuManagerCoronal ->SetColorModeToTrace ();
}


void vtkTensorManager::SetColorModeToDistanceToIdentity ()
{
  this->TensorVisuManagerAxial   ->SetColorModeToDistanceToIdentity ();
  this->TensorVisuManagerSagittal->SetColorModeToDistanceToIdentity ();
  this->TensorVisuManagerCoronal ->SetColorModeToDistanceToIdentity ();
}


void vtkTensorManager::SetColorMode (const int& i)
{
  this->TensorVisuManagerAxial   ->SetColorMode (i);
  this->TensorVisuManagerSagittal->SetColorMode (i);
  this->TensorVisuManagerCoronal ->SetColorMode (i);
}


void vtkTensorManager::FlipX (bool a)
{
  this->TensorVisuManagerAxial   ->FlipX (a);
  this->TensorVisuManagerSagittal->FlipX (a);
  this->TensorVisuManagerCoronal ->FlipX (a);
}


void vtkTensorManager::FlipY (bool a)
{
  this->TensorVisuManagerAxial   ->FlipY (a);
  this->TensorVisuManagerSagittal->FlipY (a);
  this->TensorVisuManagerCoronal ->FlipY (a);
}


void vtkTensorManager::FlipZ (bool a)
{
  this->TensorVisuManagerAxial   ->FlipZ (a);
  this->TensorVisuManagerSagittal->FlipZ (a);
  this->TensorVisuManagerCoronal ->FlipZ (a);
}


void vtkTensorManager::SetLookupTable (vtkLookupTable* lut)
{
  this->TensorVisuManagerAxial   ->SetLookupTable (lut);
  this->TensorVisuManagerSagittal->SetLookupTable (lut);
  this->TensorVisuManagerCoronal ->SetLookupTable (lut);
}


void vtkTensorManager::SetScalars (vtkDataArray* scalars)
{
  this->TensorVisuManagerAxial   ->SetScalars (scalars);
  this->TensorVisuManagerSagittal->SetScalars (scalars);
  this->TensorVisuManagerCoronal ->SetScalars (scalars);
}



void vtkTensorManager::SetAxialSliceVisibility (int i)
{
  this->TensorVisuManagerAxial->GetActor ()->SetVisibility (i);
}


void vtkTensorManager::SetSagittalSliceVisibility (int i)
{
  this->TensorVisuManagerSagittal->GetActor ()->SetVisibility (i);
}


void vtkTensorManager::SetCoronalSliceVisibility (int i)
{
  this->TensorVisuManagerCoronal->GetActor ()->SetVisibility (i);
}

