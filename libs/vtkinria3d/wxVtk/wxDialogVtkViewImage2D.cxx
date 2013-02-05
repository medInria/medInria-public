/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxDialogVtkViewImage2D.cxx 1282 2009-10-08 17:00:40Z acanale $
Language:  C++
Author:    $Author: acanale $
Date:      $Date: 2009-10-08 19:00:40 +0200 (Thu, 08 Oct 2009) $
Version:   $Revision: 1282 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "wxDialogVtkViewImage2D.h"

#include "wxVTKRenderWindowInteractor.h"
#include "vtkViewImage2D.h"
#include "vtkRenderer.h"

wxDialogVtkViewImage2D::wxDialogVtkViewImage2D (wxWindow* parent, int id, const wxString& title,
                                                const wxPoint& pos, const wxSize& size,
                                                long style)
  : wxDialog (parent, id, title, pos, size, style)
{

  m_wxVtkView = new wxVTKRenderWindowInteractor(this , wxID_ANY,
                                       wxDefaultPosition,
                                       wxDefaultSize,
                                       wxWANTS_CHARS | wxNO_FULL_REPAINT_ON_RESIZE,
                                       title);
  
  m_View = vtkViewImage2D::New();
  vtkRenderer* rend = vtkRenderer::New();
  m_wxVtkView->GetRenderWindow()->AddRenderer (rend);

  m_View->SetRenderWindow (m_wxVtkView->GetRenderWindow());
  m_View->SetRenderer (rend);
  rend->Delete();
  
  this->SetProperties();
  this->DoLayout();
}


BEGIN_EVENT_TABLE (wxDialogVtkViewImage2D, wxDialog)
  EVT_CLOSE(wxDialogVtkViewImage2D::OnCloseEvent)
END_EVENT_TABLE()

void wxDialogVtkViewImage2D::OnCloseEvent (wxCloseEvent& event)
{
  if( !event.CanVeto() )
  {
    this->Destroy();
  }
  else
  {
    event.Veto();
    this->Show (false);
  }
}


wxDialogVtkViewImage2D::~wxDialogVtkViewImage2D()
{
//   m_View->Detach();
  m_wxVtkView->Delete();
  m_View->Delete();
}

void
wxDialogVtkViewImage2D::SetProperties()
{
  m_View->SetBackgroundColor (0.0, 0.0, 0.0);
  m_View->SetInteractionStyle (vtkViewImage2D::SELECT_INTERACTION);
}


void
wxDialogVtkViewImage2D::DoLayout()
{
  wxBoxSizer* sizer = new wxBoxSizer (wxHORIZONTAL);
  sizer->Add (m_wxVtkView, 1, wxEXPAND|wxALL, 5);
  this->SetSizerAndFit (sizer);
  this->SetSize (500,500);
  this->Layout();
}

