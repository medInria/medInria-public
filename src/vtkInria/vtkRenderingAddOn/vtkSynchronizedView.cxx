/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkSynchronizedView.h>

#include "vtkObjectFactory.h"
#include "vtkInteractorObserver.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRendererCollection.h"
#include "vtkProp.h"
#include "vtkTextActor.h"
#include "vtkCoordinate.h"
#include "vtkProperty.h"
#include "vtkProperty2D.h"
#include "vtkTextProperty.h"
#include <vtkCornerAnnotation.h>
#include <vtkOrientationAnnotation.h>
#include <vtkCamera.h>

#include <algorithm>


vtkCxxRevisionMacro(vtkSynchronizedView, "$Revision: 1323 $");
vtkStandardNewMacro(vtkSynchronizedView);



vtkSynchronizedView::vtkSynchronizedView()
{
  
  this->IsProcessed     = 0;
  this->Interaction     = 1;
  this->ShowAnnotations = 1;
  this->ShowDirections  = 1;
  this->AboutDataVisibility = 1;
  this->LinkRender      = 1;
  this->LinkCameraFocalAndPosition = 0;
  
  this->Renderer               = 0;
  this->RenderWindow           = 0;
  this->RenderWindowInteractor = 0;
  //this->InteractorStyle        = 0;

  // Initilize Annotations
  this->CornerAnnotation = vtkCornerAnnotation::New();
  this->CornerAnnotation->SetNonlinearFontScaleFactor (0.3);

  this->TextProperty = vtkTextProperty::New();
  this->CornerAnnotation->SetTextProperty ( this->TextProperty );
  
  this->OrientationAnnotation = vtkOrientationAnnotation::New();
  this->OrientationAnnotation->SetNonlinearFontScaleFactor (0.25);

  this->AboutData = "";
  
  this->Parent = 0;

  this->InternalMTime = 0;

}


vtkSynchronizedView::~vtkSynchronizedView()
{	
  this->TextProperty->Delete();
  this->CornerAnnotation->Delete();
  this->OrientationAnnotation->Delete();

  if( this->RenderWindow )
  {
    this->RenderWindow->Delete();
    this->RenderWindow = 0;
  }


  if( this->Renderer )
  {
    this->Renderer->Delete();
    this->Renderer = 0;
  }

  if( this->RenderWindowInteractor )
  {
    this->RenderWindowInteractor->Delete();
    this->RenderWindowInteractor = 0;
  }

  /*
    if( this->InteractorStyle )
    {
    this->InteractorStyle->Delete();
    this->InteractorStyle = 0;
    }*/
}


void vtkSynchronizedView::SetRenderWindow(vtkRenderWindow *arg)
{
  if (this->RenderWindow == arg)
  {
    return;
  }
  
  this->Uninitialize();
  
  if (this->RenderWindow)
  {
    this->RenderWindow->UnRegister(this);
  }
  
  this->RenderWindow = arg;
  
  if (this->RenderWindow)
  {
    this->RenderWindow->Register(this);
  }

  if (this->RenderWindow && this->RenderWindow->GetInteractor())
  {
    this->SetRenderWindowInteractor (this->RenderWindow->GetInteractor());
  }
  this->Initialize();

  this->Modified();
}


void vtkSynchronizedView::SetRenderer(vtkRenderer *arg)
{
  if (this->Renderer == arg)
  {
    return;
  }
  
  this->Uninitialize();
  
  if (this->Renderer)
  {
    this->Renderer->UnRegister(this);
  }
  
  this->Renderer = arg;
  
  if (this->Renderer)
  {
    this->Renderer->Register(this);
  }
  
  this->Initialize();
  
  this->Modified();
}


#ifndef VTK_LEGACY_REMOVE
void vtkSynchronizedView::SetInteractor(vtkRenderWindowInteractor *arg)
{
  VTK_LEGACY_REPLACED_BODY(vtkSynchronizedView::SetInteractor, "vtkINRIA3D 0.1",
			   vtkSynchronizedView::SetRenderWindowInteractor);
  this->SetRenderWindowInteractor ( arg );
}
#endif



void vtkSynchronizedView::SetRenderWindowInteractor(vtkRenderWindowInteractor *arg)
{
  if (this->RenderWindowInteractor == arg)
  {
    return;
  }

  this->Uninitialize();
  
  if (this->RenderWindowInteractor)
  {
    this->RenderWindowInteractor->UnRegister(this);
  }
  
  this->RenderWindowInteractor = arg;
  
  if (this->RenderWindowInteractor)
  {
    this->RenderWindowInteractor->Register(this);
  }
  
  this->Initialize();

  this->Modified();
}



/*
void vtkSynchronizedView::SetInteractorStyle(vtkInteractorStyle* style)
{
 
  if (this->InteractorStyle == style)
  {
    return;
  }

  this->Uninitialize();

  if (this->InteractorStyle)
  {
    this->InteractorStyle->UnRegister(this);
  }
  
  this->InteractorStyle = style;
  
  
  if (this->InteractorStyle)
  {
    this->InteractorStyle->Register(this);
  }

  this->Initialize();

	this->Uninitialize();
	this->InteractorStyle = style;
	this->Initialize();

  //this->SetInteraction();
  //this->Modified();
}
*/



void vtkSynchronizedView::Initialize()
{
  if (this->Renderer)
  {
    //this->Renderer->SetBackground(0.9,0.9,0.9);
    this->Renderer->AddViewProp ( this->CornerAnnotation );
    this->Renderer->AddViewProp ( this->OrientationAnnotation );
  }
  
  if (this->RenderWindow && this->Renderer)
  {
    this->RenderWindow->AddRenderer(this->Renderer);  
  }
  
  if (this->RenderWindowInteractor && this->RenderWindow)
  {
    /*
      if ( this->InteractorStyle )
      {
      this->RenderWindowInteractor->SetInteractorStyle( this->InteractorStyle );
      this->SetInteraction();
      }
    */
    this->RenderWindowInteractor->SetRenderWindow(this->RenderWindow);
  }
}



void vtkSynchronizedView::Uninitialize()
{
  if (this->Renderer)
  {
    this->Renderer->RemoveViewProp( this->CornerAnnotation );
    this->Renderer->RemoveViewProp( this->OrientationAnnotation );
  }
  
  if (this->RenderWindow && this->Renderer)
  {
    this->RenderWindow->RemoveRenderer(this->Renderer);
  }
  /*
    if (this->RenderWindowInteractor)
    {
    //this->RenderWindowInteractor->SetInteractorStyle(NULL);
    //this->RenderWindowInteractor->SetRenderWindow(NULL); // never!
    }*/
}


#ifndef VTK_LEGACY_REMOVE
std::vector < vtkSynchronizedView* > vtkSynchronizedView::GetViewToObserve() const
{ 
  return this->GetChildren(); 
}
#endif

void vtkSynchronizedView::AddActor (vtkProp* actor)
{
  if (this->GetRenderer())
  {
    this->GetRenderer()->AddActor(actor);
  }
}


void vtkSynchronizedView::RemoveActor (vtkProp* actor)
{
  if (this->GetRenderer())
  {
    this->GetRenderer()->RemoveActor(actor);
  }
}


void vtkSynchronizedView::SetInteraction(int interaction)
{
  this->Interaction = interaction;
  
  if ( this->GetRenderWindowInteractor() && this->GetRenderWindowInteractor()->GetInteractorStyle() )
  {
    if ( !this->Interaction )
    {
      this->GetRenderWindowInteractor()->GetInteractorStyle()->SetEnabled(0);
    }
    else
    {
      this->GetRenderWindowInteractor()->GetInteractorStyle()->SetEnabled(1);
    }
  }

  this->Modified();
}


void vtkSynchronizedView::SetUpLeftAnnotation(const char *p_annotation)
{
  //this->UpLeftAnnotation = p_annotation;  
  //this->UpdateAnnotations();
  this->CornerAnnotation->SetText (2, p_annotation);
  this->Modified();
}


const char *vtkSynchronizedView::GetUpLeftAnnotation() const
{
  //return this->UpLeftAnnotation.c_str();
  return this->CornerAnnotation->GetText (2);
}


void vtkSynchronizedView::SetUpRightAnnotation(const char *p_annotation)
{
  //this->UpRightAnnotation = p_annotation;  
  //this->UpdateAnnotations();
  this->CornerAnnotation->SetText (3, p_annotation);
  this->Modified();
}


const char *vtkSynchronizedView::GetUpRightAnnotation() const
{
  //return this->UpRightAnnotation.c_str();
  return this->CornerAnnotation->GetText (3);
}


void vtkSynchronizedView::SetDownLeftAnnotation(const char *p_annotation)
{
  //this->DownLeftAnnotation = p_annotation;  
  //this->UpdateAnnotations();
  this->CornerAnnotation->SetText (0, p_annotation);
  this->Modified();
}


const char *vtkSynchronizedView::GetDownLeftAnnotation() const
{
  //return this->DownLeftAnnotation.c_str();
  return this->CornerAnnotation->GetText (0);
}


void vtkSynchronizedView::SetDownRightAnnotation(const char *p_annotation)
{
  //this->DownRightAnnotation = p_annotation;  
  //this->UpdateAnnotations();
  this->CornerAnnotation->SetText (1, p_annotation);
  this->Modified();
}


const char *vtkSynchronizedView::GetDownRightAnnotation() const
{
  //return this->DownRightAnnotation.c_str();
  return this->CornerAnnotation->GetText (1);
}


void vtkSynchronizedView::SetNorthAnnotation(const char *p_annotation)
{
  //this->NorthAnnotation = p_annotation;  
  //this->UpdateAnnotations();
  this->OrientationAnnotation->SetText (1, p_annotation);
  this->Modified();
}


const char *vtkSynchronizedView::GetNorthAnnotation() const
{
  //return this->NorthAnnotation.c_str();
  return this->OrientationAnnotation->GetText (1);
}


void vtkSynchronizedView::SetSouthAnnotation(const char *p_annotation)
{
  //this->SouthAnnotation = p_annotation;
  //this->UpdateAnnotations();
  this->OrientationAnnotation->SetText (3, p_annotation);
  this->Modified();
}


const char *vtkSynchronizedView::GetSouthAnnotation() const
{
  //return this->SouthAnnotation.c_str();
  return this->OrientationAnnotation->GetText (3);
}


void vtkSynchronizedView::SetEastAnnotation(const char *p_annotation)
{
  //this->EastAnnotation = p_annotation;  
  //this->UpdateAnnotations();
  this->OrientationAnnotation->SetText (0, p_annotation);
  this->Modified();
}


const char *vtkSynchronizedView::GetEastAnnotation() const
{
  //return this->EastAnnotation.c_str();
  return this->OrientationAnnotation->GetText (0);
}


void vtkSynchronizedView::SetWestAnnotation(const char *p_annotation)
{
  //this->WestAnnotation = p_annotation;
  //this->UpdateAnnotations();
  this->OrientationAnnotation->SetText (2, p_annotation);
  this->Modified();
}


const char *vtkSynchronizedView::GetWestAnnotation() const
{
  //return this->WestAnnotation.c_str();
  return this->OrientationAnnotation->GetText (2);
}


void vtkSynchronizedView::SetAboutData (const char* str)
{
  this->AboutData = str;
  if( this->AboutDataVisibility )
    this->SetDownRightAnnotation ( str );
  this->Modified();
}


const char* vtkSynchronizedView::GetAboutData() const
{
  return this->AboutData.c_str();
}


void vtkSynchronizedView::SetAboutDataVisibility (int val)
{
  this->AboutDataVisibility = val;
  if( val )
  {
    this->SetDownRightAnnotation ( this->GetAboutData() );
  }
  else
  {
    this->SetDownRightAnnotation ("");
  }
}


#ifndef VTK_LEGACY_REMOVE
void vtkSynchronizedView::UpdateAnnotations()
{
  VTK_LEGACY_BODY (vtkSynchronizedView::UpdateAnnotations, "vtkINRIA3D 0.1");
}
#endif


void vtkSynchronizedView::SetShowAnnotations (int val)
{
  this->ShowAnnotations = val;
  if( val )
  {
    this->CornerAnnotation->SetVisibility (1);
  }
  else
  {
    this->CornerAnnotation->SetVisibility (0);
  }
  this->Modified();
}


void vtkSynchronizedView::SetShowDirections (int val)
{
  this->ShowDirections = val;
  if( val )
  {
    this->OrientationAnnotation->SetVisibility (1);
  }
  else
  {
    this->OrientationAnnotation->SetVisibility (0);
  }
  this->Modified();
}


void vtkSynchronizedView::SetBackgroundColor(double r, double g, double b)
{
  if (this->Renderer)
  {
    this->Renderer->SetBackground(r,g,b);
  }
  this->Modified();
}


void vtkSynchronizedView::SetTextColor (double color[3])
{
  this->SetTextColor ( color[0], color[1], color[2]);
  this->Modified();
}


void vtkSynchronizedView::SetTextColor (double r, double g, double b)
{
  this->TextProperty->SetColor (r, g, b);
  this->CornerAnnotation->Modified();
  this->Modified();
}


bool vtkSynchronizedView::HasChild (vtkSynchronizedView* view) const
{
  if( !view )
  {
    return false;
  }
  
  
  for( unsigned int i=0; i<this->Children.size(); i++)
  {
    if ( this->Children[i]==view )
    {
      return true;
    }
  }

  return false;
}



void vtkSynchronizedView::SetCameraPosition(double position[3])
{
  if (!this->GetRenderWindow()->GetNeverRendered())
    this->GetRenderer()->GetActiveCamera()->SetPosition(position);
  this->Modified();
}


void vtkSynchronizedView::SetCameraFocalPoint(double position[3])
{
  if(!this->GetRenderWindow()->GetNeverRendered())
    this->GetRenderer()->GetActiveCamera()->SetFocalPoint(position);
  this->Modified();
}


void vtkSynchronizedView::SetCameraViewUp(double position[3])
{
  if(!this->GetRenderWindow()->GetNeverRendered())
    this->GetRenderer()->GetActiveCamera()->SetViewUp(position);
  this->Modified();
}


void vtkSynchronizedView::SetCameraFocalAndPosition (double focal[3], double pos[3])
{
  if( !this->GetRenderer() )
  {
    return;
  }

  vtkCamera *camera = this->GetRenderer()->GetActiveCamera();
  double c_position[3], c_focal[3];
  camera->GetPosition(c_position);
  camera->GetFocalPoint (c_focal);

  camera->SetFocalPoint (focal[0], focal[1], c_focal[2]);
  camera->SetPosition   (pos[0], pos[1], c_position[2]);

  this->Modified();
}


void vtkSynchronizedView::GetCameraFocalAndPosition (double focal[3], double pos[3])
{
  if( !this->GetRenderer() )
  {
    return;
  }

  vtkCamera *camera = this->GetRenderer()->GetActiveCamera();
  camera->GetPosition(pos);
  camera->GetFocalPoint (focal);
}


void vtkSynchronizedView::SyncSetCameraFocalAndPosition (double focal[3], double pos[3])
{

  if( this->IsLocked() )
  {
    return;
  }

  if( this->GetLinkCameraFocalAndPosition() )
  {
    this->SetCameraFocalAndPosition (focal, pos);
  }

  this->Lock();
  for( unsigned int i=0; i<this->Children.size(); i++)
  {
    vtkSynchronizedView* view = vtkSynchronizedView::SafeDownCast (this->Children[i]);

    if( view /*&& view->GetLinkCameraFocalAndPosition()*/ )
    {
      view->SyncSetCameraFocalAndPosition (focal, pos);
      view->Update();
    }
  }
  this->UnLock();

}


void vtkSynchronizedView::ResetCamera()
{
  if (this->Renderer)
  {
    this->Renderer->ResetCameraClippingRange();
    this->Renderer->ResetCamera();
  }
}


void vtkSynchronizedView::Render()
{
  if (this->RenderWindow && !this->RenderWindow->GetNeverRendered() )
  {
    this->RenderWindow->Render();
    this->InternalMTime = this->GetMTime();
  }
}


void vtkSynchronizedView::SyncRender()
{
  
  if( this->IsLocked() )
  {
    return;
  }
  
  this->Render ();
  
  // this boolean is used so that the other observe won't call
  // Render() again and again and again...
  this->Lock();
  for( unsigned int i=0; i<this->Children.size(); i++)
  {
    vtkSynchronizedView* view = this->Children[i];
    if( view && view->GetLinkRender())
    {
      view->SyncRender ();
    }
  }
  
  this->UnLock();
}


void vtkSynchronizedView::Update()
{
  if( this->GetMTime()>this->InternalMTime )
  {
    this->Render();
  }
}


void vtkSynchronizedView::SyncUpdate()
{
  
  if( this->IsLocked() )
  {
    return;
  }
  
  this->Update ();
  
  // this boolean is used so that the other observe won't call
  // Render() again and again and again...
  this->Lock();
  for( unsigned int i=0; i<this->Children.size(); i++)
  {
    vtkSynchronizedView* view = this->Children[i];
    if( view && view->GetLinkRender())
    {
      view->SyncUpdate ();
    }
  }
  
  this->UnLock();
}


void vtkSynchronizedView::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Interaction-State = " << this->Interaction << "\n";
  os << indent << "Render Window = " << this->RenderWindow << "\n";
  if (this->GetRenderWindow())
    this->GetRenderWindow()->PrintSelf (os, indent);
  
}


void vtkSynchronizedView::SetParent (vtkSynchronizedView* parent)
{
  this->Parent = parent;
  this->Modified();
}


vtkSynchronizedView* vtkSynchronizedView::GetParent() const
{
  return this->Parent;
}


void vtkSynchronizedView::AddChild (vtkSynchronizedView* child)
{

  if( this->HasChild(child) ) // nothing to do
  {
    return;
  }

  if ( child==this ) // someone wants to add itself as a child to itself
  {
    // remove the child from its parent as apparently it doesn't want it anymore
    vtkSynchronizedView* parent = child->GetParent();
    if( parent )
    {
      parent->RemoveChild (child);
    }
    // but do nothing more
    return;
  }

  
  if( child )
  {
    /**
       We temporary store the child's parent
     */
    vtkSynchronizedView* parent = child->GetParent();
    child->Register  (this);

    /**
       Now that the child has changed its parent, we remove
       the child from its previous parent's children list.
       If we have done that earlier, this could result in
       a call to Delete(), since the RemoveChild function
       unregsiter the object.
     */
    
    if( parent )
    {
      parent->RemoveChild (child);
    }

    child->SetParent (this);    
    this->Children.push_back (child);
    
  }
  
}


void vtkSynchronizedView::AddChildren (std::vector<vtkSynchronizedView*> children)
{

  for( unsigned int i=0; i<children.size(); i++)
  {
    this->AddChild (children[i]);
  }
  
}


int vtkSynchronizedView::GetNumberOfChildren() const
{
  return (int)this->Children.size();
}


void vtkSynchronizedView::RemoveChild (vtkSynchronizedView* view)
{
  
  if( !view || this->Children.size()==0 )
  {
    return;
  }

  std::vector<vtkSynchronizedView*>::iterator it =
    std::find (this->Children.begin(), this->Children.end(), view);

  if( it==this->Children.end() ) // view is not in the list
  {
    return;
  }
  
  (*it)->SetParent (0);
  (*it)->UnRegister (this);
  this->Children.erase (it);  
}


void vtkSynchronizedView::RemoveAllChildren()
{

  for( unsigned int i=0; i<this->Children.size(); i++)
  {
	this->Children[i]->SetParent( 0 );
    this->Children[i]->UnRegister (this);
  }

  this->Children.clear();
  
}


void vtkSynchronizedView::Detach()
{
  
  vtkSynchronizedView* parent = this->GetParent();
  if( parent )
  {

    parent->AddChildren ( this->Children );
    parent->RemoveChild ( this );
    
    /**
       Handle the case where the parent's parent of the view is the view itself.
       Tell the it that it no longer has a parent, life is sad...
    */
    ;
    if( parent->GetParent()==this )
    {
      parent->SetParent(0);
      parent->UnRegister(this);
    }
    
  }
  else
  {
    this->RemoveAllChildren(); 
  }
  
}


void vtkSynchronizedView::Lock()
{
  this->IsProcessed = 1;
  //this->SetIsProcessed (1);
}


void vtkSynchronizedView::UnLock()
{
  this->IsProcessed = 0;
  //this->SetIsProcessed (0);
}
