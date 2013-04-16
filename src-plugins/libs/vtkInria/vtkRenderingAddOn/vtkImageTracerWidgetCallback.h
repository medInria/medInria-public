/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#pragma once

#include "vtkRenderingAddOnExport.h"

#include "vtkCommand.h"
#include "vtkPolyData.h"
#include "vtkObject.h"

class vtkViewImage2DWithTracer;

class VTK_RENDERINGADDON_EXPORT vtkImageTracerWidgetCallback : public vtkCommand
{

 public:
  
  static vtkImageTracerWidgetCallback *New()
  { return new vtkImageTracerWidgetCallback; }

  
  virtual void Execute (vtkObject* caller, unsigned long, void*);
  
  void SetView ( vtkViewImage2DWithTracer* view);
  
  vtkViewImage2DWithTracer* GetView() const
  {
    return this->View;
  }

  vtkPolyData* GetPolyData() const
  {
    return this->PathPoly;
  }
  
  

 protected:
 
  vtkImageTracerWidgetCallback();
  ~vtkImageTracerWidgetCallback();
  
  void OnEndInteractionEvent(vtkObject* caller);
  void OnKeyPressEvent (vtkObject* caller, unsigned char);

 
 private:
  vtkImageTracerWidgetCallback (const vtkImageTracerWidgetCallback&);
  void operator=(const vtkImageTracerWidgetCallback&);
  
  
  vtkPolyData*               PathPoly;
  vtkViewImage2DWithTracer*  View;
  
  
};




