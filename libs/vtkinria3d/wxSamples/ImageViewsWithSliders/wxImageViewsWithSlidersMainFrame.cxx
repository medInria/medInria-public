/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxHelloWorldMainFrame.cxx 674 2008-02-13 18:53:40Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2008-02-13 19:53:40 +0100 (Wed, 13 Feb 2008) $
Version:   $Revision: 674 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "wxImageViewsWithSlidersMainFrame.h"

#include "fileopen.xpm"

#include <vtkMetaImageData.h>
#include <wxVtkViewImage2DWithSliders.h>
#include <vtkRenderingAddOn/vtkViewImage2DWithTracer.h>

wxImageViewsWithSlidersMainFrame::wxImageViewsWithSlidersMainFrame (wxWindow* parent, int id,
                                                                    const wxString& title,
                                                                    const wxPoint& pos,
                                                                    const wxSize& size,
                                                                    long style )
  : wxFrame ( parent, id, title, pos, size, style )
{
  
  m_View1 = new wxVtkViewImage2DWithSliders (this, wxID_ANY);
  m_View2 = new wxVtkViewImage2DWithSliders (this, wxID_ANY);
  m_View3 = new wxVtkViewImage2DWithSliders (this, wxID_ANY);

  
    
  /**
     We create a toolbar with the open button
  */
  wxToolBar* toolBar = new wxToolBar(this, wxID_ANY,
                                     wxDefaultPosition,
                                     wxDefaultSize,
                                     wxTB_HORIZONTAL|wxTB_DOCKABLE|wxTB_3DBUTTONS|wxTB_TEXT|wxTB_FLAT);
  toolBar->SetToolBitmapSize(wxSize(32,32));
  toolBar->AddTool (OPEN_BUTTON, wxT("Open"),
                    wxBitmap(fileopen), wxNullBitmap,
                    wxITEM_NORMAL, wxT(""), wxT(""));
  toolBar->Realize(); // actually create the toolbar

  // we set the toolbar to the wxFrame...
  this->SetToolBar (toolBar);

  // ...and we create a status bar
  this->CreateStatusBar(1);
  this->SetStatusText( wxT("Welcome to wxHelloWorld") );


  // finish:
  this->DoLayout();
  this->SetProperties();
  
}

  
wxImageViewsWithSlidersMainFrame::~wxImageViewsWithSlidersMainFrame()
{
}


/**
   Table event: we associate to an id (here: OPEN_BUTTON) a function of the class
   (here: OnOpenButton). The macro EVT_TOOL will tell wxWidgets that the toolbar
   button with id OPEN_BUTTON is pressed, then call OnOpenButton function.
 */

BEGIN_EVENT_TABLE (wxImageViewsWithSlidersMainFrame, wxFrame)
  EVT_TOOL (OPEN_BUTTON, wxImageViewsWithSlidersMainFrame::OnOpenButton)
END_EVENT_TABLE()




void wxImageViewsWithSlidersMainFrame::SetProperties()
{

  /**
     In this function, we specify some properties of our views, like the type
     of interaction, the 3D rendering mode, etc.
   */

  m_View1->GetViewImage2D()->SetOrientation (2);
  m_View2->GetViewImage2D()->SetOrientation (0);
  m_View3->GetViewImage2D()->SetOrientation (1);
  
  m_View1->GetViewImage2D()->SetLeftButtonInteractionStyle   (vtkViewImage2D::ZOOM_INTERACTION);
  m_View1->GetViewImage2D()->SetMiddleButtonInteractionStyle (vtkViewImage2D::SELECT_INTERACTION);
  m_View1->GetViewImage2D()->SetWheelInteractionStyle        (vtkViewImage2D::SELECT_INTERACTION);
  m_View1->GetViewImage2D()->SetRightButtonInteractionStyle  (vtkViewImage2D::WINDOW_LEVEL_INTERACTION);

  m_View2->GetViewImage2D()->SetLeftButtonInteractionStyle   (vtkViewImage2D::ZOOM_INTERACTION);
  m_View2->GetViewImage2D()->SetMiddleButtonInteractionStyle (vtkViewImage2D::SELECT_INTERACTION);
  m_View2->GetViewImage2D()->SetWheelInteractionStyle        (vtkViewImage2D::SELECT_INTERACTION);
  m_View2->GetViewImage2D()->SetRightButtonInteractionStyle  (vtkViewImage2D::WINDOW_LEVEL_INTERACTION);

  m_View3->GetViewImage2D()->SetLeftButtonInteractionStyle   (vtkViewImage2D::ZOOM_INTERACTION);
  m_View3->GetViewImage2D()->SetMiddleButtonInteractionStyle (vtkViewImage2D::SELECT_INTERACTION);
  m_View3->GetViewImage2D()->SetWheelInteractionStyle        (vtkViewImage2D::SELECT_INTERACTION);
  m_View3->GetViewImage2D()->SetRightButtonInteractionStyle  (vtkViewImage2D::WINDOW_LEVEL_INTERACTION);

  
  m_View1->GetViewImage2D()->SetBackgroundColor (0.0,0.0,0.0);
  m_View2->GetViewImage2D()->SetBackgroundColor (0.0,0.0,0.0);
  m_View3->GetViewImage2D()->SetBackgroundColor (0.0,0.0,0.0);


  m_View1->GetViewImage2D()->AddChild (m_View2->GetViewImage2D());
  m_View2->GetViewImage2D()->AddChild (m_View3->GetViewImage2D());
  m_View3->GetViewImage2D()->AddChild (m_View1->GetViewImage2D());
  
}



void wxImageViewsWithSlidersMainFrame::DoLayout()
{

  wxBoxSizer* sizer = new wxBoxSizer (wxVERTICAL);


  wxBoxSizer* sizer1 = new wxBoxSizer (wxHORIZONTAL);  
  sizer1->Add (m_View2, 1, wxALL|wxEXPAND, 0);
  sizer1->Add (m_View3, 1, wxALL|wxEXPAND, 0);
  
  sizer->Add (m_View1, 2, wxALL|wxEXPAND, 0);
  sizer->Add (sizer1,  1, wxALL|wxEXPAND, 0);
  
  this->SetSizerAndFit (sizer);
  this->SetAutoLayout (true);
  this->SetSize (500,500);
  this->Layout();
}




void wxImageViewsWithSlidersMainFrame::OnOpenButton (wxCommandEvent& event)
{

  wxFileDialog* myFileDialog = new wxFileDialog(this, wxT ("Open an Image"),
                                                wxT(""), wxT(""),
                                                wxT("All (*.*)|*.*"),
                                                wxFD_OPEN|wxFD_CHANGE_DIR, wxDefaultPosition);
  wxString fileName;
  fileName.Empty();
  int OK = myFileDialog->ShowModal();
  
  if( OK==wxID_OK )
  {
    fileName = myFileDialog->GetPath();
  }
  
  myFileDialog->Destroy();
  

  if( fileName.IsEmpty() )
  {
    return;
  }
  

  
  vtkMetaImageData* metaimage = 0;
  
  if (vtkMetaImageData::CanReadFile(fileName.char_str()))
  {
    metaimage = vtkMetaImageData::New();
    try
    {
	metaimage->Read (fileName.char_str());
    }
    catch (vtkErrorCode::ErrorIds)
    {
      std::cerr << "Cannot read file " << fileName << std::endl;
      wxMessageDialog* myDialog = new wxMessageDialog(this, wxT("Error while reading file.\n(check the log window for more details)"),
						      wxT ("Error"),
						      wxOK|wxICON_ERROR);
      myDialog->ShowModal();
      myDialog->Destroy();
      metaimage->Delete();
      return;
    }
  }
  
  if( !metaimage )
  {
    return;
  }


  m_View1->GetViewImage2D()->SetImage ( metaimage->GetImageData() );
  m_View2->GetViewImage2D()->SetImage ( metaimage->GetImageData() );
  m_View3->GetViewImage2D()->SetImage ( metaimage->GetImageData() );

  
  metaimage->Delete();
    

  double* range = m_View1->GetViewImage2D()->GetImage()->GetScalarRange();

  m_View1->GetZSlider()     ->SetRange ( m_View1->GetViewImage2D()->GetWholeZMin(), m_View1->GetViewImage2D()->GetWholeZMax());
  m_View1->GetWindowSlider()->SetRange ((int)range[0], (int)range[1]);
  m_View1->GetLevelSlider() ->SetRange ((int)range[0], (int)range[1]);

  m_View2->GetZSlider()     ->SetRange ( m_View2->GetViewImage2D()->GetWholeZMin(), m_View2->GetViewImage2D()->GetWholeZMax());
  m_View2->GetWindowSlider()->SetRange ((int)range[0], (int)range[1]);
  m_View2->GetLevelSlider() ->SetRange ((int)range[0], (int)range[1]);

  m_View3->GetZSlider()     ->SetRange ( m_View3->GetViewImage2D()->GetWholeZMin(), m_View3->GetViewImage2D()->GetWholeZMax());
  m_View3->GetWindowSlider()->SetRange ((int)range[0], (int)range[1]);
  m_View3->GetLevelSlider() ->SetRange ((int)range[0], (int)range[1]);
  

  m_View1->GetViewImage2D()->SyncResetCurrentPoint();

  m_View1->GetViewImage2D()->ResetZoom();
  m_View2->GetViewImage2D()->ResetZoom();
  m_View3->GetViewImage2D()->ResetZoom();
  
  m_View1->GetViewImage2D()->SyncResetWindowLevel();
}
