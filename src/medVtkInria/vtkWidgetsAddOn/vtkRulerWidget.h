/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medVtkInriaExport.h"
#include <vtkInteractorObserver.h>


class vtkRulerWidgetObserver;
class vtkPolyData;
class vtkActor2D;
class vtkRenderer;

class MEDVTKINRIA_EXPORT vtkRulerWidget : public vtkInteractorObserver
{

 public:
  static vtkRulerWidget *New();
  vtkTypeRevisionMacro(vtkRulerWidget, vtkInteractorObserver);
  void PrintSelf(ostream& os, vtkIndent indent) {};

  virtual void SetEnabled(int);

  void ExecuteCameraUpdateEvent(vtkObject *o, unsigned long event, void *calldata);
  
 protected:
  vtkRulerWidget();
  ~vtkRulerWidget();


 private:
  vtkRulerWidget(const vtkRulerWidget&);
  void operator=(const vtkRulerWidget&);

  vtkRulerWidgetObserver *Observer;
  unsigned long StartEventObserverId;
  
  vtkRenderer *RendererY;
  vtkPolyData *OutlineY;
  vtkActor2D  *OutlineYActor;

  vtkRenderer *RendererX;
  vtkPolyData *OutlineX;
  vtkActor2D  *OutlineXActor;
};



