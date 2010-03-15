/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxSnapshotTaker.h 609 2008-01-14 09:21:24Z filus $
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
#ifndef _wx_SnapshotTaker_h_
#define _wx_SnapshotTaker_h_

// For compilers that supports precompilation , includes  wx/wx.h
#include  "wx/wxprec.h"

#ifndef WX_PRECOMP
#include  "wx/wx.h"
#endif

#include <vtkRenderWindow.h>

#include <vtkINRIA3DConfigure.h>

class WX_VTK_EXPORT wxSnapshotTaker
{

 public:

  wxSnapshotTaker(wxWindow* parent);
  virtual ~wxSnapshotTaker();

  void SetRenderWindow (vtkRenderWindow* rwin)
  { m_RenderWindow = rwin;}

  vtkRenderWindow* GetRenderWindow (void) const
  { return m_RenderWindow; }

  void SetMagnification(const int& n)
  { m_Magnification = n; }
  int GetMagnification (void) const
  { return m_Magnification; }
  
  
  /** Actually take the snapshot */
  void Snap (void);

 protected:

 private:

  wxWindow*        m_Parent;
  vtkRenderWindow* m_RenderWindow;
  int              m_Magnification;
  
};


#endif
