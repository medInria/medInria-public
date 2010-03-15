/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxItkProgressDialog.cxx 477 2007-11-20 17:46:10Z filus $
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
#include "wxItkProgressDialog.h"

wxItkProgressDialog::wxItkProgressDialog(const wxString& title,
                                         const wxString& message,
                                         int maximum, wxWindow * parent, int style)
  : wxProgressDialog2(title, message, maximum, parent, style)
{
  m_Callback = wxItkProgressDialogCallback::New();
  m_Callback->SetProgressDialog (this);
}

void wxItkProgressDialog::SetItkObjectToWatch (itk::ProcessObject::Pointer object)
{
  if( object.IsNull() )
    return;

  object->AddObserver (itk::ProgressEvent(), m_Callback);
}
