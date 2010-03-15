/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkImageTracerWidgetCallback.h 1080 2009-02-18 13:27:02Z acanale $
Language:  C++
Author:    $Author: acanale $
Date:      $Date: 2009-02-18 14:27:02 +0100 (Wed, 18 Feb 2009) $
Version:   $Revision: 1080 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// version vtkRenderingAddOn
#ifndef _vtkImageTracerWidgetCallback_h_
#define _vtkImageTracerWidgetCallback_h_

#include "vtkINRIA3DConfigure.h"

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
  
  vtkViewImage2DWithTracer* GetView (void) const
  {
    return this->View;
  }

  vtkPolyData* GetPolyData (void) const
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



#endif
