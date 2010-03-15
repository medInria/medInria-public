/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxVtkPageView.h 705 2008-02-23 14:04:14Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2008-02-23 15:04:14 +0100 (Sat, 23 Feb 2008) $
Version:   $Revision: 705 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _wx_VtkViewImage2DWithSliders_h_
#define _wx_VtkViewImage2DWithSliders_h_

// For compilers that supports precompilation , includes  wx/wx.h
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include  "wx/wx.h"
#endif

#include <vtkCommand.h>
#include <vtkINRIA3DConfigure.h>

class vtkViewImage2D;
class wxVTKRenderWindowInteractor;



class WX_VTK_EXPORT wxSliderChangeObserver : public vtkCommand
{

 public:
  static wxSliderChangeObserver* New()
  { return new wxSliderChangeObserver; }

  void Execute(vtkObject *   caller, 
               unsigned long event, 
               void *        callData);
  
  wxSliderChangeObserver();
  ~wxSliderChangeObserver();

  void SetZSlider (wxSlider* slider)
  { this->ZSlider = slider; }
  void SetWindowSlider (wxSlider* slider)
  { this->WindowSlider = slider; }
  void SetLevelSlider (wxSlider* slider)
  { this->LevelSlider = slider; }
  void SetZoomSlider (wxSlider* slider)
  { this->ZoomSlider = slider; }

  
  void LockZoom (void)
  { this->ZoomLock = 1; }
  void UnLockZoom (void)
  { this->ZoomLock = 0;}

  void LockZ (void)
  { this->ZLock = 1; }
  void UnLockZ (void)
  { this->ZLock = 0;}

  void LockWindowLevel (void)
  { this->WindowLevelLock = 1; }
  void UnLockWindowLevel (void)
  { this->WindowLevelLock = 0;}

 
 protected:
  
 private:
  wxSlider *ZSlider;
  wxSlider *WindowSlider;
  wxSlider *LevelSlider;
  wxSlider *ZoomSlider;

  int ZoomLock;
  int WindowLevelLock;
  int ZLock;  

};





class WX_VTK_EXPORT wxVtkViewImage2DWithSliders : public wxPanel
{

 public:
  wxVtkViewImage2DWithSliders (wxWindow* parent, int id=wxID_ANY,
                               const wxPoint& pos=wxDefaultPosition,
                               const wxSize& size=wxDefaultSize,
                               long style=0,
                               const wxString& title=wxT (""),
                               unsigned int orientation=0);
  virtual ~wxVtkViewImage2DWithSliders();

  enum
  {
    SL_ZSLIDER,
    SL_WINDOWSLIDER,
    SL_LEVELSLIDER,
    SL_ZOOMSLIDER
  };
  

  vtkViewImage2D* GetViewImage2D (void) const
  { return this->ViewImage2D; }


  virtual void OnZSlider (wxScrollEvent&);
  virtual void OnWindowSlider (wxScrollEvent&);
  virtual void OnLevelSlider (wxScrollEvent&);
  virtual void OnZoomSlider (wxScrollEvent&);

  wxSlider* GetZSlider (void) const
  { return this->ZSlider; }
  wxSlider* GetWindowSlider (void) const
  { return this->WindowSlider; }
  wxSlider* GetLevelSlider (void) const
  { return this->LevelSlider; }
  wxSlider* GetZoomSlider (void) const
  { return this->ZoomSlider; }


 protected:
  
  DECLARE_EVENT_TABLE()

  void SetProperties(void);
  void DoLayout(void);


 private:

  vtkViewImage2D*    ViewImage2D;
  wxVTKRenderWindowInteractor* ViewInteractor;
  wxSlider*                    ZSlider;
  wxSlider*                    WindowSlider;
  wxSlider*                    LevelSlider;
  wxSlider*                    ZoomSlider;

  wxSliderChangeObserver*      SliderObserver;
  
};


#endif
