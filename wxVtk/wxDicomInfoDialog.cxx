/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxDicomInfoDialog.cxx 1 2007-11-20 17:46:10Z ntoussaint $
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
#include "wxDicomInfoDialog.h"

//#include <pixmaps/fileopen.xpm>
//#include <pixmaps/cd.xpm>


wxDicomInfoDialog::wxDicomInfoDialog (wxWindow* parent, int id,
				      const wxString& title,
				      const wxPoint& pos,
				      const wxSize& size,
				      long style)
  : wxDialog(parent, id, title, pos, size, style)
{
  

  this->ButtonCANCEL             = new wxButton (this, BUTTON_CANCEL, wxT("Cancel"), wxDefaultPosition, wxSize (80,30));
  this->ButtonOK                 = new wxButton (this, BUTTON_OK,     wxT("OK"), wxDefaultPosition, wxSize (80,30));

  this->ListBox = new wxListCtrl (this, LISTBOX_FLAGS, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
  
  this->SetProperties();
  this->DoLayout();

  
}

wxDicomInfoDialog::~wxDicomInfoDialog()
{
  
}

void wxDicomInfoDialog::SetProperties()
{

  wxListItem itemCol1;
  itemCol1.SetText(wxT("Description"));
  itemCol1.SetImage(-1);
  this->ListBox->InsertColumn(0, itemCol1);
  this->ListBox->SetColumnWidth( 0, wxLIST_AUTOSIZE );

  wxListItem itemCol2;
  itemCol2.SetText(wxT("Type"));
  itemCol2.SetImage(-1);
  this->ListBox->InsertColumn(1, itemCol2);
  this->ListBox->SetColumnWidth( 1, wxLIST_AUTOSIZE );
  
  wxListItem itemCol3;
  itemCol3.SetText(wxT("Value"));
  itemCol3.SetImage(-1);
  this->ListBox->InsertColumn(2, itemCol3);
  this->ListBox->SetColumnWidth( 2, wxLIST_AUTOSIZE );
  
}


void wxDicomInfoDialog::DoLayout()
{
  wxBoxSizer* sizermain        = new wxBoxSizer(wxVERTICAL);

  wxBoxSizer* sizertree        = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* sizerOK          = new wxBoxSizer(wxHORIZONTAL);
  
  sizertree ->Add (this->ListBox, 1, wxALL|wxEXPAND, 5);
  
  sizerOK ->Add (this->ButtonCANCEL, 0, wxALL|wxEXPAND, 5);
  sizerOK ->Add (this->ButtonOK,     0, wxALL|wxEXPAND, 5);
  
  sizermain ->Add (sizertree,     1, wxALL|wxEXPAND, 0);
  sizermain ->Add (sizerOK,       0, wxALIGN_RIGHT, 0);
  

  this->SetAutoLayout(true);
  this->SetSizerAndFit (sizermain);

  this->SetSize (wxSize (400,300));
  
  this->Layout();
  
}


BEGIN_EVENT_TABLE (wxDicomInfoDialog, wxDialog)
  EVT_BUTTON (BUTTON_CANCEL,          wxDicomInfoDialog::OnButtonCANCEL)
  EVT_BUTTON (BUTTON_OK,              wxDicomInfoDialog::OnButtonOK)
END_EVENT_TABLE()

  
void wxDicomInfoDialog::OnButtonCANCEL (wxCommandEvent &event)
{
  EndModal(wxID_CANCEL); 
}

void wxDicomInfoDialog::OnButtonOK (wxCommandEvent &event)
{

  
  EndModal(wxID_OK);

  
}


void wxDicomInfoDialog::SetDicomTagList (std::vector<itk::DicomTag> list)
{

  this->ListBox->DeleteAllItems();

  int id = 0;
  
  for (unsigned int i=0; i<list.size(); i++)
  {
    
    wxListItem item;
    item.SetId (i);
    item.SetColumn (0);
    item.SetText(wxString( list[i].name.c_str(), wxConvUTF8));
    item.SetImage(-1);
    this->ListBox->InsertItem(item);
    id++;
  }


  for (unsigned int i=0; i<list.size(); i++)
  {
      
    wxListItem item2;
    item2.SetId (i);
    item2.SetColumn (1);
    item2.SetText(wxString( list[i].NotUsed.c_str(), wxConvUTF8));
    item2.SetImage(-1);
    this->ListBox->SetItem(item2);
    id++;

    wxListItem item3;
    item3.SetId (i);
    item3.SetColumn (2);
    item3.SetText(wxString(list[i].value.c_str(), wxConvUTF8));
    item3.SetImage(-1);
    this->ListBox->SetItem(item3);
    id++;
  }
  
  this->ListBox->SetColumnWidth( 0, wxLIST_AUTOSIZE );
  this->ListBox->SetColumnWidth( 1, wxLIST_AUTOSIZE );
  this->ListBox->SetColumnWidth( 2, wxLIST_AUTOSIZE );

  
}


