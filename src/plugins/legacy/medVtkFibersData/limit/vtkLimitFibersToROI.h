#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtkFibersDataPluginExport.h>

#include <vtkPolyDataAlgorithm.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vector>


class MEDVTKFIBERSDATAPLUGIN_EXPORT vtkLimitFibersToROI: public vtkPolyDataAlgorithm
{

 public:
  static vtkLimitFibersToROI *New();
  vtkTypeMacro(vtkLimitFibersToROI, vtkPolyDataAlgorithm);

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

  int* GetBooleanOperationVector()
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




