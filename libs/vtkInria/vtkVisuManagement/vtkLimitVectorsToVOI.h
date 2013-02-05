/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkLimitVectorsToVOI.h 608 2008-01-14 08:21:23Z filus $
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
#ifndef _vtk_LimitVectorsToVOI_h_
#define _vtk_LimitVectorsToVOI_h_

#include "vtkINRIA3DConfigure.h"

#include "vtkUnstructuredGridAlgorithm.h"

class VTK_VISUMANAGEMENT_EXPORT vtkLimitVectorsToVOI: public vtkUnstructuredGridAlgorithm
{

 public:
  static vtkLimitVectorsToVOI *New();
  vtkTypeRevisionMacro(vtkLimitVectorsToVOI, vtkUnstructuredGridAlgorithm);
  //vtkStandardNewMacro(vtkLimitVectorsToVOI);
  //{ return new vtkLimitVectorsToVOI; }
    
  void PrintSelf (ostream& os, vtkIndent indent){};
  
  
  /** Set the VOI */
  void SetVOI (const int&, const int&, const int&,
               const int&, const int&, const int&);
  
 protected:
  vtkLimitVectorsToVOI();
  ~vtkLimitVectorsToVOI(){};
  
  // Usual data generation method
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  //----------------------------------------------------------------------------
virtual int RequestInformation(
  vtkInformation* vtkNotUsed(request),
  vtkInformationVector** vtkNotUsed(inputVector),
  vtkInformationVector* vtkNotUsed(outputVector))
{
  // do nothing let subclasses handle it
  return 1;
}


 private:
  vtkLimitVectorsToVOI (const vtkLimitVectorsToVOI&);
  void operator=(const vtkLimitVectorsToVOI&);

  int m_XMin;
  int m_XMax;
  int m_YMin;
  int m_YMax;
  int m_ZMin;
  int m_ZMax;
  
};



#endif
