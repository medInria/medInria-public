/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxHelloWorldMainFrame.h 1145 2009-04-16 11:32:29Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2009-04-16 13:32:29 +0200 (Thu, 16 Apr 2009) $
Version:   $Revision: 1145 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef wx_HelloWordMainFrame_h_
#define wx_HelloWordMainFrame_h_

#include "vtkINRIA3DConfigure.h"

// For compilers that supports precompilation , includes  wx/wx.h
#include  "wx/wxprec.h"

#ifndef WX_PRECOMP
#include  "wx/wx.h"
#endif

#include <vtkINRIA3DConfigure.h>

/** I declare here classes I need */
class wxVTKRenderWindowInteractor;
class vtkViewImage2D;
class vtkViewImage3D;
class vtkViewImageCollection;

class HELLOWORLDLIB_EXPORT wxHelloWorldMainFrame : public wxFrame
{

 public:
  
  wxHelloWorldMainFrame (wxWindow* parent, int id=-1,
                         const wxString& title=wxT(""),
                         const wxPoint& pos=wxDefaultPosition,
                         const wxSize& size=wxDefaultSize,
                         long style=wxDEFAULT_FRAME_STYLE);
  virtual ~wxHelloWorldMainFrame();
  

  /**
     My wxHelloWorldMainFrame is made with a button to open an image, and
     four views (3x2D viewer + 1x3D viewer) for image visualization.
  */
  vtkViewImage2D* m_View1;
  vtkViewImage2D* m_View2;
  vtkViewImage2D* m_View3;
  vtkViewImage3D* m_View4;

  /** */
  wxVTKRenderWindowInteractor* m_VtkView1;
  wxVTKRenderWindowInteractor* m_VtkView2;
  wxVTKRenderWindowInteractor* m_VtkView3;
  wxVTKRenderWindowInteractor* m_VtkView4;

  vtkViewImageCollection* m_Pool;

  /**
     In wxWidgets, each wx object is associated to an integer declared in
     an enum. Thus, each event (button pressed, mouse click, etc.) will refer
     to the acting object by its id.
   */

  enum
  {
    OPEN_BUTTON
  };


  /**
     This function will be called when one hits the open button.
   */
  virtual void OnOpenButton (wxCommandEvent& );
  

 protected:

  /**
     In wxWidgets, one have to declare a table that will associate to each
     event (action) a callback function. This is a c++ macro.
   */
  DECLARE_EVENT_TABLE()

 private:

  void SetProperties (void);
  void DoLayout (void);

};


#endif
