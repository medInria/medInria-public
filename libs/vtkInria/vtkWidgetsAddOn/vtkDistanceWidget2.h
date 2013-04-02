/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkDistanceWidget2.h 833 2008-05-15 12:17:31Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2008-05-15 14:17:31 +0200 (Thu, 15 May 2008) $
Version:   $Revision: 833 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#pragma once

#include "vtkWidgetsAddOnExport.h"
#include <vtkLineWidget.h>

class vtkCaptionActor2D;


class VTK_WIDGETSADDON_EXPORT vtkDistanceWidget2 : public vtkLineWidget
{
  
 public:
  
  static vtkDistanceWidget2* New();
  vtkTypeRevisionMacro(vtkDistanceWidget2, vtkLineWidget);

  // Description:
  // Methods that satisfy the superclass' API.
  virtual void SetEnabled(int);
  virtual void PlaceWidget(double bounds[6]);
  void PlaceWidget()
    {this->Superclass::PlaceWidget();}
  void PlaceWidget(double xmin, double xmax, double ymin, double ymax,
                   double zmin, double zmax)
    {this->Superclass::PlaceWidget(xmin,xmax,ymin,ymax,zmin,zmax);}

 protected: 

  vtkDistanceWidget2();
  ~vtkDistanceWidget2();

  virtual void OnMouseMove();
  
  vtkCaptionActor2D* Caption;

};


