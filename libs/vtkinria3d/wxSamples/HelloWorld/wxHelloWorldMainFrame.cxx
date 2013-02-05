/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxHelloWorldMainFrame.cxx 1350 2009-11-19 15:16:30Z ntoussaint $
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
#include "wxHelloWorldMainFrame.h"

#include "fileopen.xpm"

#include "wxVTKRenderWindowInteractor.h"
#include <vtkViewImage2D.h>
#include <vtkViewImage3D.h>

#include <vtkRenderer.h>

wxHelloWorldMainFrame::wxHelloWorldMainFrame (wxWindow* parent, int id,
                                              const wxString& title,
                                              const wxPoint& pos,
                                              const wxSize& size,
                                              long style )
  : wxFrame ( parent, id, title, pos, size, style )
{
  
  /**
     Here we simply instanciate the GUI elements.
   */

  long viewStyle = wxWANTS_CHARS | wxNO_FULL_REPAINT_ON_RESIZE;

  m_VtkView1 = new wxVTKRenderWindowInteractor (this , wxID_ANY,
                                                wxDefaultPosition,
                                                wxDefaultSize,
                                                viewStyle,
                                                wxT (""));

  m_VtkView2 = new wxVTKRenderWindowInteractor (this , wxID_ANY,
                                                wxDefaultPosition,
                                                wxDefaultSize,
                                                viewStyle,
                                                wxT (""));

  m_VtkView3 = new wxVTKRenderWindowInteractor (this , wxID_ANY,
                                                wxDefaultPosition,
                                                wxDefaultSize,
                                                viewStyle,
                                                wxT (""));

  m_VtkView4 = new wxVTKRenderWindowInteractor (this , wxID_ANY,
                                                wxDefaultPosition,
                                                wxDefaultSize,
                                                viewStyle,
                                                wxT (""));
  

  m_View1 = vtkViewImage2D::New();
  m_View2 = vtkViewImage2D::New();
  m_View3 = vtkViewImage2D::New();
  m_View4 = vtkViewImage3D::New();

  
  vtkRenderer* rend1 = vtkRenderer::New();
  vtkRenderer* rend2 = vtkRenderer::New();
  vtkRenderer* rend3 = vtkRenderer::New();
  vtkRenderer* rend4 = vtkRenderer::New();

  m_VtkView1->GetRenderWindow()->AddRenderer (rend1);
  m_VtkView2->GetRenderWindow()->AddRenderer (rend2);
  m_VtkView3->GetRenderWindow()->AddRenderer (rend3);
  m_VtkView4->GetRenderWindow()->AddRenderer (rend4);


  m_View1->SetRenderWindow (m_VtkView1->GetRenderWindow());
  m_View2->SetRenderWindow (m_VtkView2->GetRenderWindow());
  m_View3->SetRenderWindow (m_VtkView3->GetRenderWindow());
  m_View4->SetRenderWindow (m_VtkView4->GetRenderWindow());

  
  m_View1->SetRenderWindowInteractor (m_VtkView1);
  m_View2->SetRenderWindowInteractor (m_VtkView2);
  m_View3->SetRenderWindowInteractor (m_VtkView3);
  m_View4->SetRenderWindowInteractor (m_VtkView4);

  
  m_View1->SetRenderer (rend1);
  m_View2->SetRenderer (rend2);
  m_View3->SetRenderer (rend3);
  m_View4->SetRenderer (rend4);


  rend1->Delete();
  rend2->Delete();
  rend3->Delete();
  rend4->Delete();

    
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

  
wxHelloWorldMainFrame::~wxHelloWorldMainFrame()
{
  m_View1->Detach();
  m_View2->Detach();
  m_View3->Detach();
  m_View4->Detach();
  
  // the wxVtkViews are registered in the vtk framework. One needs to call Delete().
  m_View1->Delete();
  m_View2->Delete();
  m_View3->Delete();
  m_View4->Delete();

  
  m_VtkView1->Delete();
  m_VtkView2->Delete();
  m_VtkView3->Delete();
  m_VtkView4->Delete();
  
}


/**
   Table event: we associate to an id (here: OPEN_BUTTON) a function of the class
   (here: OnOpenButton). The macro EVT_TOOL will tell wxWidgets that the toolbar
   button with id OPEN_BUTTON is pressed, then call OnOpenButton function.
 */

BEGIN_EVENT_TABLE (wxHelloWorldMainFrame, wxFrame)
  EVT_TOOL (OPEN_BUTTON, wxHelloWorldMainFrame::OnOpenButton)
END_EVENT_TABLE()




void wxHelloWorldMainFrame::SetProperties()
{

  /**
     In this function, we specify some properties of our views, like the type
     of interaction, the 3D rendering mode, etc.
   */
  m_View1->SetInteractionStyle (vtkViewImage2D::SELECT_INTERACTION);
  m_View2->SetInteractionStyle (vtkViewImage2D::SELECT_INTERACTION);
  m_View3->SetInteractionStyle (vtkViewImage2D::SELECT_INTERACTION);

  m_View1->SetOrientation (vtkViewImage2D::AXIAL_ID);
  m_View2->SetOrientation (vtkViewImage2D::CORONAL_ID);
  m_View3->SetOrientation (vtkViewImage2D::SAGITTAL_ID);

  m_View1->SetBackgroundColor (0.0,0.0,0.0);
  m_View2->SetBackgroundColor (0.0,0.0,0.0);
  m_View3->SetBackgroundColor (0.0,0.0,0.0);
  

  double color[3] = {0.0,0.0,0.0};
  m_View4->SetTextColor (color);
  m_View4->SetRenderingModeToPlanar();
  
  /**
     Here we link the views, so that each action on view1 is copied to
     view2, which sends it to view 3, etc. This results in synchronized
     views.
  */
  m_View1->AddChild (m_View2);
  m_View1->AddChild (m_View4);
  m_View2->AddChild (m_View3);
  m_View3->AddChild (m_View1); 
  
}



void wxHelloWorldMainFrame::DoLayout()
{
  /**
     This is the trickiest part of building a user interface. Here, we tell
     wxWidgets how to place/resize the widgets. In this simple example, we
     create a GridSizer which is nothing more than a table with 2 rows and
     2 colums. Then, we place a view in each cell.
   */

  wxGridSizer* sizer = new wxGridSizer(2,2,5,5);
  sizer->Add (m_VtkView1, 1, wxALL|wxEXPAND, 0);
  sizer->Add (m_VtkView2, 1, wxALL|wxEXPAND, 0);
  sizer->Add (m_VtkView3, 1, wxALL|wxEXPAND, 0);
  sizer->Add (m_VtkView4, 1, wxALL|wxEXPAND, 0);
  
  this->SetSizerAndFit (sizer);
  this->SetAutoLayout (true);
  this->SetSize (500,500);
  this->Layout();
}


/**
   For a better clarity, we will overload this function in a class that
   derives from this class, whose purpose is only to build the window.
 */
void wxHelloWorldMainFrame::OnOpenButton (wxCommandEvent& event)
{
  std::cout << "wxHelloWorldMainFrame::OnOpenButton not implemented yet." << std::endl;
}
