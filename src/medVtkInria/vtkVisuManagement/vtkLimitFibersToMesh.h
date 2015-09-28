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

#include <vtkPolyDataAlgorithm.h>

class vtkDelaunay3D;


class MEDVTKINRIA_EXPORT vtkLimitFibersToMesh: public vtkPolyDataAlgorithm
{

 public:
  static vtkLimitFibersToMesh *New();
  vtkTypeMacro(vtkLimitFibersToMesh, vtkPolyDataAlgorithm);
    
  void PrintSelf (ostream& os, vtkIndent indent){};
  
  
  /** Set the Mesh */
  void SetMesh (vtkDataSet* mesh);
  vtkGetObjectMacro (Mesh, vtkDataSet);
  

  /**
     Set the boolean operator: switch between concatenating the fibers (AND - 1)
     and removing the fibers (NOT - 0).
   */
  void SetBooleanOperation (int n)
  {
    this->BooleanOperation = n;
  }
  

  /**
     Set the boolean operator: switch between concatenating the fibers (AND - 0)
     and removing the fibers (NOT - 1).
   */
  void SetBooleanOperationToAND()
  {
    this->BooleanOperation = 1;
  }
  

  /**
     Set the boolean operator: switch between concatenating the fibers (AND - 0)
     and removing the fibers (NOT - 1).
   */
  void SetBooleanOperationToNOT()
  {
    this->BooleanOperation = 0;
  }

  vtkGetMacro (BooleanOperation, int);
  
  
 protected:
  vtkLimitFibersToMesh();
  ~vtkLimitFibersToMesh();
  
  // Usual data generation method
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  

 private:
  vtkLimitFibersToMesh (const vtkLimitFibersToMesh&);
  void operator=(const vtkLimitFibersToMesh&);

  int BooleanOperation;

  vtkDataSet* Mesh;
  vtkDelaunay3D* Mesher;
  double Bounds[6];
  
  
  
};




