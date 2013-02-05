/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxDicomInfoDialog.h 1 2007-11-20 17:46:10Z ntoussaint $
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
#ifndef _wx_VtkDICOMExporter_h_
#define _wx_VtkDICOMExporter_h_

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
#include  "wx/wx.h"
#endif

#include <wx/listctrl.h>
#include <string>
#include <vector>
#include <itkGDCMExporter.h>
#include <itkDicomDictionary.h>

#include <vtkINRIA3DConfigure.h>

/**
   \class wxDicomInfoDialog wxDicomInfoDialog.h "wxDicomInfoDialog.h"

   \author  Nicolas Toussaint,
   \date    Dec. 2007
*/

class WX_VTK_EXPORT wxDicomInfoDialog: public wxDialog
{

 public:
  wxDicomInfoDialog (wxWindow* parent, int id,
		      const wxString& title=wxT (""),
		      const wxPoint& pos=wxDefaultPosition,
		      const wxSize& size=wxDefaultSize,
		      long style=wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
  virtual ~wxDicomInfoDialog();
  
  /** callback : internal use only */
  void OnButtonCANCEL (wxCommandEvent &event);
  void OnButtonOK     (wxCommandEvent &event);
  
  void SetDicomTagList (std::vector<itk::DicomTag> list);  

  wxString Title;
  wxSize   Size;
  wxPoint  Pos;
  long     Style;

  wxButton* ButtonCANCEL;
  wxButton* ButtonOK;

  wxListCtrl* ListBox;
  
  
 protected:
  
  DECLARE_EVENT_TABLE()
  void SetProperties (void);
  void DoLayout(void);

 private:
  
  
  enum
  {
    LISTBOX_FLAGS,
    BUTTON_CANCEL,
    BUTTON_OK
  };
  
  
};



#endif
