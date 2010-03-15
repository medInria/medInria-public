/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxHelloWorldMainFrameInteractor.h 609 2008-01-14 09:21:24Z filus $
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
#ifndef _wx_HelloWorldMainFrameInteractor_h_
#define _wx_HelloWorldMainFrameInteractor_h_

#include "vtkINRIA3DConfigure.h"

#include "wxHelloWorldMainFrame.h"

/**
   This class aims at defining the virtuals functions (=callbacks)
   of the MainFrame class, that is used only to build the user interface.
 */
class HELLOWORLDLIB_EXPORT wxHelloWorldMainFrameInteractor: public wxHelloWorldMainFrame
{
  
 public:
  wxHelloWorldMainFrameInteractor (wxWindow* parent, int id=-1,
                                   const wxString& title=wxT(""),
                                   const wxPoint& pos=wxDefaultPosition,
                                   const wxSize& size=wxDefaultSize,
                                   long style=wxDEFAULT_FRAME_STYLE);
  
  virtual ~wxHelloWorldMainFrameInteractor(){};


  /**
     This function will open a file dialog, read a file and input the
     file in our four views.
   */
  void OnOpenButton (wxCommandEvent& );
  
 protected:

 private:
  
};


#endif
