#include <vtkSphericalHarmonicManager.h>

#include <vtkObjectFactory.h>

vtkCxxRevisionMacro(vtkSphericalHarmonicManager, "$Revision: 477 $");
vtkStandardNewMacro(vtkSphericalHarmonicManager);

vtkSphericalHarmonicManager::vtkSphericalHarmonicManager()
{
  this->RenderWindowInteractor = 0;

  this->Input = 0;

  this->Renderer = 0;

  this->SHVisuManagerAxial    = vtkSphericalHarmonicVisuManager::New();
  this->SHVisuManagerSagittal = vtkSphericalHarmonicVisuManager::New();
  this->SHVisuManagerCoronal  = vtkSphericalHarmonicVisuManager::New();
  
  for( int i=0; i<3; i++)
  {
    this->CurrentPosition[i] = 0;
  }
  
}



vtkSphericalHarmonicManager::~vtkSphericalHarmonicManager()
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

  this->SHVisuManagerAxial->Delete();
  this->SHVisuManagerSagittal->Delete();
  this->SHVisuManagerCoronal->Delete();
  
}

void vtkSphericalHarmonicManager::SetRenderWindowInteractor (vtkRenderWindowInteractor* rwin, vtkRenderer* ren)
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

    this->Renderer->Delete();
  }

}


void vtkSphericalHarmonicManager::Initialize()
{

  if( this->RenderWindowInteractor )
  {
    /*
    this->RenderWindowInteractor->GetRenderWindow()->GetRenderers()->InitTraversal();
    vtkRenderer* first_renderer = this->RenderWindowInteractor->GetRenderWindow()->GetRenderers()->GetNextItem();
    if (first_renderer)
    {
      first_renderer->RemoveActor( this->SHVisuManagerAxial->GetActor() );
      first_renderer->RemoveActor( this->SHVisuManagerSagittal->GetActor() );
      first_renderer->RemoveActor( this->SHVisuManagerCoronal->GetActor() );
    }
    */

    if ( this->Renderer )
    {
//      this->RenderWindowInteractor->GetRenderWindow()->RemoveRenderer ( this->Renderer );
//      int numLayers = this->RenderWindowInteractor->GetRenderWindow()->GetNumberOfLayers();
//      this->RenderWindowInteractor->GetRenderWindow()->SetNumberOfLayers ( numLayers-1 );
      this->Renderer->RemoveActor( this->SHVisuManagerAxial->GetActor() );
      this->Renderer->RemoveActor( this->SHVisuManagerSagittal->GetActor() );
      this->Renderer->RemoveActor( this->SHVisuManagerCoronal->GetActor() );
//      this->Renderer->Delete();
//      this->Renderer = 0;
    }
    
  }
  
}





void vtkSphericalHarmonicManager::Update()
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
  this->SHVisuManagerSagittal->SetVOI (X, X, 0, dims[1]-1, 0, dims[2]-1);
  this->SHVisuManagerCoronal ->SetVOI (0, dims[0]-1, Y, Y, 0, dims[2]-1);
  this->SHVisuManagerAxial   ->SetVOI (0, dims[0]-1, 0, dims[1]-1, Z, Z);
  

  // finally set the data
  this->SHVisuManagerAxial->SetInput( this->Input );
  this->SHVisuManagerSagittal->SetInput( this->Input );
  this->SHVisuManagerCoronal->SetInput( this->Input );

  if ( /*!*/this->Renderer )
  {  
//    this->RenderWindowInteractor->GetRenderWindow()->GetRenderers()->InitTraversal();
//    vtkRenderer* first_renderer = this->RenderWindowInteractor->GetRenderWindow()->GetRenderers()->GetNextItem();

//    /*
//    if (first_renderer)
//    {
//      first_renderer->AddActor ( this->SHVisuManagerAxial->GetActor() );
//      first_renderer->AddActor ( this->SHVisuManagerSagittal->GetActor() );
//      first_renderer->AddActor ( this->SHVisuManagerCoronal->GetActor() );
//    }
//    */

//    int numLayers = this->RenderWindowInteractor->GetRenderWindow()->GetNumberOfLayers();
//    this->RenderWindowInteractor->GetRenderWindow()->SetNumberOfLayers ( numLayers + 1 );
//    this->Renderer = vtkRenderer::New();
//    this->Renderer->SetLayer ( numLayers );
//    this->Renderer->SetActiveCamera ( first_renderer->GetActiveCamera() );
    this->Renderer->AddActor ( this->SHVisuManagerAxial->GetActor() );
    this->Renderer->AddActor ( this->SHVisuManagerSagittal->GetActor() );
    this->Renderer->AddActor ( this->SHVisuManagerCoronal->GetActor() );

//    this->RenderWindowInteractor->GetRenderWindow()->AddRenderer ( this->Renderer );
  }
  
    
}



void vtkSphericalHarmonicManager::SetCurrentPosition (const int& X, const int& Y, const int& Z)
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
  this->SHVisuManagerSagittal->SetVOI (XX, XX, 0, dims[1]-1, 0, dims[2]-1);
  this->SHVisuManagerCoronal ->SetVOI (0, dims[0]-1, YY, YY, 0, dims[2]-1);
  this->SHVisuManagerAxial   ->SetVOI (0, dims[0]-1, 0, dims[1]-1, ZZ, ZZ);

  this->CurrentPosition[0] = X;
  this->CurrentPosition[1] = Y;
  this->CurrentPosition[2] = Z;
  
}



void vtkSphericalHarmonicManager::SetCurrentPosition (int pos[3])
{
  this->SetCurrentPosition (pos[0], pos[1], pos[2]);
}



void vtkSphericalHarmonicManager::ResetPosition()
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



void vtkSphericalHarmonicManager::SetSampleRate (const int& n1,const int& n2, const int& n3)
{
  this->SHVisuManagerAxial   ->SetSampleRate (n1, n2, n3);
  this->SHVisuManagerSagittal->SetSampleRate (n1, n2, n3);
  this->SHVisuManagerCoronal ->SetSampleRate (n1, n2, n3);
}


void vtkSphericalHarmonicManager::SetGlyphResolution (int res)
{
  this->SHVisuManagerAxial   ->SetGlyphResolution (res);
  this->SHVisuManagerSagittal->SetGlyphResolution (res);
  this->SHVisuManagerCoronal ->SetGlyphResolution (res);
}

void vtkSphericalHarmonicManager::SetTesselationType (const int& type)
{
  this->SHVisuManagerAxial   ->SetTesselationType(type);
  this->SHVisuManagerSagittal->SetTesselationType(type);
  this->SHVisuManagerCoronal ->SetTesselationType(type);
}

void vtkSphericalHarmonicManager::SetTesselationBasis (const int& type)
{
  this->SHVisuManagerAxial   ->SetTesselationBasis(type);
  this->SHVisuManagerSagittal->SetTesselationBasis(type);
  this->SHVisuManagerCoronal ->SetTesselationBasis(type);
}



    /*
void vtkSHManager::SetScalarRange (const float& min, const float& max)
{
  this->SHVisuManagerAxial   ->SetScalarRange (min, max);
  this->SHVisuManagerSagittal->SetScalarRange (min, max);
  this->SHVisuManagerCoronal ->SetScalarRange (min, max);
}
    */

void vtkSphericalHarmonicManager::FlipX (bool a)
{
  this->SHVisuManagerAxial   ->FlipX (a);
  this->SHVisuManagerSagittal->FlipX (a);
  this->SHVisuManagerCoronal ->FlipX (a);
}


void vtkSphericalHarmonicManager::FlipY (bool a)
{
  this->SHVisuManagerAxial   ->FlipY (a);
  this->SHVisuManagerSagittal->FlipY (a);
  this->SHVisuManagerCoronal ->FlipY (a);
}


void vtkSphericalHarmonicManager::FlipZ (bool a)
{
  this->SHVisuManagerAxial   ->FlipZ (a);
  this->SHVisuManagerSagittal->FlipZ (a);
  this->SHVisuManagerCoronal ->FlipZ (a);
}

void vtkSphericalHarmonicManager::MaxThesisFunc (bool a)
{
  this->SHVisuManagerAxial   ->MaxThesisFunc (a);
  this->SHVisuManagerSagittal->MaxThesisFunc (a);
  this->SHVisuManagerCoronal ->MaxThesisFunc (a);
}

/*
void vtkSphericalHarmonicManager::SetLookupTable (vtkLookupTable* lut)
{
  this->SHVisuManagerAxial   ->SetLookupTable (lut);
  this->SHVisuManagerSagittal->SetLookupTable (lut);
  this->SHVisuManagerCoronal ->SetLookupTable (lut);
}

*/

void vtkSphericalHarmonicManager::GetSphericalHarmonicDimensions (int * dims)
{
    this->Input->GetDimensions(dims);
}

void vtkSphericalHarmonicManager::SetAxialSliceVisibility (int i)
{
  this->SHVisuManagerAxial->GetActor ()->SetVisibility (i);
}


void vtkSphericalHarmonicManager::SetSagittalSliceVisibility (int i)
{
  this->SHVisuManagerSagittal->GetActor ()->SetVisibility (i);
}


void vtkSphericalHarmonicManager::SetCoronalSliceVisibility (int i)
{
  this->SHVisuManagerCoronal->GetActor ()->SetVisibility (i);
}


void vtkSphericalHarmonicManager::SetGlyphScale (const float& scale)
{
  this->SHVisuManagerAxial   ->SetGlyphScale (scale);
  this->SHVisuManagerSagittal->SetGlyphScale (scale);
  this->SHVisuManagerCoronal ->SetGlyphScale (scale);
}
