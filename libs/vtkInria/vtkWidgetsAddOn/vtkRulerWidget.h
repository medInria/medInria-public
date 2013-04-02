/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkViewImage2D.h 1218 2009-08-04 11:29:59Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2009-08-04 13:29:59 +0200 (mar, 04 aoû 2009) $
Version:   $Revision: 1218 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#pragma once

#include "vtkWidgetsAddOnExport.h"
#include <vtkInteractorObserver.h>


class vtkRulerWidgetObserver;
class vtkPolyData;
class vtkActor2D;
class vtkRenderer;

class VTK_WIDGETSADDON_EXPORT vtkRulerWidget : public vtkInteractorObserver
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



