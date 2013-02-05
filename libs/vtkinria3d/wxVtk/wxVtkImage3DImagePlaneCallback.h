/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxVtkImage3DImagePlaneCallback.h 611 2008-01-14 10:32:53Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2008-01-14 11:32:53 +0100 (Mon, 14 Jan 2008) $
Version:   $Revision: 611 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _wx_VtkImage3DImagePlaneCallback_h_
#define _wx_VtkImage3DImagePlaneCallback_h_

#include "vtkImage3DImagePlaneCallback.h"
#include "wxDialogVtkViewImage2D.h"
      
class WX_VTK_EXPORT wxVtkImage3DImagePlaneCallback: public vtkImage3DImagePlaneCallback
{

 public:
  static wxVtkImage3DImagePlaneCallback* New()
  { return new wxVtkImage3DImagePlaneCallback;};

  virtual void Execute ( vtkObject *caller, unsigned long, void* );


  void SetViewImage2D (wxDialogVtkViewImage2D* view)
  {
    this->ViewImage2D = view;
    this->FirstRender = true;
  }

  virtual void Reset (void)
  {
    vtkImage3DImagePlaneCallback::Reset();
    this->FirstRender = true;
  }

 protected:
  wxVtkImage3DImagePlaneCallback()
  {
    this->ViewImage2D = 0;
    this->FirstRender = true;
  }
  ~wxVtkImage3DImagePlaneCallback(){}
    

 private:

  wxDialogVtkViewImage2D*   ViewImage2D;
  bool                      FirstRender;
  
};

#endif
