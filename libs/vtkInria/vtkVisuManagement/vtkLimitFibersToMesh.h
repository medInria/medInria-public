/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkLimitFibersToMesh.h 608 2008-01-14 08:21:23Z filus $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2008-01-14 08:21:23 +0000 (Mon, 14 Jan 2008) $
Version:   $Revision: 608 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#pragma once

#include "vtkVisuManagementExport.h"

#include "vtkPolyDataAlgorithm.h"

class vtkDelaunay3D;


class VTK_VISUMANAGEMENT_EXPORT vtkLimitFibersToMesh: public vtkPolyDataAlgorithm
{

 public:
  static vtkLimitFibersToMesh *New();
  vtkTypeRevisionMacro(vtkLimitFibersToMesh, vtkPolyDataAlgorithm);
    
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




