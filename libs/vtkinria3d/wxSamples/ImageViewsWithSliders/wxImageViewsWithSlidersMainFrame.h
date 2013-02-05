/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxHelloWorldMainFrame.h 609 2008-01-14 09:21:24Z filus $
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
#ifndef wx_ImageViewsWithSlidersMainFrame_h_
#define wx_ImageViewsWithSlidersMainFrame_h_

#include "vtkINRIA3DConfigure.h"

// For compilers that supports precompilation , includes  wx/wx.h
#include  "wx/wxprec.h"

#ifndef WX_PRECOMP
#include  "wx/wx.h"
#endif


#if defined (WIN32) && defined (wxWidgets_BUILD_SHARED_LIBS)

 #if defined (ImageViewsWithSlidersLib_EXPORTS)
  #define ImageViewsWithSlidersLib_EXPORT __declspec( dllexport )
 #else
  #define ImageViewsWithSlidersLib_EXPORT __declspec( dllimport )
 #endif
#else
  #define ImageViewsWithSlidersLib_EXPORT
#endif

/** I declare here classes I need */
class wxVtkViewImage2DWithSliders;

class ImageViewsWithSlidersLib_EXPORT wxImageViewsWithSlidersMainFrame : public wxFrame
{

 public:
  
  wxImageViewsWithSlidersMainFrame (wxWindow* parent, int id=-1,
                                    const wxString& title=wxT(""),
                                    const wxPoint& pos=wxDefaultPosition,
                                    const wxSize& size=wxDefaultSize,
                                    long style=wxDEFAULT_FRAME_STYLE);
  virtual ~wxImageViewsWithSlidersMainFrame();
  

  
  
  wxVtkViewImage2DWithSliders* m_View1;
  wxVtkViewImage2DWithSliders* m_View2;
  wxVtkViewImage2DWithSliders* m_View3;

  enum
  {
    OPEN_BUTTON
  };


  virtual void OnOpenButton (wxCommandEvent& );
  

 protected:

  DECLARE_EVENT_TABLE()

 private:

  void SetProperties (void);
  void DoLayout (void);

};


#endif
