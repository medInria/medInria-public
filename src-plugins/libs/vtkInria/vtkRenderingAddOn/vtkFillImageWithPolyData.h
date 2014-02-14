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

#include <vtkThreadedImageAlgorithm.h>
#include <vtkPolyData.h>

class VTK_RENDERINGADDON_EXPORT vtkFillImageWithPolyData : public vtkThreadedImageAlgorithm
{

 public:
  static vtkFillImageWithPolyData *New();
  vtkTypeRevisionMacro (vtkFillImageWithPolyData, vtkThreadedImageAlgorithm);
  void PrintSelf (ostream &os, vtkIndent indent);


  // Set/Get the polyData:
  vtkSetObjectMacro (PolyData, vtkPolyData);
  vtkGetObjectMacro (PolyData, vtkPolyData);

  vtkSetMacro (InsidePixelValue, double);
  vtkGetMacro (InsidePixelValue, double);


  vtkSetMacro (ExtractionDirection, int);//0: X, 1:Y, 2:Z
  vtkGetMacro (ExtractionDirection, int);
  
  
  double Angle2D (const double dp1[2], const double dp2[2]);
  

 protected:
  vtkFillImageWithPolyData();
  ~vtkFillImageWithPolyData();


  virtual int RequestInformation (vtkInformation *, 
                                  vtkInformationVector **,
                                  vtkInformationVector *);
  
 
  virtual void ThreadedRequestData(vtkInformation *request, 
                                   vtkInformationVector **inputVector, 
                                   vtkInformationVector *outputVector,
                                   vtkImageData ***inData, 
                                   vtkImageData **outData,
                                   int extent[6], int threadId);

 private:
  vtkFillImageWithPolyData (const vtkFillImageWithPolyData&);
  void operator=(const vtkFillImageWithPolyData&);

  vtkPolyData* PolyData;
  double       InsidePixelValue;
  int          ExtractionDirection;
  double       BBox[6];
  
  
};




