/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxImageThumbnail.h 1282 2009-10-08 17:00:40Z acanale $
Language:  C++
Author:    $Author: acanale $
Date:      $Date: 2009-10-08 19:00:40 +0200 (Thu, 08 Oct 2009) $
Version:   $Revision: 1282 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _wx_ImageThumbnail_h_
#define _wx_ImageThumbnail_h_

// For compilers that supports precompilation , includes  wx/wx.h
#include  "wx/wxprec.h"

#ifndef WX_PRECOMP
#include  "wx/wx.h"
#endif

#include <vector>

#include "vtkINRIA3DConfigure.h"

#include <vtkViewImage2D.h>

class vtkImageData;
class wxVTKRenderWindowInteractor;
class vtkMatrix4x4;


class IMAGEVIEWER_EXPORT wxImageThumbnail : public wxPanel
{
  
 public:
  
  wxImageThumbnail(wxWindow* parent, int id=wxID_ANY,
                   const wxPoint& pos=wxDefaultPosition,
                   const wxSize& size=wxDefaultSize,
                   long style=wxDEFAULT_FRAME_STYLE,
                   const wxString& title=wxT (""));
  virtual ~wxImageThumbnail()
  {
    this->RemoveAll();
  };
    

   //virtual void OnPaintEvent    (wxPaintEvent &);
  
  
  void AddImage (vtkImageData*, vtkMatrix4x4* matrix, const char*);
  void ChangeImage(const int&, vtkImageData*);
  void RemoveImage (const int&);
  void RemoveAll (void);

  void Render (void);

  void SetInteractionStyle (unsigned int is)
  {
    m_InteractionStyle = is;
    for( unsigned int i=0; i<m_ViewList.size(); i++)
    {
      m_ViewList[i]->SetInteractionStyle (is);
    }
  }
  int GetInteractionStyle (void) const
  {
    return m_InteractionStyle;
  }

  void ChangeOrientation (unsigned int id);
  void LinkViews (bool val);

  void SetLookupTable (vtkLookupTable* lut);
  void SetShow2DAxis  (bool value);
  

  wxFlexGridSizer* m_Sizer;


  /**
     access and set methods for layout settings of the preview screen
     NumberOfColumns : the maximum number of columns allowed
     KeepImageRatio : If on, the preview thumbnails will keep a raisonable ratio
  */

  unsigned int GetNumberOfColumns (void)
  { return this->NumberOfColumns; }
  void SetNumberOfColumns (unsigned int n)
  { this->NumberOfColumns = n; }
  bool GetKeepImageRatio (void)
  { return this->KeepImageRatio; }
  void SetKeepImageRatio (bool n)
  { this->KeepImageRatio = n; }
  void KeepImageRatioOff (void)
  { this->SetKeepImageRatio (false); }
  void KeepImageRatioOn (void)
  { this->SetKeepImageRatio (true); }
  
/*   vtkGetMacro (NumberOfColumns, unsigned int); */
/*   vtkSetMacro (NumberOfColumns, unsigned int); */

/*   vtkGetMacro (KeepImageRatio, int); */
/*   vtkSetMacro (KeepImageRatio, int); */
/*   vtkBooleanMacro (KeepImageRatio, int); */
  
  
 protected:
  
   DECLARE_EVENT_TABLE()
  
 private:

  void SetProperties (void);
  void DoLayout (void);
  
  std::vector<vtkViewImage2D*>              m_ViewList;
  std::vector<wxVTKRenderWindowInteractor*> m_wxViewList;
  
  int m_InteractionStyle;
  bool m_LinkViews;
  unsigned int m_OrientationFlag;

  unsigned int NumberOfColumns;
  bool KeepImageRatio;

  
};

#endif
