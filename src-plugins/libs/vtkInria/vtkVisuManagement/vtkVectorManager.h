/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "vtkVisuManagementExport.h"

#include <vtkObject.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkMatrix4x4.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>

#include "vtkVectorVisuManager.h"

class VTK_VISUMANAGEMENT_EXPORT vtkVectorManager: public vtkObject
{

 public:

  static vtkVectorManager *New();

  void SetRenderWindowInteractor (vtkRenderWindowInteractor*, vtkRenderer* ren=0);
  vtkGetObjectMacro (RenderWindowInteractor, vtkRenderWindowInteractor);

  vtkSetObjectMacro (Input, vtkImageData);
  vtkGetObjectMacro (Input, vtkImageData);

  vtkGetObjectMacro (VectorVisuManagerAxial,    vtkVectorVisuManager);
  vtkGetObjectMacro (VectorVisuManagerSagittal, vtkVectorVisuManager);
  vtkGetObjectMacro (VectorVisuManagerCoronal,  vtkVectorVisuManager);

  void SetDirectionMatrix(vtkMatrix4x4 *mat);
  vtkGetObjectMacro (DirectionMatrix, vtkMatrix4x4);


    /** Set a scaling factor for the glyhs. */
  void SetGlyphScale(const float& f);

  /** Set the sample rate. 1 over n tensors will be
      displaid.*/
  void SetSampleRate(const int&,const int&,const int&);

  /** Generate the glyph and add them to the RenderWindowInteractor */
  void Update();

  /** Remove the actors from the renderer. */
  void Initialize();

  /** Set the current position in voxel coordinates. */
  void SetCurrentPosition (const int&, const int&, const int&);

  /** Set the current position in voxel coordinates. */
  void SetCurrentPosition (int pos[3]);

  /** Set the current position in physical coordinates. */
  void SetCurrentPosition(const double&, const double&, const double&);

  /** Set the current position in physical coordinates. */
  void SetCurrentPosition(double pos[3]);

  /** Get the current position. */
  const int* GetCurrentPosition() const
  { return this->CurrentPosition; }

  /** Get the current position. */
  void GetCurrentPosition (int pos[3]) const
  { for( int i=0; i<3; i++)
      pos[i] = this->CurrentPosition[i];
  }

  /** Reset the position to the center of the dataset. */
  void ResetPosition();

  /** Set the Axial/Coronal/Sagittal slice visibility on or off. */
  void SetAxialSliceVisibility (int);

  /** Set the Axial/Coronal/Sagittal slice visibility on or off. */
  void SetSagittalSliceVisibility (int);

  /** Set the Axial/Coronal/Sagittal slice visibility on or off. */
  void SetCoronalSliceVisibility (int);

  void SetScaleMode(int mode);
  void SetColorMode(int mode);
  void SetProjection(bool enable);


 protected:
  vtkVectorManager();
  ~vtkVectorManager();

 private:

  vtkRenderWindowInteractor* RenderWindowInteractor;

  vtkImageData* Input;

  vtkMatrix4x4 *DirectionMatrix;
  vtkMatrix4x4 *PhysicalToVoxelCoordinatesTransformMatrix;

  //BTX
  vtkSmartPointer<vtkRenderer> Renderer;
  //ETX
  vtkVectorVisuManager* VectorVisuManagerAxial;
  vtkVectorVisuManager* VectorVisuManagerSagittal;
  vtkVectorVisuManager* VectorVisuManagerCoronal;

  int CurrentPosition[3];

};



