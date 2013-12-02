/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "vtkRenderingAddOnExport.h"

#include <vtkCommand.h>
#include <vtkDataSet.h>
#include <vtkPlane.h>
#include <vtkPlaneWidget.h>
#include <vtkSetGet.h>
#include <vtkObjectFactory.h>
#include <vtkPlane.h>

#include <vector>

class VTK_RENDERINGADDON_EXPORT vtkDataSet3DCroppingPlaneCallback: public vtkCommand
{

 public:
  static vtkDataSet3DCroppingPlaneCallback* New()
  { return new vtkDataSet3DCroppingPlaneCallback; };

  virtual void Execute ( vtkObject *caller, unsigned long, void* );

  void SetPlaneWidget (vtkPlaneWidget* widget)
  {
    this->PlaneWidget = widget;
  }

  vtkPlane* GetPlane()
  {
    return this->Plane;
  }
  
  
  
  unsigned int AddInput (vtkDataSet* dataset);
  unsigned int RemoveInput (vtkDataSet* dataset);
  
  vtkDataSet* GetInput (unsigned int i);
  vtkDataSet* GetOutput (unsigned int i);
  
  
 protected:
  vtkDataSet3DCroppingPlaneCallback()
  {
    this->Plane = vtkPlane::New();
  };
  
  ~vtkDataSet3DCroppingPlaneCallback()
  {
    this->Plane->Delete();
    
  };
  
 private:

  //BTX
  std::vector<vtkDataSet*> InputList;
  std::vector<vtkDataSet*> OutputList;
  //ETX

  vtkPlaneWidget* PlaneWidget;
  vtkPlane* Plane;
  
  
};



