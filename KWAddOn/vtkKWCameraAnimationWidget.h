/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWCameraAnimationWidget.h 560 2007-12-17 13:20:51Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2007-12-17 14:20:51 +0100 (Mon, 17 Dec 2007) $
Version:   $Revision: 560 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _vtkKWCameraAnimationWidget_h
#define _vtkKWCameraAnimationWidget_h

#include "vtkINRIA3DConfigure.h"
#include "vtkKWSimpleAnimationWidget.h"


class KW_ADDON_EXPORT vtkKWCameraAnimationWidget : public vtkKWSimpleAnimationWidget
{
public:
  static vtkKWCameraAnimationWidget* New();
  vtkTypeRevisionMacro(vtkKWCameraAnimationWidget,vtkKWSimpleAnimationWidget);
  
  // Description:

  virtual void SetSliceStart (int val)
  { this->SliceRangeValues[0] = val;}
  
  virtual void SetSliceEnd (int val)
  { this->SliceRangeValues[1] = val;}
  virtual void SetSliceRangeValues (int val[2])
  {
    this->SliceRangeValues[0] = val[0];
    this->SliceRangeValues[1] = val[0];
  }
  virtual void SetRotationX (int val)
  { this->RotationValues[0] = val;}
  virtual void SetRotationY (int val)
  { this->RotationValues[1] = val;}
  virtual void SetRotationZ (int val)
  { this->RotationValues[2] = val;}
  virtual void SetRotationValues (int val[3])
  {
    this->RotationValues[0] = val[0];
    this->RotationValues[1] = val[1];
    this->RotationValues[2] = val[2];
  }
  
  virtual void SetZoomFactorValue (double val)
  { this->ZoomFactorValue = val;}

  virtual void Update();
  
  
protected:
  vtkKWCameraAnimationWidget();
  ~vtkKWCameraAnimationWidget(){};


private:
  vtkKWCameraAnimationWidget(const vtkKWCameraAnimationWidget&);   // Not implemented.
  void operator=(const vtkKWCameraAnimationWidget&);        // Not implemented.

  int SliceRangeValues[2];
  int RotationValues[3];
  double ZoomFactorValue;
  
  
};

#endif
