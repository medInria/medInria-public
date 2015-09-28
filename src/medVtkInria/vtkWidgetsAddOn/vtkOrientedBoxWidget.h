/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medVtkInriaExport.h>
#include <vtkBoxWidget.h>

class vtkMatrix4x4;

/**
   \author Nicolas Toussaint
   \class vtkOrientedBoxWidget
   \brief orthogonal hexahedron 3D widget with pre-defined orientation

   Description

   This 3D widget defines a region of interest. By default it behaves exactly as the
   vtkBoxWidget class does. But if the user set the Orientation matrix with
   SetOrientationMatrix() then all the actors of the widgets (handles, hexahedron, etc)
   will be oriented according to the argument.

   \see
   vtk3DWidget vtkBoxWidget
 */
class MEDVTKINRIA_EXPORT vtkOrientedBoxWidget : public vtkBoxWidget
{
  
 public:
  
  static vtkOrientedBoxWidget* New();
  vtkTypeMacro(vtkOrientedBoxWidget, vtkBoxWidget);

  vtkGetObjectMacro (OrientationMatrix, vtkMatrix4x4);
  virtual void SetOrientationMatrix (vtkMatrix4x4* matrix);
  
 protected: 

  vtkOrientedBoxWidget();
  ~vtkOrientedBoxWidget();

  virtual void OnMouseMove();
  
  vtkMatrix4x4* OrientationMatrix;
  vtkMatrix4x4* InvertedOrientationMatrix;
  

};


