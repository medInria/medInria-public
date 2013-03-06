/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkDisplacementFieldToGridFilter.h 608 2008-01-14 08:21:23Z filus $
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
#pragma once

#include "vtkVisuManagementExport.h"

#include <vtkPolyDataAlgorithm.h>

/**
   \class vtkDisplacementFieldToGridFilter vtkDisplacementFieldToGridFilter.h "vtkDisplacementFieldToGridFilter.h"
   \brief Takes a displacement field as input and warp a regular grid with it, export a polydata
   \author: Nicolas Toussaint
*/

class vtkImageData;


class VTK_VISUMANAGEMENT_EXPORT vtkDisplacementFieldToGridFilter: public vtkPolyDataAlgorithm
{
  
 public:

  static vtkDisplacementFieldToGridFilter* New();
  vtkTypeRevisionMacro(vtkDisplacementFieldToGridFilter, vtkPolyDataAlgorithm);


  unsigned int* GetResolution (void)
  {
    return this->Resolution;
  }
  void SetResolution (unsigned int rx, unsigned int ry, unsigned int rz)
  {
    this->Resolution[0] = rx;
    this->Resolution[1] = ry;
    this->Resolution[2] = rz;
  }
  
  void SetResolution (unsigned int res)
  {
    this->Resolution[0] = res;
    this->Resolution[1] = res;
    this->Resolution[2] = res;
  }
  

  // Description:
  // Set an Input of this filter.  This method is only for support of
  // old-style pipeline connections.  When writing new code you should
  // use SetInputConnection(), AddInputConnection(), and
  // ReplaceNthInputConnection() instead.
  void SetInput(int num, vtkDataObject *input);
  void SetInput(vtkDataObject *input) { this->SetInput(0, input); };

  // Description:
  // Get one input to this filter. This method is only for support of
  // old-style pipeline connections.  When writing new code you should
  // use vtkAlgorithm::GetInputConnection(0, num).
  vtkDataObject *GetInput(int num);
  vtkDataObject *GetInput() { return this->GetInput(0); };

  // Description:
  // Get the number of inputs to this filter. This method is only for
  // support of old-style pipeline connections.  When writing new code
  // you should use vtkAlgorithm::GetNumberOfInputConnections(0).
  int GetNumberOfInputs() { return this->GetNumberOfInputConnections(0); };
  
 protected:
  
  vtkDisplacementFieldToGridFilter();
  ~vtkDisplacementFieldToGridFilter();  

  // see vtkAlgorithm for docs.
  virtual int FillInputPortInformation(int, vtkInformation*);

  virtual int RequestInformation (vtkInformation *, 
                                  vtkInformationVector **,
                                  vtkInformationVector *);
  
  // see vtkAlgorithm for docs.
  virtual int RequestData(vtkInformation* request,
				 vtkInformationVector** inputVector,
				 vtkInformationVector* outputVector);


  
  unsigned int Resolution[3];

  void FillRegularGridFromImage (vtkImageData* image, vtkPolyData* polydata, unsigned int resolution[3]);
  
  
  
 private:

  vtkDisplacementFieldToGridFilter(const vtkDisplacementFieldToGridFilter&);  // Not implemented.
  void operator=(const vtkDisplacementFieldToGridFilter&);  // Not implemented.
  
};






  
