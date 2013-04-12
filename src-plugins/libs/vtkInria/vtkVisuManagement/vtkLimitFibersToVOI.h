/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkLimitFibersToVOI.h 1271 2009-09-18 17:22:29Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2009-09-18 19:22:29 +0200 (Fri, 18 Sep 2009) $
Version:   $Revision: 1271 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#pragma once

#include "vtkVisuManagementExport.h"

#include "vtkPolyDataAlgorithm.h"
#include <vector>

class VTK_VISUMANAGEMENT_EXPORT vtkLimitFibersToVOI: public vtkPolyDataAlgorithm
{

 public:
  static vtkLimitFibersToVOI *New();
  vtkTypeRevisionMacro(vtkLimitFibersToVOI, vtkPolyDataAlgorithm);
    
  void PrintSelf (ostream& os, vtkIndent indent){};
  
  
  /** Set the VOI */
  void SetVOI (const double&, const double&, const double&,
               const double&, const double&, const double&);


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
  vtkLimitFibersToVOI();
  ~vtkLimitFibersToVOI(){};
  
  // Usual data generation method
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  

 private:
  vtkLimitFibersToVOI (const vtkLimitFibersToVOI&);
  void operator=(const vtkLimitFibersToVOI&);

  int BooleanOperation;

  double m_XMin;
  double m_XMax;
  double m_YMin;
  double m_YMax;
  double m_ZMin;
  double m_ZMax;
  
};




