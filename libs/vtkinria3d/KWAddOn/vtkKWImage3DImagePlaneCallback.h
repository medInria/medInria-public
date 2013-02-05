/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxVtkImage3DImagePlaneCallback.h 477 2007-11-20 17:46:10Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2007-11-20 18:46:10 +0100 (Tue, 20 Nov 2007) $
Version:   $Revision: 477 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _vtkKWImage3DImagePlaneCallback_h_
#define _vtkKWImage3DImagePlaneCallback_h_

#include "vtkImage3DImagePlaneCallback.h"
#include "vtkKWViewImage2DDialog.h"

#include <vector>

class vtkDataSet3DCroppingPlaneCallback;
class vtkDataSet;
class vtkPlaneWidget;
class vtkMetaDataSet;
class vtkActor;

class KW_ADDON_EXPORT vtkKWImage3DImagePlaneCallback: public vtkImage3DImagePlaneCallback
{

 public:
  static vtkKWImage3DImagePlaneCallback* New()
  { return new vtkKWImage3DImagePlaneCallback;};
  
  virtual void Execute ( vtkObject *caller, unsigned long, void* );

  void SetPlaneWidget (vtkPlaneWidget* widget);

  void SetViewImage2D (vtkKWViewImage2DDialog* view)
  {
    this->ViewImage2D = view;
    this->FirstRender = true;
  }

  virtual void Reset (void)
  {
    vtkImage3DImagePlaneCallback::Reset();
    this->FirstRender = true;
  }

  void AddMetaDataSet (vtkMetaDataSet* dataset);
  void RemoveMetaDataSet (vtkMetaDataSet* dataset);
  

 protected:
  vtkKWImage3DImagePlaneCallback();
  ~vtkKWImage3DImagePlaneCallback();
  
    

 private:

  vtkKWViewImage2DDialog*   ViewImage2D;
  bool                      FirstRender;
  
  vtkDataSet3DCroppingPlaneCallback* DataSetCropper;

  std::vector<vtkActor*> ActorList;
  
  
  
};

#endif
