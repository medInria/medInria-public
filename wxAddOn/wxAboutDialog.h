/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxAboutDialog.h 608 2008-01-14 08:21:23Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2008-01-14 09:21:23 +0100 (Mon, 14 Jan 2008) $
Version:   $Revision: 608 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _wx_AboutDialog_h_
#define _wx_AboutDialog_h_

// For compilers that supports precompilation , includes  wx/wx.h
#include  "wx/wxprec.h"

#ifndef WX_PRECOMP
#include  "wx/wx.h"
#endif

#include "vtkINRIA3DConfigure.h"

class WX_ADDON_EXPORT wxAboutDialog : public wxDialog
{

 public:
  wxAboutDialog(wxWindow*, int, const wxString&, const wxPoint&, const wxSize&, long);
  virtual ~wxAboutDialog(){};
  
  void SetBitmapLogo(const wxBitmap&);
  void SetAboutText(const wxString&);
  
 protected:
  void SetProperties();
  void DoLayout();
  
 private:
  wxStaticBitmap* m_Bitmap;
  wxTextCtrl*     m_AboutText;
  
};

#endif
