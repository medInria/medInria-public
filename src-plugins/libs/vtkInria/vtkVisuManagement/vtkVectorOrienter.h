/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "vtkImageAlgorithm.h"
#include "vtkVisuManagementExport.h"

class vtkMatrix4x4;

class VTK_VISUMANAGEMENT_EXPORT vtkVectorOrienter : public vtkImageAlgorithm
{
public:

    enum ProjectionMode
    {
        ProjectOnYZ,
        ProjectOnXZ,
        ProjectOnXY
    };

  vtkTypeMacro(vtkVectorOrienter,vtkImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Construct object to extract all of the input data.
  static vtkVectorOrienter *New();

  // Description:
  // Specify i-j-k (min,max) pairs to extract. The resulting structured points
  // dataset can be of any topological dimension (i.e., point, line, image,
  // or volume).
  void SetVOI(const int&,const int&,const int&,const int&,const int&,const int&);

  // Description:
  // Set Orientation Matrix
  void SetOrientationMatrix(vtkMatrix4x4 *matrix){Matrix=matrix;}

  void SetProjection(bool enable){Projection=enable;}
  bool GetProjection(){return Projection;}


protected:
  vtkVectorOrienter();
  ~vtkVectorOrienter() {}


  virtual int RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector);



private:
  vtkVectorOrienter(const vtkVectorOrienter&);  // Not implemented.
  void operator=(const vtkVectorOrienter&);  // Not implemented.

  ProjectionMode    ProjMode;

  int               VOI[6];
  vtkMatrix4x4      *Matrix;
  bool              Projection;
};
