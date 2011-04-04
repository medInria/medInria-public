/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkIsosurfaceManager.h 979 2008-10-21 08:04:50Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2008-10-21 10:04:50 +0200 (Tue, 21 Oct 2008) $
Version:   $Revision: 979 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _vtk_IsosurfaceManager_h_
#define _vtk_IsosurfaceManager_h_

#include "vtkINRIA3DConfigure.h"

#include "vtkImageToIsosurface.h"
#include <vector>

#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkLookupTable.h>
#include <vtkProperty.h>
#include <vtkMatrix4x4.h>


class VTK_VISUMANAGEMENT_EXPORT vtkIsosurfaceManager: public vtkObject
{
  
 public:
  
  static vtkIsosurfaceManager* New();
  vtkTypeRevisionMacro(vtkIsosurfaceManager, vtkObject);

  vtkSetObjectMacro (Input, vtkImageData);
  vtkGetObjectMacro (Input, vtkImageData);
  vtkSetObjectMacro (LUT, vtkLookupTable);
  vtkGetObjectMacro (LUT, vtkLookupTable);

  vtkSetObjectMacro (DirectionMatrix, vtkMatrix4x4);
  vtkGetObjectMacro (DirectionMatrix, vtkMatrix4x4);

  /** Set the render window interactor */
  void SetRenderWindowInteractor (vtkRenderWindowInteractor* rwin);
  vtkGetObjectMacro (RenderWindowInteractor, vtkRenderWindowInteractor);

   /** Remove all actors added by this manager. */
  void RemoveAllActors (void);

  /** Get Isosurfaces as vtkActors */
  std::vector<vtkImageToIsosurface*> GetIsosurfaces (void) const
  { return this->Isosurfaces; }
  
  /** Get the recovered isovalues from input image */ 
  std::vector<int> GetIsoValues (void) const
  { return this->IsoValues; }

  unsigned int GetNumberOfIsoValues (void) const
  { return (unsigned int)(this->IsoValues.size()); }

  /** Generate isosurfaces */
  void GenerateData(void);

  /** Reset all data from the manager */
  void ResetData(void);

  /** Specify the visibility of an isosurface */
  void SetVisibility (int IsVisible);

  /** Specify the visibility of an isosurface */
  void SetVisibility (int i, int IsVisible);

  /** Get the visibility of the isosurfaces */
  int GetVisibility (void) const
  { return this->Visibility; }

  /** Get the visibility of the isosurfaces */
  int GetVisibility (int i) const;

  /** Set the opacity to use for isosurfaces */
  void SetOpacity (double val);

  /** Set the opacity of a specific isosurface */
  void SetOpacity (int i, double val);

  /** Get the opacity used for isosurfaces */
  double GetOpacity(void) const
  { return this->Opacity; }

  /** Get the opacity used for isosurfaces */
  double GetOpacity(int i) const;

  /** Set the decimation On/Off */
  void SetDecimationOn (void);

  /** Set the decimation On/Off */
  void SetDecimationOff (void);

  void Enable(void);

  void Disable(void);
  
 protected:
  
  vtkIsosurfaceManager();
  ~vtkIsosurfaceManager();

  /** Update isosurfaces according to parameters */
  void UpdateActors(void);


 private:

  vtkRenderWindowInteractor*           RenderWindowInteractor;    
  std::vector<vtkImageToIsosurface*>   Isosurfaces;
  vtkImageData*                        Input;
  vtkLookupTable*                      LUT;
  std::vector<int>                     IsoValues;
  double*                              DefaultColor;
  int                                  Visibility;
  double                               Opacity;
  int                                  Decimation;

  vtkMatrix4x4                        *DirectionMatrix;

};


#endif



  
