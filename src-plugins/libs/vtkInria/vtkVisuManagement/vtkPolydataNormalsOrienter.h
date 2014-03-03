/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "vtkPolyDataAlgorithm.h"

class vtkMatrix4x4;

class vtkPolyDataNormalsOrienter : public vtkPolyDataAlgorithm
{
public:
    static vtkPolyDataNormalsOrienter *New();

    void SetOrientationMatrix(vtkMatrix4x4 *matrix){Matrix=matrix;}

protected:
    vtkPolyDataNormalsOrienter();
    ~vtkPolyDataNormalsOrienter() {}


    virtual int RequestData(vtkInformation* request,
                            vtkInformationVector** inputVector,
                            vtkInformationVector* outputVector);

private:
    vtkMatrix4x4      *Matrix;

};




