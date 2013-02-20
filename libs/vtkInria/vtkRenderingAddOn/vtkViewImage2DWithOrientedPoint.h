/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkViewImage2DWithOrientedPoint.h 1284 2009-10-13 15:19:31Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2009-10-13 17:19:31 +0200 (Tue, 13 Oct 2009) $
Version:   $Revision: 1284 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// version vtkRenderingAddOn
#ifndef _vtkViewImage2DWithOrientedPoint_h_
#define _vtkViewImage2DWithOrientedPoint_h_

#include "vtkRenderingAddOnExport.h"

#include<vector>
#include<iostream>

#include <vtkRenderingAddOn/vtkViewImage2D.h>
#include <vtkImageOrientedPointWidget.h>

class vtkImageData;
class vtkLookupTable;
class vtkImageOrientedPointWidgetCallback;


/**
   \class vtkViewImage2DWithOrientedPoint vtkViewImage2DWithOrientedPoint.h "vtkViewImage2DWithOrientedPoint.h"
   \brief 3D image 2D slice viewer with a registration widget integrated
   \author Nicolas Toussaint
   This class is a vtkViewImage2D for register purposes, it can output point positions
   set by user clicks. 
   
*/

class VTK_RENDERINGADDON_EXPORT vtkViewImage2DWithOrientedPoint: public vtkViewImage2D
{
  
 public:
  
  static vtkViewImage2DWithOrientedPoint* New();
  vtkTypeRevisionMacro(vtkViewImage2DWithOrientedPoint, vtkViewImage2D);

  virtual void SetImage (vtkImageData* image);

  void PrepareForDelete (void);

  /** Activate the manual registering interactor */
  void SetManualRegisteringOn (void);
  
  /** De-activate the manual registering interactor */
  void SetManualRegisteringOff (void);
  
  /** Switch on/off the manual registering */
  void SetManualRegisteringMode (bool a);

  /** Get the manual registering state */
  int GetManualRegisteringMode(void) const
  {
    return this->OrientedPointWidget->GetEnabled();
  }

  
  /**
     This method takes the polydata generated by the tracing
     widget and transform it into registering data : a position
     and a direction.
  */
  void ValidateRegistering (void);
  void UpdateRegistrationData (void);

  
  /** Do not use these methods */
  vtkGetObjectMacro (OrientedPointWidget, vtkImageOrientedPointWidget);
  vtkGetObjectMacro (OrientedPointCbk, vtkImageOrientedPointWidgetCallback);
  

  //BTX
  /**
     information for registration:
     a point
     a line
     the orientation of the view
  */
  struct RegistrationData
  {
    double position[3];
    double direction[3];
    unsigned orientation;
  };
  //ETX

  /** outputs the user set position and direction */
  RegistrationData GetRegistrationData (void) const
  { return this->RegistrationData; }
  
 protected:

  vtkViewImage2DWithOrientedPoint();
  ~vtkViewImage2DWithOrientedPoint();
  

 private:

  // for manual registering
  vtkImageOrientedPointWidgetCallback* OrientedPointCbk;
  vtkImageOrientedPointWidget*         OrientedPointWidget;
  RegistrationData                RegistrationData;
  
};
  


#endif
