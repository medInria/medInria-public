/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkOrientedBoxWidget.h 833 2008-05-15 12:17:31Z ntoussaint $
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
#ifndef _vtkOrientedBoxWidget_h_
#define _vtkOrientedBoxWidget_h_

#include "vtkINRIA3DConfigure.h"
#include "vtkBoxWidget.h"

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
class VTK_WIDGETSADDON_EXPORT vtkOrientedBoxWidget : public vtkBoxWidget
{
  
 public:
  
  static vtkOrientedBoxWidget* New();
  vtkTypeRevisionMacro(vtkOrientedBoxWidget, vtkBoxWidget);

  vtkGetObjectMacro (OrientationMatrix, vtkMatrix4x4);
  virtual void SetOrientationMatrix (vtkMatrix4x4* matrix);
  
 protected: 

  vtkOrientedBoxWidget();
  ~vtkOrientedBoxWidget();

  virtual void OnMouseMove();
  
  vtkMatrix4x4* OrientationMatrix;
  vtkMatrix4x4* InvertedOrientationMatrix;
  

};

#endif /* _vtkOrientedBoxWidget_h_ */
