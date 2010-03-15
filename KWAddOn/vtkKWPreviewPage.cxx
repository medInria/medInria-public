/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWPreviewPage.cxx 572 2007-12-21 15:52:58Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2007-12-21 16:52:58 +0100 (Fri, 21 Dec 2007) $
Version:   $Revision: 572 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "vtkKWPreviewPage.h"
#include "vtkObjectFactory.h"

#include <vtkViewImage2D.h>

#include "vtkKWApplication.h"
#include "vtkKWRenderWidget.h"
#include <vtkKWSeparator.h>
#include "vtkKWIcon.h"
#include "vtkKWFrameWithScrollbar.h"

#include <vtkViewImage2D.h>
#include <vtkViewImage3D.h>
#include <vtkLookupTableManager.h>

#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCollection.h>
#include <vtkLookupTable.h>
#include <vtkRendererCollection.h>
#include <vtkActor.h>
#include <vtkKWWidgetSet.h>
#include <vtkKWTopLevel.h>

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWPreviewPage );
vtkCxxRevisionMacro( vtkKWPreviewPage, "$Revision: 1 $");


//----------------------------------------------------------------------------
vtkKWPreviewPage::vtkKWPreviewPage()
{

  this->InternalFrame = vtkKWFrameWithScrollbar::New();

  
  this->ViewList = vtkCollection::New();
  this->RenderWidgetList = vtkCollection::New();

  vtkLookupTable* lut = vtkLookupTableManager::GetLookupTable (vtkLookupTableManager::LUT_BW);
  this->SetLookupTable (lut);
  lut->Delete();
  
  this->LinkViews = true;
  this->OrientationMode = 2;
  this->MaxNumberOfColumns = 5;
  this->InteractionMode = vtkViewImage2D::SELECT_INTERACTION;
}

//----------------------------------------------------------------------------
vtkKWPreviewPage::~vtkKWPreviewPage()
{
  std::cout<<"deleting vtkKWPreviewPage "<<std::endl;

  for (unsigned int i=0; i<this->GetNumberOfPreviews(); i++)
  {
    vtkViewImage2D* view = vtkViewImage2D::SafeDownCast (this->ViewList->GetItemAsObject (i));
    vtkKWRenderWidget* widget = vtkKWRenderWidget::SafeDownCast (this->RenderWidgetList->GetItemAsObject (i));
    view->Detach();
    widget->SetParent (NULL);
  }

  this->ViewList->Delete();
  this->RenderWidgetList->Delete();

  this->InternalFrame->Delete();
  
}

//----------------------------------------------------------------------------
void vtkKWPreviewPage::ConfigureView(vtkViewImage* view, vtkKWRenderWidget* widget)
{
  vtkRenderer* renderer = vtkRenderer::New();
  vtkRendererCollection* collection = widget->GetRenderWindow()->GetRenderers();
  collection->RemoveAllItems();
  widget->GetRenderWindow()->AddRenderer (renderer);
  view->SetRenderWindow (widget->GetRenderWindow());
  view->SetRenderer (renderer);
  renderer->Delete();
}


//----------------------------------------------------------------------------
void vtkKWPreviewPage::AddPreviewImage (vtkImageData* image, const char* name, vtkMatrix4x4* matrix )
{


  vtkViewImage2D* view = vtkViewImage2D::New();
  vtkKWRenderWidget* widget = vtkKWRenderWidget::New();
  
  widget->SetParent (this->InternalFrame->GetFrame());
  widget->Create();
  //widget->SetWidth (800);
  //widget->SetHeight (800);
  widget->SetBorderWidth (1);

  this->ConfigureView (view, widget);
  view->SetLinkZoom (true);
  view->ScalarBarVisibilityOff();
  view->SetLinkCameraFocalAndPosition(true);  
  
  view->SetImage (image);
  view->SetAboutData (name);
  view->SetBackgroundColor (0,0,0);
  view->SetShowAnnotations (false);

  
  if (matrix)
    view->SetDirectionMatrix (matrix);
  view->ResetCurrentPoint();
  view->ResetWindowLevel();

  
  view->SetOrientation (this->GetOrientationMode());
  
  
  this->ViewList->AddItem (view);
  this->RenderWidgetList->AddItem (widget);

  if (this->GetNumberOfPreviews() > 1)
  {
    vtkViewImage2D* previous_view = vtkViewImage2D::SafeDownCast (this->ViewList->GetItemAsObject (this->GetNumberOfPreviews()-2));
    vtkViewImage2D* first_view = vtkViewImage2D::SafeDownCast (this->ViewList->GetItemAsObject (0));
    if (previous_view && first_view)
    {
      previous_view->RemoveChild (first_view);
      previous_view->AddChild (view);
      view->AddChild (first_view);
    }
  }
  
  
  view->Delete();
  widget->Delete();

  //this->Update();
  
  
}

//----------------------------------------------------------------------------
void vtkKWPreviewPage::RemovePreviewImage (vtkImageData* image)
{

  int id = -1;
  
  this->FindView (image, id);
  
  if (id == -1)
    return;
  
  vtkViewImage2D* view = vtkViewImage2D::SafeDownCast (this->ViewList->GetItemAsObject (id));
  vtkKWRenderWidget* widget = vtkKWRenderWidget::SafeDownCast (this->RenderWidgetList->GetItemAsObject (id));

  view->Detach();

  widget->SetParent (NULL);
  
  this->ViewList->RemoveItem (view);
  this->RenderWidgetList->RemoveItem (widget);
  
}


//----------------------------------------------------------------------------
void vtkKWPreviewPage::Render (void)
{

  for (unsigned int i=0; i<this->GetNumberOfPreviews(); i++)
  {
    vtkKWRenderWidget* widget = vtkKWRenderWidget::SafeDownCast (this->RenderWidgetList->GetItemAsObject (i));
    if (!widget->IsMapped())
      continue;
    widget->Render();
  }
 
}


//----------------------------------------------------------------------------
void vtkKWPreviewPage::SetOrientationMode (int mode)
{
  for (unsigned int i=0; i<this->GetNumberOfPreviews(); i++)
  {
    vtkViewImage2D* view = vtkViewImage2D::SafeDownCast (this->ViewList->GetItemAsObject (i));
    view->SetOrientation (mode);
  }

  this->OrientationMode = mode;
  this->Render();
  
}


//----------------------------------------------------------------------------
void vtkKWPreviewPage::SetInteractionMode (int mode)
{
  for (unsigned int i=0; i<this->GetNumberOfPreviews(); i++)
  {
    vtkViewImage2D* view = vtkViewImage2D::SafeDownCast (this->ViewList->GetItemAsObject (i));
    view->SetInteractionStyle (mode);
  }

  this->InteractionMode = mode;
}




//----------------------------------------------------------------------------
void vtkKWPreviewPage::SetLookupTable (vtkLookupTable* lut)
{
  
  for (unsigned int i=0; i<this->GetNumberOfPreviews(); i++)
  {
    vtkViewImage2D* view = vtkViewImage2D::SafeDownCast (this->ViewList->GetItemAsObject (i));
    view->SetLookupTable (lut);
  }
  this->LookupTable = lut;  
  
}


//----------------------------------------------------------------------------
void vtkKWPreviewPage::Update (void)
{
  if (!this->IsCreated())
    return;
  
  this->PackSelf();
  this->Render();
}



//----------------------------------------------------------------------------
vtkViewImage2D* vtkKWPreviewPage::FindView(vtkImageData* imagedata, int &cookie)
{
  for (unsigned int i=0; i<this->GetNumberOfPreviews(); i++)
  {
    vtkViewImage2D* view = vtkViewImage2D::SafeDownCast(this->ViewList->GetItemAsObject(i));
    if (view)
    {
      if (view->GetImage() == imagedata)
      {
	cookie = i;
	return view;
      }
    }
  }

  return NULL;
}


//----------------------------------------------------------------------------
void vtkKWPreviewPage::CreateWidget()
{
  // Check if already created

  if (this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }
  
  // Call the superclass to create the whole widget
  this->Superclass::CreateWidget();

  this->InternalFrame->SetParent (this);
  this->InternalFrame->HorizontalScrollbarVisibilityOff();
  this->InternalFrame->VerticalScrollbarVisibilityOff();
  this->InternalFrame->Create();
  
  this->Script("pack %s -side top -expand t -fill both", 
	       this->InternalFrame->GetWidgetName());
  
  
  this->PackSelf();  
  
}


void vtkKWPreviewPage::PackSelf()
{

  if (!this->IsCreated())
  {
    //vtkErrorMacro("don't call this method before calling Create() : no effect !");
    return;
  }
  
  this->InternalFrame->GetFrame()->UnpackChildren();

  unsigned int N = this->GetNumberOfPreviews();

  if (N > 16)
  {
    this->InternalFrame->VerticalScrollbarVisibilityOn();
    this->InternalFrame->HorizontalScrollbarVisibilityOn();
  }
  else
  {
    this->InternalFrame->VerticalScrollbarVisibilityOff();
    this->InternalFrame->HorizontalScrollbarVisibilityOff();
  }

  unsigned int NumberOfCols = 0;
  if (N <= 1)
    NumberOfCols = N;
  else if (N <= 4)
    NumberOfCols = 2;
  else if (N <= 9)
    NumberOfCols = 3;
  else
    NumberOfCols = 4;    
  
  unsigned int iter_col = 0;
  unsigned int iter_row = 0;

  unsigned int size = 350;
  vtkKWTopLevel* parent = this->GetParentTopLevel();
  
  if (parent)
  {
    size = (unsigned int)( (double)(parent->GetWidth() * 4.0)/(double)(4.0 * 5.0) );
  }
  
  for (unsigned int i=0; i<N; i++)
  {
    vtkKWRenderWidget* widget = vtkKWRenderWidget::SafeDownCast (this->RenderWidgetList->GetItemAsObject (i));
    if (N > 16)
    {
      widget->SetWidth (size);
      widget->SetHeight (size);
    }

    this->Script ("grid %s -column %u -row %u -sticky news",
		  widget->GetWidgetName(), iter_col, iter_row);

    iter_col++;
    
    if (iter_col >= NumberOfCols)
    {
      iter_row++;
      iter_col = 0;
    }
    
  }


  unsigned NumberOfRows = iter_row+1;
  if ((N == 1) || (N == 2) || (N == 4) || (N == 9) || (N == 16))
    NumberOfRows--;  

  for (unsigned int i=0; i<NumberOfCols; i++)
  {
    this->Script ("grid columnconfigure %s %d -weight 1",
		  this->InternalFrame->GetFrame()->GetWidgetName(), i);
  }
  for (unsigned int i=0; i<NumberOfRows; i++)
  {
    this->Script ("grid rowconfigure %s %d -weight 1",
		  this->InternalFrame->GetFrame()->GetWidgetName(), i);
  }


}

