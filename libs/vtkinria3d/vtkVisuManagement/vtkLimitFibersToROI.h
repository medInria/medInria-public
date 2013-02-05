/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkLimitFibersToROI.h 608 2008-01-14 08:21:23Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2008-01-14 09:21:23 +0100 (Mon, 14 Jan 2008) $
Version:   $Revision: 608 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _vtk_LimitFibersToROI_h_
#define _vtk_LimitFibersToROI_h_

#include "vtkINRIA3DConfigure.h"

#include "vtkPolyDataAlgorithm.h"
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vector>


class VTK_VISUMANAGEMENT_EXPORT vtkLimitFibersToROI: public vtkPolyDataAlgorithm
{

 public:
  static vtkLimitFibersToROI *New();
  vtkTypeRevisionMacro(vtkLimitFibersToROI, vtkPolyDataAlgorithm);

  vtkSetObjectMacro (MaskImage,  vtkImageData);
  vtkGetObjectMacro (MaskImage,  vtkImageData);

  vtkSetObjectMacro (DirectionMatrix,  vtkMatrix4x4);
  vtkGetObjectMacro (DirectionMatrix,  vtkMatrix4x4);
  
  void PrintSelf (ostream& os, vtkIndent indent){};


  /**
     Set the BooleanOperationVector array. The length of BooleanOperationVector
     must be equal to the maximum value of the MaskImage. Values must be either
     0 (NOT), 1 (AND).
   */
  void SetBooleanOperationVector (int vec[256])
  {
    for( unsigned int i=0; i<256; i++)
    {
      this->BooleanOperationVector[i] = vec[i];
    }
    this->Modified();
  }

  void SetBooleanOperation (int id, int value);

  int* GetBooleanOperationVector (void)
  {
    return this->BooleanOperationVector;
  }
  
  
  
    
 protected:
  vtkLimitFibersToROI();
  ~vtkLimitFibersToROI();
  
  // Usual data generation method
  int RequestData (vtkInformation *,vtkInformationVector **,vtkInformationVector *);
  

 private:
  vtkLimitFibersToROI (const vtkLimitFibersToROI&);
  void operator=(const vtkLimitFibersToROI&);
  
  vtkImageData* MaskImage;
  vtkMatrix4x4* DirectionMatrix;

  int BooleanOperationVector[256];
 
  
};



#endif
