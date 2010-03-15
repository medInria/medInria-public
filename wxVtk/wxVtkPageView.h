/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxVtkPageView.h 1300 2009-10-27 21:54:42Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2009-10-27 22:54:42 +0100 (Tue, 27 Oct 2009) $
Version:   $Revision: 1300 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _wxVtkPageView_h_
#define _wxVtkPageView_h_

#include <vtkINRIA3DConfigure.h>

// For compilers that supports precompilation , includes  wx/wx.h
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include  "wx/wx.h"
#endif

#ifdef vtkINRIA3D_USE_ITK
#include <itkImage.h>
#include <itkGDCMImporter.h>
#endif


class wxVTKRenderWindowInteractor;
class vtkViewImage2D;
class vtkViewImage3D;
class vtkImageData;
class vtkLookupTable;
class vtkMatrix4x4;


class WX_VTK_EXPORT wxVtkPageView : public wxPanel
{

 public:

  wxVtkPageView(wxWindow* parent, int id=wxID_ANY,
                 const wxPoint& pos=wxDefaultPosition,
                 const wxSize& size=wxDefaultSize,
                 long style=0,
                 const wxString& title=wxT (""),
				 int layoutMode=LAYOUT_NORMAL,
				 int padSize = 0);
  virtual ~wxVtkPageView();


  /**
     The layout modes allow the user to change the way views
     are arranged together in the PageView
  */
  enum LayoutModeIds
  {
    LAYOUT_NORMAL = 0,
    LAYOUT_NO_3D,
    LAYOUT_3D_ONTOP,
    LAYOUT_3D_ONRIGHT
  };
  
  /**
     The view mode allows allows to build a page view
     with view-with-tracer objects
  */
  enum View2DModeIds
  {
    VIEW_MODE_NORMAL = 0,
    VIEW_MODE_TRACING
  };

  void SetInput (vtkImageData*, const bool& reset=true, vtkMatrix4x4* orientationmatrix = NULL);

#ifdef vtkINRIA3D_USE_ITK
  typedef float ImageComponentType;
  typedef itk::Image<ImageComponentType, 3> ImageType;

  void SetITKInput ( ImageType::Pointer, const bool& reset=true);
  ImageType::Pointer GetItkImage (void);
  
#endif
  vtkImageData* GetInput (void) const;

  vtkViewImage2D* GetView1(void) const
  { return m_View1; }
  vtkViewImage2D* GetView2(void) const
  { return m_View2; }
  vtkViewImage2D* GetView3(void) const
  { return m_View3; }
  vtkViewImage3D* GetView4(void) const
  { return m_View4; }

  void SetLookupTable (vtkLookupTable* lut);
  
  const char* GetImageName (void) const
  { return m_ImageName.c_str(); }
  void SetImageName (const char* name)
  { m_ImageName = name; }
  
  // 2D
  void SetInteractionStyle              (int);
  void SetInteractionStyleToSelector    (void);
  void SetInteractionStyleToWindowLevel (void);
  void SetInteractionStyleToZoom        (void);
  void SetInteractionStyleToFullPage    (void);

  // 3D
  void SetRenderingModeToMPR (void);
  void SetRenderingModeToVR  (void);

  void OnFullScreen (unsigned int id);
  void NoFullScreen (void);

  void Refresh(void);
  
  // render the 4 views
  void Render (void);
  
  wxVTKRenderWindowInteractor* GetDisplayedView (void) const;

  /**
     Set/Get the layout mode of this page !
  */ 
  unsigned int GetLayoutMode (void)
  { return m_LayoutMode; }
  void SetLayoutMode (unsigned int layout)
  {
    m_LayoutMode = layout;
    this->DoLayout();
  }
  unsigned int GetPadSpace (void)
  {return m_PadSpace; }
  void SetPadSpace (unsigned int pad)
  {
    m_PadSpace = pad;
    this->DoLayout();
  }
  unsigned int GetViewMode (void)
  { return m_ViewMode; }
  void SetViewMode (unsigned int mode)
  {
    m_ViewMode = mode;
    this->SetProperties ();
    this->DoLayout();
  }

  unsigned int GetFullScreenMode (void)
  { return m_FullScreenMode; }
  
  void DetachViews (void);
  void AttachViews (void);

  void SetOrientationMatrix (vtkMatrix4x4* matrix);
  vtkMatrix4x4* GetOrientationMatrix (void)
  { return m_OrientationMatrix; }
  
  
 protected:
 
  DECLARE_EVENT_TABLE();
  
  
 private:

  void SetProperties(void);
  void DoLayout(void);

  vtkViewImage2D* m_View1;
  vtkViewImage2D* m_View2;
  vtkViewImage2D* m_View3;
  vtkViewImage3D* m_View4;

  wxVTKRenderWindowInteractor* m_ViewInteractor1;
  wxVTKRenderWindowInteractor* m_ViewInteractor2;
  wxVTKRenderWindowInteractor* m_ViewInteractor3;
  wxVTKRenderWindowInteractor* m_ViewInteractor4;

  unsigned int m_LayoutMode;
  unsigned int m_PadSpace;
  unsigned int m_FullScreenMode;
  unsigned int m_ViewMode;


  vtkMatrix4x4* m_OrientationMatrix;
    
  std::string m_ImageName;

#ifdef vtkINRIA3D_USE_ITK
  ImageType::Pointer ItkImage;
#endif
  
};


#endif
