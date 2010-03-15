/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxDialogVtkViewImage2D.h 609 2008-01-14 09:21:24Z filus $
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
#ifndef _wx_DialogVtkViewImage2D_h_
#define _wx_DialogVtkViewImage2D_h_

// For compilers that supports precompilation , includes  wx/wx.h
#include  "wx/wxprec.h"

#ifndef WX_PRECOMP
#include  "wx/wx.h"
#endif

class wxVTKRenderWindowInteractor;
class vtkViewImage2D;

#include <vtkINRIA3DConfigure.h>

class WX_VTK_EXPORT wxDialogVtkViewImage2D: public wxDialog
{

 public:

  wxDialogVtkViewImage2D(wxWindow* parent, int id=-1, const wxString& title=wxT (""),
                         const wxPoint& pos=wxDefaultPosition,
                         const wxSize& size=wxDefaultSize,
                         long style = wxDEFAULT_DIALOG_STYLE);
  virtual ~wxDialogVtkViewImage2D();

  virtual void OnCloseEvent (wxCloseEvent &event);

  vtkViewImage2D* GetVtkViewImage2D (void) const
  { return m_View; }
  
  
 protected:

  DECLARE_EVENT_TABLE()
    
 private:

  void DoLayout (void);
  void SetProperties (void);

  wxVTKRenderWindowInteractor* m_wxVtkView;
  vtkViewImage2D* m_View;
  
};

#endif
