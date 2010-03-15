/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxVtkPageView.cxx 1350 2009-11-19 15:16:30Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2009-11-19 16:16:30 +0100 (Thu, 19 Nov 2009) $
Version:   $Revision: 1350 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "wxVtkPageView.h"
#include "vtkViewImage2D.h"
#include "vtkViewImage2DWithTracer.h"
#include "vtkViewImage3D.h"

#include "wxVTKRenderWindowInteractor.h"
#include <vtkImageData.h>
#include <vtkLookupTable.h>
#include <vtkImageReslice.h>
#include <vtkImageShiftScale.h>
#include <vtkRenderer.h>


wxVtkPageView::wxVtkPageView (wxWindow* parent, int id,
                              const wxPoint& pos, const wxSize& size,
                              long styl, const wxString& title,
							  int layoutMode, int padSize)
  : wxPanel (parent, id, pos, size, styl, title)
{
  long style = wxWANTS_CHARS | wxNO_FULL_REPAINT_ON_RESIZE; // | wxSUNKEN_BORDER;

  m_ViewInteractor1 = new wxVTKRenderWindowInteractor (this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style, wxT(""));
  m_ViewInteractor2 = new wxVTKRenderWindowInteractor (this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style, wxT(""));
  m_ViewInteractor3 = new wxVTKRenderWindowInteractor (this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style, wxT(""));
  m_ViewInteractor4 = new wxVTKRenderWindowInteractor (this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style, wxT(""));

  m_View1 = 0;
  m_View2 = 0;
  m_View3 = 0;
  m_View4 = 0;
  
  m_LayoutMode = layoutMode;
  m_PadSpace = padSize;
  m_FullScreenMode = vtkViewImage::NB_PLAN_IDS;
  m_OrientationMatrix = vtkMatrix4x4::New();
  m_OrientationMatrix->Identity();
  
  
#ifdef vtkINRIA3D_USE_ITK
  this->ItkImage = 0;
#endif
  
  this->SetProperties();
  this->DoLayout();
}

wxVtkPageView::~wxVtkPageView()
{
  m_View1->Detach();
  m_View2->Detach();
  m_View3->Detach();
  m_View4->Detach();

  m_View1->Delete();
  m_View2->Delete();
  m_View3->Delete();
  m_View4->Delete();

  m_OrientationMatrix->Delete();

  this->SetSizer( NULL );

  m_ViewInteractor1->Delete();
  m_ViewInteractor2->Delete();
  m_ViewInteractor3->Delete();
  m_ViewInteractor4->Delete();
  
#ifdef vtkINRIA3D_USE_ITK
  this->ItkImage = 0;
#endif
}


BEGIN_EVENT_TABLE(wxVtkPageView, wxPanel)
  //EVT_PAINT(wxVtkPageView::OnPaintEvent)
END_EVENT_TABLE()

/*
void wxVtkPageView::OnPaintEvent(wxPaintEvent& event)
{
}
*/

void wxVtkPageView::SetProperties()
{

  if (m_View1){
    m_View1->Detach();
    m_View1->Delete();
  }
  if (m_View2){
    m_View2->Detach();
    m_View2->Delete();
  }
  if (m_View3){
    m_View3->Detach();
    m_View3->Delete();
  }
  if (m_View4){
    m_View4->Detach();
    m_View4->Delete();
  }  

  switch(m_ViewMode)
  {
      case VIEW_MODE_TRACING:
	m_View1 = vtkViewImage2DWithTracer::New();
	m_View2 = vtkViewImage2DWithTracer::New();
	m_View3 = vtkViewImage2DWithTracer::New();
	m_View4 = vtkViewImage3D::New();
	break;

      case VIEW_MODE_NORMAL:
      default:
	m_View1 = vtkViewImage2D::New();
	m_View2 = vtkViewImage2D::New();
	m_View3 = vtkViewImage2D::New();
	m_View4 = vtkViewImage3D::New();
	break;
  }
 
  vtkRenderer* rend1 = vtkRenderer::New();
  vtkRenderer* rend2 = vtkRenderer::New();
  vtkRenderer* rend3 = vtkRenderer::New();
  vtkRenderer* rend4 = vtkRenderer::New();
  

  m_View1->SetRenderWindow (m_ViewInteractor1->GetRenderWindow());
  m_View2->SetRenderWindow (m_ViewInteractor2->GetRenderWindow());
  m_View3->SetRenderWindow (m_ViewInteractor3->GetRenderWindow());
  m_View4->SetRenderWindow (m_ViewInteractor4->GetRenderWindow());

  m_View1->SetRenderWindowInteractor (m_ViewInteractor1);
  m_View2->SetRenderWindowInteractor (m_ViewInteractor2);
  m_View3->SetRenderWindowInteractor (m_ViewInteractor3);
  m_View4->SetRenderWindowInteractor (m_ViewInteractor4);

  m_View1->SetRenderer (rend1);
  m_View2->SetRenderer (rend2);
  m_View3->SetRenderer (rend3);
  m_View4->SetRenderer (rend4);
  
  m_View1->SetBackgroundColor (0.0,0.0,0.0);
  m_View2->SetBackgroundColor (0.0,0.0,0.0);
  m_View3->SetBackgroundColor (0.0,0.0,0.0);
//   m_View4->SetBackgroundColor (0.0,0.0,0.0);  
  m_View1->SetInteractionStyle (vtkViewImage2D::SELECT_INTERACTION);
  m_View2->SetInteractionStyle (vtkViewImage2D::SELECT_INTERACTION);
  m_View3->SetInteractionStyle (vtkViewImage2D::SELECT_INTERACTION);

  m_View1->CursorFollowMouseOn();
  m_View2->CursorFollowMouseOn();
  m_View3->CursorFollowMouseOn();
  
  rend1->Delete();
  rend2->Delete();
  rend3->Delete();
  rend4->Delete();

  m_View1->SetOrientation( vtkViewImage2D::AXIAL_ID);
  m_View2->SetOrientation( vtkViewImage2D::CORONAL_ID);
  m_View3->SetOrientation( vtkViewImage2D::SAGITTAL_ID);

  m_View1->ScalarBarVisibilityOn();
  m_View2->ScalarBarVisibilityOn();
  m_View3->ScalarBarVisibilityOn();
  m_View4->ScalarBarVisibilityOn();

  double textcolor[3]={0.0,0.0,0.0};
  m_View4->SetTextColor (textcolor);
  m_View4->SetRenderingModeToPlanar();

  m_View1->AddChild (m_View2);
  m_View1->AddChild (m_View4);
  m_View2->AddChild (m_View3);
  m_View3->AddChild (m_View1);
}


void wxVtkPageView::DoLayout()
{
  wxFlexGridSizer* sizerViews = NULL;
  wxFlexGridSizer* sizer1 = NULL;
  wxFlexGridSizer* sizer2 = NULL;
  
  switch(this->GetLayoutMode())
  {
      case LAYOUT_NO_3D :
	
 	m_ViewInteractor4->Show (false);	
	
	sizerViews= new wxFlexGridSizer (2,1, m_PadSpace, m_PadSpace);
	sizerViews->AddGrowableCol (0,1);
	sizerViews->AddGrowableRow (0,5);
	sizerViews->AddGrowableRow (1,3);
	
	sizer1= new wxFlexGridSizer (1,1, m_PadSpace, m_PadSpace);
	sizer1->AddGrowableCol (0,1);
	sizer1->AddGrowableRow (0,1);
	
	sizer1->Add (m_ViewInteractor1, 1, wxALL|wxEXPAND,0);
	
	/*
	  sizer2= new wxFlexGridSizer (1,3, m_PadSpace, m_PadSpace);
	  sizer2->AddGrowableCol (0,1000);
	  sizer2->AddGrowableCol (1,1000);
	  sizer2->AddGrowableCol (2,1);
	  sizer2->AddGrowableRow (0,1);
	*/
	
	sizer2 = new wxFlexGridSizer (1,2, m_PadSpace, m_PadSpace);
	sizer2->AddGrowableCol (0,1);
	sizer2->AddGrowableCol (1,1);	
	sizer2->AddGrowableRow (0,1);
	
	sizer2->Add (m_ViewInteractor2, 1, wxALL|wxEXPAND,0);
	sizer2->Add (m_ViewInteractor3, 1, wxALL|wxEXPAND,0);
	//sizer2->Add (m_ViewInteractor4, 1, wxALL|wxEXPAND,0);
	
	sizerViews->Add (sizer1, 1, wxALL|wxEXPAND,0);
	sizerViews->Add (sizer2, 1, wxALL|wxEXPAND,0);
	
	break;

	
      case LAYOUT_NORMAL :
	
      default :
	
	sizerViews= new wxFlexGridSizer (2,2, m_PadSpace, m_PadSpace);
	//wxGridSizer* sizerViews = new wxGridSizer (2,2,0,0);
	sizerViews->AddGrowableCol (0,1);
	sizerViews->AddGrowableCol (1,1);
	sizerViews->AddGrowableRow (0,1);
	sizerViews->AddGrowableRow (1,1);
	
	sizerViews->Add (m_ViewInteractor1, 1, wxALL|wxEXPAND,0);
	sizerViews->Add (m_ViewInteractor2, 1, wxALL|wxEXPAND,0);
	sizerViews->Add (m_ViewInteractor3, 1, wxALL|wxEXPAND,0);
	sizerViews->Add (m_ViewInteractor4, 1, wxALL|wxEXPAND,0);

	break;
  }

  this->SetSizer(sizerViews);
    
  this->Fit();
  this->SetAutoLayout (true);
  this->Layout();

  
}

void wxVtkPageView::SetOrientationMatrix (vtkMatrix4x4* matrix)
{
  if (matrix)
    m_OrientationMatrix->DeepCopy (matrix);
}


void wxVtkPageView::SetInput(vtkImageData* image, const bool& reset, vtkMatrix4x4* orientationmatrix)
{

  m_View1->SetDirectionMatrix ( orientationmatrix );
  m_View2->SetDirectionMatrix ( orientationmatrix );
  m_View3->SetDirectionMatrix ( orientationmatrix );
  m_View4->SetDirectionMatrix ( orientationmatrix );

  
  m_View1->SetImage(image);
  m_View2->SetImage(image);
  m_View3->SetImage(image);  


  switch(this->GetLayoutMode())
  {
      case LAYOUT_NO_3D :
	break;
      default:
	m_View4->SetImage ( image );
	break;
  }

    
  if( reset )
  {
    m_View1->SyncResetPosition();
    m_View1->SyncResetWindowLevel();
    m_View1->ResetZoom();
    m_View2->ResetZoom();
    m_View3->ResetZoom();
    m_View4->ResetZoom();
  }
  this->SetOrientationMatrix (orientationmatrix);
    
}


#ifdef vtkINRIA3D_USE_ITK
void wxVtkPageView::SetITKInput(ImageType::Pointer image, const bool& reset)
{
  this->ItkImage = image;

}

wxVtkPageView::ImageType::Pointer wxVtkPageView::GetItkImage()
{
  return this->ItkImage;
}
#endif

vtkImageData*  wxVtkPageView::GetInput() const
{
  return m_View1->GetImage();
}


void wxVtkPageView::OnFullScreen (unsigned int id)
{
  switch (id)
  {
      case vtkViewImage::AXIAL_ID :
	m_ViewInteractor1->Show (true);
	m_ViewInteractor2->Show (false);
	m_ViewInteractor3->Show (false);
	m_ViewInteractor4->Show (false);
	
	break;
      case vtkViewImage::CORONAL_ID :
	m_ViewInteractor1->Show (false);
	m_ViewInteractor2->Show (true);
	m_ViewInteractor3->Show (false);
	m_ViewInteractor4->Show (false);
	
	break;
      case vtkViewImage::SAGITTAL_ID :
	m_ViewInteractor1->Show (false);
	m_ViewInteractor2->Show (false);
	m_ViewInteractor3->Show (true);
	m_ViewInteractor4->Show (false);
	
	break;
      case vtkViewImage::NB_PLAN_IDS :

	if (this->GetLayoutMode() == LAYOUT_NO_3D)
	  break;
	
	m_ViewInteractor1->Show (false);
	m_ViewInteractor2->Show (false);
	m_ViewInteractor3->Show (false);
	m_ViewInteractor4->Show (true);
	
	break;
      default :
	break;
  }

  m_FullScreenMode = id;
  
  this->Layout();
}

void wxVtkPageView::NoFullScreen (void)
{
  m_ViewInteractor1->Show (true);
  m_ViewInteractor2->Show (true);
  m_ViewInteractor3->Show (true);

  m_FullScreenMode = vtkViewImage::NB_PLAN_IDS;
  
  if (this->GetLayoutMode() != LAYOUT_NO_3D)
    m_ViewInteractor4->Show (true);

  this->Layout();
}

  
void wxVtkPageView::SetInteractionStyle (int i)
{
  m_View1->SetInteractionStyle (i);
  m_View2->SetInteractionStyle (i);
  m_View3->SetInteractionStyle (i);
}

void wxVtkPageView::SetInteractionStyleToSelector()
{
  m_View1->SetInteractionStyle (vtkViewImage2D::SELECT_INTERACTION);
  m_View2->SetInteractionStyle (vtkViewImage2D::SELECT_INTERACTION);
  m_View3->SetInteractionStyle (vtkViewImage2D::SELECT_INTERACTION);
}

void wxVtkPageView::SetInteractionStyleToWindowLevel()
{
  m_View1->SetInteractionStyle (vtkViewImage2D::WINDOW_LEVEL_INTERACTION);
  m_View2->SetInteractionStyle (vtkViewImage2D::WINDOW_LEVEL_INTERACTION);
  m_View3->SetInteractionStyle (vtkViewImage2D::WINDOW_LEVEL_INTERACTION);
}

void wxVtkPageView::SetInteractionStyleToZoom()
{
  m_View1->SetInteractionStyle (vtkViewImage2D::ZOOM_INTERACTION);
  m_View2->SetInteractionStyle (vtkViewImage2D::ZOOM_INTERACTION);
  m_View3->SetInteractionStyle (vtkViewImage2D::ZOOM_INTERACTION);
}

void wxVtkPageView::SetInteractionStyleToFullPage()
{
  m_View1->SetInteractionStyle (vtkViewImage2D::FULL_PAGE_INTERACTION);
  m_View2->SetInteractionStyle (vtkViewImage2D::FULL_PAGE_INTERACTION);
  m_View3->SetInteractionStyle (vtkViewImage2D::FULL_PAGE_INTERACTION);
}

void wxVtkPageView::SetRenderingModeToMPR()
{
  if (this->GetLayoutMode() != LAYOUT_NO_3D)
    m_View4->SetRenderingModeToPlanar();
}

void wxVtkPageView::SetRenderingModeToVR()
{
  if (this->GetLayoutMode() != LAYOUT_NO_3D)
    m_View4->SetRenderingModeToVR();
}


void wxVtkPageView::SetLookupTable (vtkLookupTable* lut)
{

  if( !lut )
  {
    return;
  }

  m_View1->SetLookupTable (lut);
  m_View2->SetLookupTable (lut);
  m_View3->SetLookupTable (lut);
  m_View4->SetLookupTable (lut);
}

void wxVtkPageView::Refresh()
{
//   m_View1->GetImageReslice()->Update();
//   m_View2->GetImageReslice()->Update();
//   m_View3->GetImageReslice()->Update();  
  if(!this->GetInput())
    return;
  
  m_View1->GetImage()->Modified();
  m_View2->GetImage()->Modified();
  m_View3->GetImage()->Modified();
  m_View1->Render();
}



void wxVtkPageView::Render()
{
  if( m_ViewInteractor1->IsShownOnScreen() )
  {
    m_View1->Render();
  }
  if( m_ViewInteractor2->IsShownOnScreen() )
  {
    m_View2->Render();
  }
  if( m_ViewInteractor3->IsShownOnScreen() )
  {
    m_View3->Render();
  }
  if( m_ViewInteractor4->IsShownOnScreen() )
  {
    m_View4->Render();
  }
}


wxVTKRenderWindowInteractor* wxVtkPageView::GetDisplayedView (void) const
{
  if (m_ViewInteractor1->IsShown() && m_ViewInteractor2->IsShown() &&
      m_ViewInteractor3->IsShown() && m_ViewInteractor4->IsShown())
    return NULL;

  if (m_ViewInteractor1->IsShown())
    return m_ViewInteractor1;
  if (m_ViewInteractor2->IsShown())
    return m_ViewInteractor2;
  if (m_ViewInteractor3->IsShown())
    return m_ViewInteractor3;
  if (m_ViewInteractor4->IsShown())
    return m_ViewInteractor4;

  return NULL;
  
}

  
void wxVtkPageView::DetachViews (void)
{
  m_View1->Detach();
  m_View2->Detach();
  m_View3->Detach();
  m_View4->Detach();
}

  
void wxVtkPageView::AttachViews (void)
{
  m_View1->AddChild (m_View2);
  m_View2->AddChild (m_View3);
  m_View3->AddChild (m_View1);
  m_View1->AddChild (m_View4);
}
