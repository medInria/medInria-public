/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkIsosurfaceManager.h"
#include <vtkObjectFactory.h>

#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkLookupTableManager.h"


vtkStandardNewMacro(vtkIsosurfaceManager);

vtkIsosurfaceManager::vtkIsosurfaceManager()
{
  this->Input = 0;
  this->RenderWindowInteractor = 0;
  this->LUT = vtkLookupTableManager::GetROILookupTable();
  this->DefaultColor = new double[4];
  this->DefaultColor[0] = 1;
  this->DefaultColor[1] = 0;
  this->DefaultColor[2] = 0;
  this->DefaultColor[3] = 1;
  
  this->Visibility      = 1;
  this->Opacity         = 0.5;
  this->Decimation      = 0;

  this->DirectionMatrix = 0;
}

vtkIsosurfaceManager::~vtkIsosurfaceManager()
{
  this->ResetData();
  
  delete [] this->DefaultColor;
  
  if (this->LUT)
  {
    this->LUT->Delete();
  }

  if( this->Input )
  {
    this->Input->Delete();
  }

  if (this->DirectionMatrix)
  {
    this->DirectionMatrix->Delete();
  }

  if( this->RenderWindowInteractor )
  {
    this->RenderWindowInteractor->Delete();
  }
  
}

void vtkIsosurfaceManager::Enable()
{
    if( this->RenderWindowInteractor && this->RenderWindowInteractor->GetRenderWindow())
    {
      this->RenderWindowInteractor->GetRenderWindow()->GetRenderers()->InitTraversal();
      vtkRenderer* first_renderer =
              this->RenderWindowInteractor->GetRenderWindow()->GetRenderers()->GetNextItem();
      /*this->RenderWindowInteractor->FindPokedRenderer(this->RenderWindowInteractor->GetLastEventPosition()[0],
                                                                                    this->RenderWindowInteractor->GetLastEventPosition()[1]);
                                                                                    */
      if (first_renderer)
      {
        for( unsigned int i=0; i<this->Isosurfaces.size(); i++)
        {
          first_renderer->AddViewProp ( this->Isosurfaces[i]->GetActor() );
        }
      }
    }
}

void vtkIsosurfaceManager::Disable()
{
    this->RemoveAllActors();
}

void vtkIsosurfaceManager::SetRenderWindowInteractor (vtkRenderWindowInteractor* rwin)
{ 
  if( rwin != this->RenderWindowInteractor )
  {   
    if( this->RenderWindowInteractor != NULL )
    {
      this->RemoveAllActors();
      this->RenderWindowInteractor->UnRegister (this);
    }
    
    this->RenderWindowInteractor = rwin;
    if( this->RenderWindowInteractor )
    {
      this->RenderWindowInteractor->Register(this);
    }
  }
}

void vtkIsosurfaceManager::RemoveAllActors()
{
  if (this->RenderWindowInteractor && this->RenderWindowInteractor->GetRenderWindow())
  {
    this->RenderWindowInteractor->GetRenderWindow()->GetRenderers()->InitTraversal();

    vtkRenderer* first_renderer =
            this->RenderWindowInteractor->GetRenderWindow()->GetRenderers()->GetNextItem();

    if (first_renderer)
    {
      for( unsigned int i=0; i<this->Isosurfaces.size(); i++)
      {
          first_renderer->RemoveActor ( this->Isosurfaces[i]->GetActor() );
      }
    }
  }
}

void vtkIsosurfaceManager::GenerateData()
{

  if( !this->RenderWindowInteractor )
  {
    vtkErrorMacro (<<"Error: The interactor must be set prior to calling GenerateData()!");
    return;
  }

  if( !this->Input )
  {
    vtkErrorMacro (<<"Error: Input is not set!");
    return;
  }
  
  
  this->ResetData();

  
  // Get the isovalues of the Input image
  unsigned char* Scalars= (unsigned char*)this->Input->GetScalarPointer();
  for (int i=0;i<this->Input->GetNumberOfPoints();i++)
  {
    int val = (int)(*Scalars);    
    if (val>0)
    {
      bool IsInserted = false;
      for (unsigned int j=0; j<this->IsoValues.size(); j++)
      {
        if ( this->IsoValues[j]==val )
        {
          IsInserted=true;
        }
      }
      
      if (!IsInserted)
      {
        this->IsoValues.push_back (val);
      }
    }
    
    ++Scalars;
  }


  this->RenderWindowInteractor->GetRenderWindow()->GetRenderers()->InitTraversal();
  // vtkRenderer* renderer = this->RenderWindowInteractor->GetRenderWindow()->GetRenderers()->GetNextItem();
  vtkRenderer* renderer = this->RenderWindowInteractor->FindPokedRenderer(
				this->RenderWindowInteractor->GetLastEventPosition()[0],
				this->RenderWindowInteractor->GetLastEventPosition()[1]);
  

  // Add the new actors to the renderwindow
  for (unsigned int i=0; i<this->IsoValues.size(); i++)
  {
    double* color = new double[4];
    if ( this->IsoValues[i]<this->LUT->GetNumberOfTableValues() )
    {
      this->LUT->GetTableValue ( this->IsoValues[i], color );
    }
    else
    {
      color[0] = this->DefaultColor[0];
      color[1] = this->DefaultColor[1];
      color[2] = this->DefaultColor[2];
      color[3] = this->DefaultColor[3];
    }
    
    vtkImageToIsosurface* ImageToIsosurfaceFilter = vtkImageToIsosurface::New();    
    ImageToIsosurfaceFilter->SetParameters ( this->IsoValues[i], color );
    ImageToIsosurfaceFilter->SetInput ( this->Input );
    ImageToIsosurfaceFilter->Update();

    if( !this->Decimation )
    {
      ImageToIsosurfaceFilter->SetDecimationOff();
    }
    

    if (this->DirectionMatrix)
    {
      if (this->DirectionMatrix->Determinant()<0.0) // flip normals
      {
	ImageToIsosurfaceFilter->ReverseNormals();
      }
      ImageToIsosurfaceFilter->GetActor()->SetUserMatrix (this->DirectionMatrix);
    }
    
    if( renderer )
    {
      renderer->AddActor ( ImageToIsosurfaceFilter->GetActor() );
    }
    this->Isosurfaces.push_back ( ImageToIsosurfaceFilter );
    
    delete [] color;    
  }
  
  // Update the actors with the settings
  this->UpdateActors();
  

}


void vtkIsosurfaceManager::ResetData()
{

  // Remove the old actors from the renderwindow if any
  this->RemoveAllActors();

  for (unsigned int i=0; i<this->Isosurfaces.size(); i++)
  {
    this->Isosurfaces[i]->Delete();
  }
  this->Isosurfaces.clear();
  this->IsoValues.clear();
}



void vtkIsosurfaceManager::UpdateActors ()
{
  for (unsigned int i=0; i<this->Isosurfaces.size(); i++)
  {
    this->Isosurfaces[i]->GetActor()->SetVisibility ( this->Visibility );
    //this->Actors[i]->GetProperty()->SetOpacity (this->Opacity);
  }
}



void vtkIsosurfaceManager::SetVisibility (int IsVisible)
{
  this->Visibility = IsVisible;
  for (unsigned int i=0; i<this->Isosurfaces.size(); i++)
  {
    this->Isosurfaces[i]->GetActor()->SetVisibility ( this->Visibility );
  }
}



void vtkIsosurfaceManager::SetVisibility (int i, int IsVisible)
{
  int ind=-1;
  for( unsigned int j=0; j<this->IsoValues.size(); j++)
  {
    if( this->IsoValues[j]==i )
    {
      ind=(int)j;
      break;
    }
  }
  
  if( ind==-1 )
  {
    return;
  }
  else
  {
    this->Isosurfaces[ind]->GetActor()->SetVisibility (IsVisible);
  }
}



int vtkIsosurfaceManager::GetVisibility (int i) const
{
  int ind=-1;
  for( unsigned int j=0; j<this->IsoValues.size(); j++)
  {
    if( this->IsoValues[j]==i )
    {
      ind=(int)j;
      break;
    }
  }
  
  if( ind==-1 )
  {
    return 1;
  }
  else
  {
    return this->Isosurfaces[ind]->GetActor()->GetVisibility();
  }
}



void vtkIsosurfaceManager::SetOpacity (double val)
{
  this->Opacity = val;
  for (unsigned int i=0; i<this->Isosurfaces.size(); i++)
  {
    this->Isosurfaces[i]->GetActor()->GetProperty()->SetOpacity (this->Opacity);
  }
}



void vtkIsosurfaceManager::SetOpacity (int i, double val)
{
  int ind=-1;
  for( unsigned int j=0; j<this->IsoValues.size(); j++)
  {
    if( this->IsoValues[j]==i )
    {
      ind=(int)j;
      break;
    }
  }
  
  
  if( ind!=-1 )
  {
    this->Isosurfaces[ind]->GetActor()->GetProperty()->SetOpacity (val);
  }
}



double vtkIsosurfaceManager::GetOpacity(int i) const
{
  int ind=-1;
  for( unsigned int j=0; j<this->IsoValues.size(); j++)
  {
    if( this->IsoValues[j]==i )
    {
      ind=(int)j;
      break;
    }
  }

  
  if(ind!=-1)
  {
    return this->Isosurfaces[ind]->GetActor()->GetProperty()->GetOpacity();
  }
  else
  {  
    return -1.0;
  }
}


void vtkIsosurfaceManager::SetDecimationOn ()
{
  this->Decimation = 1;
  for( unsigned int i=0; i<this->Isosurfaces.size(); i++)
  {
    this->Isosurfaces[i]->SetDecimationOn();
  }
  
}


void vtkIsosurfaceManager::SetDecimationOff ()
{
  this->Decimation = 0;
  for( unsigned int i=0; i<this->Isosurfaces.size(); i++)
  {
    this->Isosurfaces[i]->SetDecimationOff();
  }
}
