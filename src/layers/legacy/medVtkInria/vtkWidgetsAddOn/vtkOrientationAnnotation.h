#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtkInriaExport.h>

#include <vtkCornerAnnotation.h>

class MEDVTKINRIA_EXPORT vtkOrientationAnnotation : public vtkCornerAnnotation
{
public:
  vtkTypeMacro(vtkOrientationAnnotation,vtkCornerAnnotation)

  // Description:
  // Instantiate object with a rectangle in normaled view coordinates
  // of (0.2,0.85, 0.8, 0.95).
  static vtkOrientationAnnotation *New();

protected:
  vtkOrientationAnnotation();
  ~vtkOrientationAnnotation();

  // Description:
  // Set text actor positions given a viewport size and justification
  void SetTextActorsPosition(const int vsize[2]) override;
  virtual void SetTextActorsJustification();

private:
  vtkOrientationAnnotation(const vtkOrientationAnnotation&);  // Not implemented.
  void operator=(const vtkOrientationAnnotation&);  // Not implemented.
};
