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

#include <vtkImageToIsosurface.h>
#include <vector>

#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkLookupTable.h>
#include <vtkProperty.h>
#include <vtkMatrix4x4.h>


class MEDVTKINRIA_EXPORT vtkIsosurfaceManager: public vtkObject
{
  
 public:
  
  static vtkIsosurfaceManager* New();
  vtkTypeMacro(vtkIsosurfaceManager, vtkObject);

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
  void RemoveAllActors();

  //BTX
  /** Get Isosurfaces as vtkActors */
  std::vector<vtkImageToIsosurface*> GetIsosurfaces() const
  { return this->Isosurfaces; }
  
  /** Get the recovered isovalues from input image */ 
  std::vector<int> GetIsoValues() const
  { return this->IsoValues; }
  //ETX
  
  unsigned int GetNumberOfIsoValues() const
  { return (unsigned int)(this->IsoValues.size()); }

  /** Generate isosurfaces */
  void GenerateData();

  /** Reset all data from the manager */
  void ResetData();

  /** Specify the visibility of an isosurface */
  void SetVisibility (int IsVisible);

  /** Specify the visibility of an isosurface */
  void SetVisibility (int i, int IsVisible);

  /** Get the visibility of the isosurfaces */
  int GetVisibility() const
  { return this->Visibility; }

  /** Get the visibility of the isosurfaces */
  int GetVisibility (int i) const;

  /** Set the opacity to use for isosurfaces */
  void SetOpacity (double val);

  /** Set the opacity of a specific isosurface */
  void SetOpacity (int i, double val);

  /** Get the opacity used for isosurfaces */
  double GetOpacity() const
  { return this->Opacity; }

  /** Get the opacity used for isosurfaces */
  double GetOpacity(int i) const;

  /** Set the decimation On/Off */
  void SetDecimationOn();

  /** Set the decimation On/Off */
  void SetDecimationOff();

  void Enable();

  void Disable();
  
 protected:
  
  vtkIsosurfaceManager();
  ~vtkIsosurfaceManager();

  /** Update isosurfaces according to parameters */
  void UpdateActors();


 private:

  vtkRenderWindowInteractor*           RenderWindowInteractor;    
  //BTX
  std::vector<vtkImageToIsosurface*>   Isosurfaces;
  //ETX
  vtkImageData*                        Input;
  vtkLookupTable*                      LUT;
  //BTX
  std::vector<int>                     IsoValues;
  //ETX
  double*                              DefaultColor;
  int                                  Visibility;
  double                               Opacity;
  int                                  Decimation;

  vtkMatrix4x4                        *DirectionMatrix;

};






  
