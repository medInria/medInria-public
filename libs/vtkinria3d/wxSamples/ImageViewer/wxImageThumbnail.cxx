/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxImageThumbnail.cxx 1356 2009-11-19 17:22:44Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2009-11-19 18:22:44 +0100 (Thu, 19 Nov 2009) $
Version:   $Revision: 1356 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "wxImageThumbnail.h"

#include <vtkViewImage2D.h>

#include "wxVTKRenderWindowInteractor.h"

#include <vtkImageData.h>
#include <vtkRenderer.h>


wxImageThumbnail::wxImageThumbnail (wxWindow* parent, int id,
                                    const wxPoint& pos,
                                    const wxSize& size,
                                    long style,
                                    const wxString& title)
  : wxPanel(parent, id, pos, size, style, title)
{

  // to be replaced by reading the config stuff in the interactor
  this->SetNumberOfColumns (4);
  // to be replaced by reading the config stuff in the interactor 
  this->KeepImageRatioOff();
  
  m_Sizer = new wxFlexGridSizer(100,this->GetNumberOfColumns(),5,5);
  for (unsigned int i=0; i<this->GetNumberOfColumns(); i++)
  {  
    m_Sizer->AddGrowableCol (i,1);
  }
  
  // TO DO : try to put a scrollbar instead of fixing rows to 100
  for( int i=0; i<100; i++)
  {
    m_Sizer->AddGrowableRow (i,1);
  }

  m_Sizer->SetFlexibleDirection (wxBOTH);
  m_Sizer->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_ALL);

  this->SetProperties();
  this->DoLayout();
}


void wxImageThumbnail::SetProperties()
{
  m_InteractionStyle = vtkViewImage2D::SELECT_INTERACTION;
  m_LinkViews        = true;

  m_OrientationFlag = vtkViewImage::AXIAL_ID;
  
}


void wxImageThumbnail::DoLayout()
{
  this->SetSizer (m_Sizer);
  this->Fit();
  this->SetAutoLayout (true);
  
  this->Layout();
}


BEGIN_EVENT_TABLE(wxImageThumbnail, wxPanel)
  //EVT_PAINT(wxImageThumbnail::OnPaintEvent)
END_EVENT_TABLE()

/*
void wxImageThumbnail::OnPaintEvent(wxPaintEvent& event)
{}
*/

void wxImageThumbnail::ChangeOrientation (unsigned int id)
{

  m_OrientationFlag = id;
  
  for( unsigned int i=0; i<m_ViewList.size(); i++)
  {
    m_ViewList[i]->SetOrientation (id);
  }
  this->Render();
}

  
void wxImageThumbnail::AddImage (vtkImageData* image, vtkMatrix4x4* matrix, const char* filename)
{
  
  if( !image )
  {
    return;
  }

  long style = wxWANTS_CHARS | wxNO_FULL_REPAINT_ON_RESIZE; // | wxSUNKEN_BORDER;
  
  vtkViewImage2D* view = vtkViewImage2D::New();
  
  wxVTKRenderWindowInteractor* wxView = new wxVTKRenderWindowInteractor (this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style, wxT(""));
  
  vtkRenderer* rend = vtkRenderer::New();
  
  view->SetRenderWindow (wxView->GetRenderWindow());
  view->SetRenderWindowInteractor(wxView);
  view->SetRenderer (rend);
  view->ScalarBarVisibilityOff();
  view->SetShow2DAxis (0);
  view->SetLinkZoom (true);
  view->SetLinkCameraFocalAndPosition(true);
  view->SetInteractionStyle (m_InteractionStyle);

  rend->Delete();
  
  m_Sizer->Add (wxView, 1, wxALL|wxEXPAND, 0);
//   view->SetOrientation (m_OrientationFlag);
  view->SetBackgroundColor (0.0,0.0,0.0);

  view->SetImage (image);
   
   view->SetDirectionMatrix (matrix);
   view->ResetCurrentPoint();
   view->ResetWindowLevel();
  view->Reset();
  
  view->SetAboutData (filename);

     
  if (m_LinkViews)
  {
    unsigned int nviews = m_ViewList.size();
    if( nviews>0 )
    {
       m_ViewList[nviews-1]->RemoveChild (m_ViewList[0]);
       m_ViewList[nviews-1]->AddChild (view);
       view->AddChild(m_ViewList[0]);
    }
  }
  
  m_ViewList.push_back (view);
  m_wxViewList.push_back (wxView);
  this->DoLayout();  
}

void wxImageThumbnail::ChangeImage(const int& i, vtkImageData* image)
{
  if( i<0 || i>=(int)(m_wxViewList.size()) || !image)
  {
    return;
  }

  m_ViewList[i]->SetImage(image);

   if( !m_wxViewList[i]->GetRenderWindow()->GetNeverRendered() )
   {
     m_ViewList[i]->Render();
   }
}


void wxImageThumbnail::RemoveImage (const int& i)
{
  if( i<0 || i>=(int)(m_wxViewList.size()) )
  {
    return;
  }

  std::vector<vtkViewImage2D*>              newList;
  std::vector<wxVTKRenderWindowInteractor*> newWxList;
 
  
  for( int j=0; j<(int)(m_wxViewList.size()); j++)
  {
    if( j==i )
    {
      bool isDetached = m_Sizer->Detach ( m_wxViewList[j] );
      if( !isDetached )
      {
        std::cerr << "Warning: child was not found." << std::endl;
      }
	  m_ViewList[j]->Detach();
      
      m_ViewList[j]->Delete();
      m_wxViewList[j]->Delete();
      m_Sizer->Layout();
    }
    else
    {  
      newList.push_back ( m_ViewList[j] );
      newWxList.push_back ( m_wxViewList[j] );
    }
  }

  m_ViewList = newList;
  m_wxViewList = newWxList;
}


void wxImageThumbnail::RemoveAll()
{
  for( unsigned int i=0; i<m_wxViewList.size(); i++)
  {
    m_ViewList[i]->Detach();
    m_Sizer->Detach (m_wxViewList[i]);
    m_ViewList[i]->Delete();
    m_wxViewList[i]->Delete();
  }
  
  m_ViewList.clear();
  m_wxViewList.clear();
  
  m_Sizer->Layout();  
}


void wxImageThumbnail::Render()
{
  for( unsigned int i=0; i<m_wxViewList.size(); i++)
  {
    if( !m_wxViewList[i]->GetRenderWindow()->GetNeverRendered() )
    {
      m_wxViewList[i]->Render();
    }
  }
 
}

void wxImageThumbnail::LinkViews (bool val)
{

  m_LinkViews = val;

  if (!m_LinkViews)
  {
    for( unsigned int i=0; i<m_ViewList.size(); i++)
    {
    m_ViewList[i]->Detach();
    }
  }
  else
  {

    for( unsigned int i=0; i<m_ViewList.size()-1; i++)
    {
      m_ViewList[i]->RemoveAllChildren();
      m_ViewList[i]->AddChild( m_ViewList[i+1] );
    }
    
     m_ViewList[m_ViewList.size()-1]->AddChild(m_ViewList[0]);
  }
  
}


void wxImageThumbnail::SetLookupTable (vtkLookupTable* lut)
{
  if( !lut )
  {
    return;
  }

  if( m_ViewList.size()>0 )
  {
     m_ViewList[0]->SyncSetLookupTable (lut);
  }
}

void wxImageThumbnail::SetShow2DAxis (bool value)
{
  for (unsigned int i=0; i<m_ViewList.size(); i++)
  {
     m_ViewList[i]->SetScalarBarVisibility (value);
  }
}
