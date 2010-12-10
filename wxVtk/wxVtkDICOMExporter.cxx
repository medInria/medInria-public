/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxVtkDICOMExporter.cxx 1 2007-11-20 17:46:10Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2007-11-20 18:46:10 +0100 (Tue, 20 Nov 2007) $
Version:   $Revision: 1 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "wxVtkDICOMExporter.h"

//#include <pixmaps/fileopen.xpm>
#include <pixmaps/cd.xpm>


wxVtkDICOMExporter::wxVtkDICOMExporter (wxWindow* parent, int id,
				const wxString& title,
				const wxPoint& pos,
				const wxSize& size,
				long style)
  : wxDialog(parent, id, title, pos, size, style)
{


  this->GDCMExporter = itk::GDCMExporter::New();
  

  this->CheckboxSelectAll        = new wxCheckBox (this, CHECKBOX_SELECTALL, wxT("Select All"));
  this->ButtonEditDICOMInfo      = new wxBitmapButton (this, BUTTON_EDIT_DICOM, wxBitmap (cd));
  this->ButtonEditDICOMInfo->SetToolTip (wxT ("Edit the DICOM information"));

  //this->ButtonPropagateDICOMInfo = new wxBitmapButton (this, BUTTON_PROPAGATE_DICOM, wxBitmap (cd));
  //this->ButtonPropagateDICOMInfo->SetToolTip (wxT ("Propagate the DICOM information to the other images"));

  this->ButtonCANCEL             = new wxButton (this, BUTTON_CANCEL, wxT("Cancel"), wxDefaultPosition, wxSize (80,30));
  this->ButtonOK                 = new wxButton (this, BUTTON_OK,     wxT("Write"), wxDefaultPosition, wxSize (80,30));

#if wxCHECK_VERSION(2, 9, 0)
  this->CheckListBox = new wxCheckListBox (this, CHECKLISTBOX_VOLUMES, wxDefaultPosition, wxDefaultSize, 0, NULL,
					   wxLB_EXTENDED);
#else
  this->CheckListBox = new wxCheckListBox (this, CHECKLISTBOX_VOLUMES, wxDefaultPosition, wxDefaultSize, 0,
					   wxLB_EXTENDED);
#endif
  
  this->SetProperties();
  this->DoLayout();

  
}

wxVtkDICOMExporter::~wxVtkDICOMExporter()
{
  
}

void wxVtkDICOMExporter::SetProperties()
{

}


void wxVtkDICOMExporter::DoLayout()
{
  wxBoxSizer* sizermain        = new wxBoxSizer(wxVERTICAL);

  wxBoxSizer* sizertree        = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* sizerbuttons     = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* sizerOK          = new wxBoxSizer(wxHORIZONTAL);
  
  sizertree      ->Add (this->CheckListBox, 1, wxALL|wxEXPAND, 5);

  sizerbuttons   ->Add (this->ButtonEditDICOMInfo,      0, wxALL|wxEXPAND, 5);
  //sizerbuttons   ->Add (this->ButtonPropagateDICOMInfo, 0, wxALL|wxEXPAND, 5);
  
  sizerOK        ->Add (this->ButtonCANCEL, 0, wxALL|wxEXPAND, 5);
  sizerOK        ->Add (this->ButtonOK,     0, wxALL|wxEXPAND, 5);
  
  sizermain       ->Add (this->CheckboxSelectAll, 0, wxALL|wxEXPAND, 0);
  sizermain       ->Add (sizertree,     1, wxALL|wxEXPAND, 0);
  sizermain       ->Add (sizerbuttons,  0, wxCENTER, 0);
  sizermain       ->Add (sizerOK,       0, wxALIGN_RIGHT, 0);
  

  this->SetAutoLayout(true);
  this->SetSizerAndFit (sizermain);

  this->SetSize (wxSize (400,300));
  
  this->Layout();
  
}


BEGIN_EVENT_TABLE (wxVtkDICOMExporter, wxDialog)
  EVT_CHECKBOX(CHECKBOX_SELECTALL,    wxVtkDICOMExporter::OnCheckboxSelectAll)
  EVT_BUTTON (BUTTON_EDIT_DICOM,      wxVtkDICOMExporter::OnEditDICOMInfo)
  EVT_BUTTON (BUTTON_PROPAGATE_DICOM, wxVtkDICOMExporter::OnPropagateDICOMInfo)
  EVT_BUTTON (BUTTON_CANCEL,          wxVtkDICOMExporter::OnButtonCANCEL)
  EVT_BUTTON (BUTTON_OK,              wxVtkDICOMExporter::OnButtonOK)
END_EVENT_TABLE()

  
void wxVtkDICOMExporter::OnButtonCANCEL (wxCommandEvent &event)
{
  EndModal(wxID_CANCEL); 
}

void wxVtkDICOMExporter::OnButtonOK (wxCommandEvent &event)
{

  unsigned int number = this->CheckListBox->GetCount();

  if (!number)
    return;
  
  wxDirDialog* myDirDialog = new wxDirDialog(this, wxT ("Choose the output DICOM directory"),
                                             wxT(""),
                                             wxDD_DIR_MUST_EXIST|wxDD_DEFAULT_STYLE|wxDD_CHANGE_DIR, wxDefaultPosition);
  
  int OK = myDirDialog->ShowModal();
  if( OK!=wxID_OK )
  {
    myDirDialog->Destroy();
    return;
  }  
    
  wxString directory = myDirDialog->GetPath();

  this->GDCMExporter->SetOutputDirectory (directory.char_str());

  myDirDialog->Destroy();


  this->GDCMExporter->WriteAll();

  
  EndModal(wxID_OK);

  
}


void wxVtkDICOMExporter::AddVolume (const char* name, wxVtkDICOMExporter::ImageType::Pointer image)
{
  // unsigned int id = this->CheckListBox->GetCount();
  wxString s_name( name, wxConvUTF8 );
  this->CheckListBox->Append ( s_name );
  this->GDCMExporter->AddVolume(image);
}



void wxVtkDICOMExporter::OnCheckboxSelectAll (wxCommandEvent &event)
{
  for (unsigned int i=0; i<this->CheckListBox->GetCount(); i++)
  {
    this->CheckListBox->Check (i, this->CheckboxSelectAll->IsChecked());
  }
}
