/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxHelloWorldApp.h 477 2007-11-20 17:46:10Z filus $
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
#ifndef _wx_ImageViewsWithSlidersApp_h_
#define _wx_ImageViewsWithSlidersApp_h_

// For compilers that supports precompilation , includes  wx/wx.h
#include  "wx/wxprec.h"

#ifndef WX_PRECOMP
#include  "wx/wx.h"
#endif

class wxImageViewsWithSlidersApp : public wxApp
{
  
 public:
  virtual bool OnInit ( void );
  
};

#endif
