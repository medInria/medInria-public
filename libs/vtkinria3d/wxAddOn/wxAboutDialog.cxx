/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxAboutDialog.cxx 477 2007-11-20 17:46:10Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2007-11-20 18:46:10 +0100 (Tue, 20 Nov 2007) $
Version:   $Revision: 477 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "wxAboutDialog.h"

wxAboutDialog::wxAboutDialog (wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
  : wxDialog(parent, id, title, pos, size, style)
{


  wxBitmap* bitmap = new wxBitmap(500,100);
  m_Bitmap = new wxStaticBitmap(this, -1, *bitmap );
  
  m_AboutText = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_CENTRE);
  wxFont myFont;
  myFont.SetWeight(wxFONTWEIGHT_BOLD);
  wxTextAttr myAttr;
  myAttr.SetFont(myFont);
  
  m_AboutText->SetStyle(-1,-1,myAttr);
  m_AboutText->AppendText(wxT("About this software..."));
  
  this->SetProperties();
  this->DoLayout();
    
}

void wxAboutDialog::SetProperties()
{

  this->SetBackgroundColour( wxColour( 250,250,250 ) );
  this->SetTitle(wxT("About"));
}

void wxAboutDialog::DoLayout()
{
  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add (m_Bitmap, 0, wxFIXED_MINSIZE, 0);
  sizer->Add (m_AboutText, 1, wxALL|wxEXPAND, 0);

  this->SetAutoLayout (true);
  this->SetSizerAndFit (sizer);
  this->Layout();
}

void wxAboutDialog::SetBitmapLogo(const wxBitmap& logo)
{
  m_Bitmap->SetBitmap(logo);
  this->DoLayout();
  this->Refresh();
}

void wxAboutDialog::SetAboutText(const wxString& license)
{
  m_AboutText->Clear();
  m_AboutText->AppendText(license);
}
