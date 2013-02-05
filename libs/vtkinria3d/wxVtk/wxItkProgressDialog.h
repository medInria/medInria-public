/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxItkProgressDialog.h 609 2008-01-14 09:21:24Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2008-01-14 10:21:24 +0100 (Mon, 14 Jan 2008) $
Version:   $Revision: 609 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _wx_ItkProgressDialog_h_
#define _wx_ItkProgressDialog_h_

// For compilers that supports precompilation , includes  wx/wx.h
#include  "wx/wxprec.h"

#ifndef WX_PRECOMP
#include  "wx/wx.h"
#endif

#include <wx/progdlg2.h>

#include <itkProcessObject.h>

#include <vtkINRIA3DConfigure.h>

#include "wxItkProgressDialogCallback.h"

class WX_VTK_EXPORT wxItkProgressDialog: public wxProgressDialog2
{

 public:
  
  wxItkProgressDialog(const wxString& title,
                      const wxString& message,
                      int maximum = 100,
                      wxWindow * parent = NULL,
                      int style = wxPD_AUTO_HIDE | wxPD_APP_MODAL);
  virtual ~wxItkProgressDialog(){}
  
  
  void SetItkObjectToWatch (itk::ProcessObject::Pointer);

  wxItkProgressDialogCallback::Pointer GetCallback (void) const
  { return m_Callback; }

  
 protected:
  

 private:

  wxItkProgressDialogCallback::Pointer m_Callback;

};


#endif
