/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id:  1182 2009-06-30 09:19:05Z  $
Language:  C++
Author:    $Author:  $
Date:      $Date: 2009-06-30 11:19:05 +0200 (mar, 30 jun 2009) $
Version:   $Revision: 1182 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "wxVtkLandmarkWindow.h"

#include "vtkLandmarkManager.h"
#include <vtkLandmarkManagerCallback.h>
#include <vtkCollection.h>
#include <vtkViewImage2D.h>

#include <vtkRenderWindow.h>

#include <wx/bmpbuttnff.h>
#include <pixmaps/fileopen.xpm>
#include <pixmaps/filesave.xpm>
#include <pixmaps/add.xpm>
#include <pixmaps/remove.xpm>




void wxVtkLandmarkWindowCallback::Execute ( vtkObject *caller, unsigned long event, void* )
{

  if (event == vtkLandmarkManagerCallback::LandmarkAddedEvent)
  {
    if (!this->LandmarkWindow )
    {
      return;
    }

    this->LandmarkWindow->AddLandmarkToList ( this->LandmarkWindow->GetLandmarkManager()->GetLastLandmark() );
    
  }
  else if (event == vtkLandmarkManagerCallback::LandmarkRemovingEvent)
  {
    this->LandmarkWindow->RemoveLandmarkFromList ( this->LandmarkWindow->GetLandmarkManager()->GetLandmarkBeingRemoved() );
  }
  else if (event == vtkLandmarkManagerCallback::LandmarkPickedEvent)
  {
    this->LandmarkWindow->HighlightLandmark ( this->LandmarkWindow->GetLandmarkManager()->GetLastPickedLandmark() );
  }
  
  
}





wxVtkLandmarkWindow::wxVtkLandmarkWindow (wxWindow* parent, int id,
                                          const wxPoint& pos,
                                          const wxSize& size,
                                          long style,
                                          const wxString& name)
  : wxPanel ( parent, id, pos, size, style, name )
{

  this->LandmarkManager = vtkLandmarkManager::New();

  this->LandmarkWindowCallback = wxVtkLandmarkWindowCallback::New();
  this->LandmarkWindowCallback->SetLandmarkWindow (this);
  this->LandmarkManager->AddObserver (vtkLandmarkManagerCallback::LandmarkAddedEvent,    this->LandmarkWindowCallback, -1);
  this->LandmarkManager->AddObserver (vtkLandmarkManagerCallback::LandmarkRemovingEvent, this->LandmarkWindowCallback, -1);
  this->LandmarkManager->AddObserver (vtkLandmarkManagerCallback::LandmarkPickedEvent, this->LandmarkWindowCallback, -1);
                                       


  this->ButtonOpenLandmarks = new wxFFBitmapButton (this, BUTTON_OPEN_LANDMARKS, wxBitmap(fileopen));
  this->ButtonSaveLandmarks = new wxFFBitmapButton (this, BUTTON_SAVE_LANDMARKS, wxBitmap(filesave));

  
  this->ButtonAddLandmark = new wxFFBitmapButton (this, BUTTON_ADD_LANDMARK, wxBitmap(add_xpm));
  this->ButtonAddLandmark->SetToolTip (wxT ("Insert a landmark to current position"));
  this->ButtonRemoveLandmark = new wxFFBitmapButton (this, BUTTON_REMOVE_LANDMARK, wxBitmap(remove_xpm));
  this->ButtonRemoveLandmark->SetToolTip (wxT ("Remove selected landmark"));
  this->ButtonRemoveAllLandmarks = new wxButton (this, BUTTON_REMOVE_ALL_LANDMARKS);
  this->ButtonRemoveAllLandmarks->SetLabel(wxT("Clear all"));
  this->ButtonRemoveLandmark->SetToolTip (wxT ("Remove all landmarks"));
  this->ButtonUpdate = new wxButton (this, BUTTON_UPDATE);
  this->ButtonUpdate->SetLabel (wxT ("Update") );
  this->ButtonUpdate->SetToolTip (wxT ("Update the mesh"));

  this->LandmarkList = new wxChoice (this, LB_LANDMARK,
                                     wxDefaultPosition, wxDefaultSize,
                                     0, NULL, wxLB_SINGLE);

  this->LandmarkFlagText   = new wxTextCtrl (this, TE_LANDMARK, wxT (""), wxDefaultPosition, wxDefaultSize,
                                             wxTE_PROCESS_ENTER);
  this->LandmarkFlagButton = new wxSpinCtrl (this, SB_LANDMARK);
  
  
  this->DoLayout();
  this->SetProperties(); 
}



BEGIN_EVENT_TABLE (wxVtkLandmarkWindow, wxPanel)
  EVT_TEXT_ENTER (TE_LANDMARK, wxVtkLandmarkWindow::OnLandmarkFlagText)
  EVT_SPINCTRL   (SB_LANDMARK, wxVtkLandmarkWindow::OnLandmarkFlagButton)
  EVT_BUTTON (BUTTON_OPEN_LANDMARKS,   wxVtkLandmarkWindow::OnButtonOpenLandmarks)
  EVT_BUTTON (BUTTON_SAVE_LANDMARKS,   wxVtkLandmarkWindow::OnButtonSaveLandmarks)
  EVT_BUTTON (BUTTON_ADD_LANDMARK,     wxVtkLandmarkWindow::OnButtonAddLandmark)
  EVT_BUTTON (BUTTON_REMOVE_LANDMARK,  wxVtkLandmarkWindow::OnButtonRemoveLandmark)
  EVT_BUTTON (BUTTON_REMOVE_ALL_LANDMARKS,  wxVtkLandmarkWindow::OnButtonRemoveAllLandmarks)
  EVT_BUTTON (BUTTON_UPDATE,           wxVtkLandmarkWindow::OnButtonUpdate)
  EVT_CHOICE (LB_LANDMARK,             wxVtkLandmarkWindow::OnLandmarkChoice)
END_EVENT_TABLE()




wxVtkLandmarkWindow::~wxVtkLandmarkWindow()
{
  this->LandmarkManager->RemoveObserver ( this->LandmarkWindowCallback );

  this->LandmarkWindowCallback->Delete();
  this->LandmarkManager->Delete();
}



void wxVtkLandmarkWindow::SetProperties()
{
  this->LandmarkList->Append (wxT ("Landmarks..."));
  this->LandmarkList->SetSelection (0);

  this->LandmarkFlagButton->SetRange (0, 255);
  this->LandmarkFlagButton->SetValue (0);
  //this->LandmarkFlagButton->SetMinSize (wxSize (32,-1));
}


void wxVtkLandmarkWindow::DoLayout()
{

  wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
  
  wxBoxSizer* sizer1 = new wxBoxSizer(wxHORIZONTAL);
  sizer1->AddStretchSpacer();
  sizer1->Add (this->ButtonOpenLandmarks, 0, wxALL|wxFIXED_MINSIZE, 5);
  sizer1->Add (this->ButtonSaveLandmarks, 0, wxALL|wxFIXED_MINSIZE, 5);
  sizer1->AddStretchSpacer();

  wxBoxSizer* sizer2 = new wxBoxSizer(wxHORIZONTAL);
  sizer2->AddStretchSpacer();
  sizer2->Add (this->ButtonAddLandmark, 0, wxALL|wxFIXED_MINSIZE, 5);
  sizer2->Add (this->ButtonRemoveLandmark, 0, wxALL|wxFIXED_MINSIZE, 5);
  sizer2->Add (this->ButtonRemoveAllLandmarks, 0, wxALL|wxFIXED_MINSIZE, 5);
  sizer2->AddStretchSpacer();

  wxBoxSizer* sizer3 = new wxBoxSizer(wxHORIZONTAL);
  sizer3->Add (this->LandmarkFlagText,   1, wxALL, 5);
  sizer3->Add (this->LandmarkFlagButton, 0, wxALL|wxFIXED_MINSIZE, 5);


  mainSizer->Add (sizer1, 0, wxALL|wxEXPAND, 0);
  mainSizer->Add (this->LandmarkList, 0, wxALL|wxEXPAND, 5);
  mainSizer->Add (sizer3, 0, wxALL|wxEXPAND, 0);
  mainSizer->Add (this->ButtonUpdate, 0, wxALL|wxEXPAND, 5);
  mainSizer->Add (sizer2, 0, wxALL|wxEXPAND, 0);
  
  
  this->SetAutoLayout(true);
  this->SetSizerAndFit(mainSizer);
  this->Layout();
}



void wxVtkLandmarkWindow::OnButtonOpenLandmarks(wxCommandEvent &event)
{

  wxFileDialog* myFileDialog = new wxFileDialog(this, wxT ("Open a landmark file"),
                                                wxT(""), wxT(""),
                                                wxT ("Landmark File (*.lms)|*.lms|")
                                                wxT ("All (*.*)|*.*"),
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

  
  this->LandmarkManager->Reset();
  this->LandmarkManager->ReadFile ( fileName.char_str() );
  this->UpdateListBox();
  
  this->RenderViews();
  
}


void wxVtkLandmarkWindow::UpdateListBox()
{
  this->LandmarkList->Clear();
  this->LandmarkList->Append (wxT ("Landmarks..."));

  for (unsigned int i=0; i<this->LandmarkManager->GetNumberOfLandmarks(); i++)
  {
    this->AddLandmarkToList ( this->LandmarkManager->GetLandmark(i) );
  }


  this->LandmarkList->SetSelection (0);
  
}



void wxVtkLandmarkWindow::OnButtonSaveLandmarks(wxCommandEvent &event)
{

  wxFileDialog* myFileDialog = new wxFileDialog(this, wxT ("Save landmarks to..."),
                                                wxT(""), wxT(""),
                                                wxT ("Landmark File (*.lms)|*.lms|")
                                                wxT ("All (*.*)|*.*"),
                                                wxFD_SAVE|wxFD_CHANGE_DIR|wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
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

  this->LandmarkManager->WriteFile ( fileName.char_str() );
  
}


void wxVtkLandmarkWindow::OnButtonAddLandmark(wxCommandEvent &event)
{
  this->LandmarkManager->CreateAndAddLandmark();
  //this->LandmarkManager->InvokeEvent ( vtkLandmarkManagerCallback::LandmarkAddedEvent );
  this->RenderViews();
}


void wxVtkLandmarkWindow::OnButtonRemoveLandmark(wxCommandEvent &event)
{

  int selection = this->LandmarkList->GetSelection()-1;
  if( selection <0 || selection >= (int)this->LandmarkManager->GetNumberOfLandmarks() )
  {
    return;
  }

  this->LandmarkManager->RemoveLandmark ( this->LandmarkManager->GetLandmark (selection) );

  this->RenderViews();
}


void wxVtkLandmarkWindow::OnButtonRemoveAllLandmarks(wxCommandEvent &event)
{
  this->LandmarkList->Clear();
  this->LandmarkList->Append (wxT ("Landmarks..."));
  this->LandmarkManager->RemoveAllLandmarks();
  this->RenderViews();
}



void wxVtkLandmarkWindow::OnButtonUpdate(wxCommandEvent &event)
{
  this->ProcessLandmarks();
}


void wxVtkLandmarkWindow::ProcessLandmarks()
{
  std::cout << "wxVtkLandmarkWindow::ProcessLandmarks() not implemented." << std::endl;
}


void wxVtkLandmarkWindow::OnLandmarkFlagText (wxCommandEvent& event)
{

  int selection = this->LandmarkList->GetSelection()-1;
  if( selection <0 || selection >= (int)this->LandmarkManager->GetNumberOfLandmarks() )
  {
    return;
  }

  wxString key = event.GetString();
  int      val = this->LandmarkFlagButton->GetValue();

  this->LandmarkManager->GetLandmark (selection)->SetFlag (key.char_str(), val);  
  
}


void wxVtkLandmarkWindow::OnLandmarkFlagButton (wxSpinEvent& event)
{

  int selection = this->LandmarkList->GetSelection()-1;
  
  if( selection <0 || selection >= (int)this->LandmarkManager->GetNumberOfLandmarks() )
  {
    return;
  }

  int      val = event.GetPosition();
  wxString key = this->LandmarkFlagText->GetValue();

  this->LandmarkManager->GetLandmark (selection)->SetFlag (key.char_str(), val);
  
}




void wxVtkLandmarkWindow::OnLandmarkChoice (wxCommandEvent& event)
{
  int selection = event.GetSelection()-1;

  if( selection <0 || selection >= (int)this->LandmarkManager->GetNumberOfLandmarks() )
  {
    return;
  }
  
  std::vector<std::string> flags = this->LandmarkManager->GetLandmark(selection)->GetFlagKeys ();
  
  if( flags.size() )
  {    
    double val = this->LandmarkManager->GetLandmark(selection)->GetFlag ( flags[0].c_str() );
    this->LandmarkFlagButton->SetValue ( int(val) );
    this->LandmarkFlagText->SetValue ( wxString( flags[0].c_str(), wxConvUTF8) );
  }


  this->LandmarkManager->SelectLandmark ( this->LandmarkManager->GetLandmark(selection) );
  this->RenderViews();
  
}




void wxVtkLandmarkWindow::AddLandmarkToList (vtkLandmark* landmark)
{
  if( !landmark )
  {
    return;
  }
  
  wxString text = wxString ( landmark->GetName(), wxConvUTF8 );
  this->LandmarkList->Append (text);
}



void wxVtkLandmarkWindow::RemoveLandmarkFromList (vtkLandmark* landmark)
{
  /**
     Look for the landmark in the list by its name. If several landmarks have the
     same name, only the first occurence will be removed.
   */
  if( !landmark )
  {
    return;
  }
  
  wxString text_to_test = wxString (landmark->GetName(), wxConvUTF8);
  for( unsigned int i=0; i<this->LandmarkList->GetCount(); i++)
  {
    if( this->LandmarkList->GetString (i) == text_to_test )
    {
      this->LandmarkList->Delete(i);
      return;
    }
  }
}



void wxVtkLandmarkWindow::RenderViews()
{
  for( int i=0; i<this->LandmarkManager->GetViewList()->GetNumberOfItems(); i++)
  {
    vtkViewImage* view = vtkViewImage::SafeDownCast ( this->LandmarkManager->GetViewList()->GetItemAsObject(i) );
    if( !view->GetRenderWindow()->GetNeverRendered() )
    {
      view->Render();
    }
  }
}



void wxVtkLandmarkWindow::HighlightLandmark (vtkLandmark* landmark)
{
  if( !landmark )
  {
    return;
  }
  
  wxString text_to_test = wxString (landmark->GetName(), wxConvUTF8);
  for( unsigned int i=0; i<this->LandmarkList->GetCount(); i++)
  {
    if( this->LandmarkList->GetString (i) == text_to_test )
    {
      this->LandmarkList->SetSelection(i);
      wxCommandEvent junk;
      junk.SetInt (i);
      this->OnLandmarkChoice (junk);
      return;
    }
  }
}
