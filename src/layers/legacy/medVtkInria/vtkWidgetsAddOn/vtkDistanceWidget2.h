/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medVtkInriaExport.h>
#include <vtkLineWidget.h>

class vtkCaptionActor2D;


class MEDVTKINRIA_EXPORT vtkDistanceWidget2 : public vtkLineWidget
{
  
 public:
  
  static vtkDistanceWidget2* New();
  vtkTypeMacro(vtkDistanceWidget2, vtkLineWidget);

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


