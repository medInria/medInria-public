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
#include <vtkPolyData.h>
#include <vtkRenderingAddOn/vtkViewImage2DWithOrientedPoint.h>
#include <vtkObject.h>
#include <vtkImageMapToWindowLevelColors.h>

class VTK_RENDERINGADDON_EXPORT vtkImageOrientedPointWidgetCallback : public vtkCommand
{

 public:
  
  static vtkImageOrientedPointWidgetCallback *New()
  { return new vtkImageOrientedPointWidgetCallback; }

  
  virtual void Execute (vtkObject* caller, unsigned long, void*);
  
  void SetView ( vtkViewImage2DWithOrientedPoint* view);

  
  vtkViewImage2DWithOrientedPoint* GetView() const
  {
    return this->View;
  }

  vtkPolyData* GetPolyData() const
  {
    return this->PathPoly;
  }

/*   void SetPositionText (wxStaticText* text) */
/*   { this->PositionText = text; } */
/*   wxStaticText* GetPositionText() const */
/*   { return this->PositionText; } */
    
/*   void SetDirectionText (wxStaticText* text) */
/*   { this->DirectionText = text; } */
/*   wxStaticText* GetDirectionText() const */
/*   { return this->DirectionText; } */

 
  void RefreshText();
  
  void UpdatePoints(vtkObject* caller);

  const double* GetPosition() const
  {
    return this->Position;
  }
  const double* GetDirection() const
  {
    return this->Direction;
  }
  

 protected:
 
 vtkImageOrientedPointWidgetCallback();
  ~vtkImageOrientedPointWidgetCallback();

  void OnEndInteractionEvent(vtkObject* caller);
  void OnMouseMoveEvent(vtkObject* caller);
  void OnKeyPressEvent (vtkObject* caller, unsigned char);

 
 private:
  vtkImageOrientedPointWidgetCallback (const vtkImageOrientedPointWidgetCallback&);
  void operator=(const vtkImageOrientedPointWidgetCallback&);


  vtkPolyData*                 PathPoly;
  vtkViewImage2DWithOrientedPoint*  View;
/*   wxStaticText*              m_PositionText; */
/*   wxStaticText*              m_DirectionText; */
  vtkPoints*                   Points;
  double*                      Position;
  double*                      Direction;
  
};




