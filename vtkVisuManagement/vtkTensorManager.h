/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkTensorManager.h 1016 2008-12-03 10:24:21Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2008-12-03 11:24:21 +0100 (Wed, 03 Dec 2008) $
Version:   $Revision: 1016 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _vtk_TensorManager_h_
#define _vtk_TensorManager_h_

#include "vtkINRIA3DConfigure.h"

#include <vtkObject.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkStructuredPoints.h>
#include <vtkLookupTable.h>
#include <vtkMatrix4x4.h>

#include "vtkTensorVisuManager.h"

class VTK_VISUMANAGEMENT_EXPORT vtkTensorManager: public vtkObject
{

 public:

  static vtkTensorManager *New();
  vtkTypeRevisionMacro (vtkTensorManager, vtkObject);


  //vtkSetObjectMacro (RenderWindowInteractor, vtkRenderWindowInteractor);
  void SetRenderWindowInteractor (vtkRenderWindowInteractor*, vtkRenderer* ren=0);
  vtkGetObjectMacro (RenderWindowInteractor, vtkRenderWindowInteractor);

  vtkSetObjectMacro (Input, vtkStructuredPoints);
  vtkGetObjectMacro (Input, vtkStructuredPoints);

  void SetDirectionMatrix(vtkMatrix4x4 *mat);
  vtkGetObjectMacro (DirectionMatrix, vtkMatrix4x4);

  vtkGetObjectMacro (TensorVisuManagerAxial,    vtkTensorVisuManager);
  vtkGetObjectMacro (TensorVisuManagerSagittal, vtkTensorVisuManager);
  vtkGetObjectMacro (TensorVisuManagerCoronal,  vtkTensorVisuManager);



  /** Set the glyph shape */
  void SetGlyphShape (int i);
  
  /** Set a specific glyph shape */
  void SetGlyphShapeToLine (void);
  
  /** Set a specific glyph shape */
  void SetGlyphShapeToDisk (void);
    
  /** Set a specific glyph shape */
  void SetGlyphShapeToArrow (void);
  
  /** Set a specific glyph shape */
  void SetGlyphShapeToCube (void);
  
  /** Set a specific glyph shape */
  void SetGlyphShapeToCylinder (void);
  
  /** Set a specific glyph shape */
  void SetGlyphShapeToSphere (void);
  
  /** Set a specific glyph shape */
  void SetGlyphShapeToSuperquadric (void);

    /** Set a scaling factor for the glyhs. */
  void SetGlyphScale(const float& f);
  
  /** Set a maximum size for the glyphs. The size
      is clamped to the maximum if it is over this
      value. */
  void SetMaxGlyphSize(const float& f);
  
  /** Set the sample rate. 1 over n tensors will be
      displaid.*/
  void SetSampleRate(const int&,const int&,const int&);
  
  /** Set the glyph resolution */
  void SetGlyphResolution (int res);
  
  /** Set the scalar range for the LUT */
  void SetScalarRange(const float&, const float&);

  /** Set the colormode to map the ith eigenvector. */
  void SetColorModeToEigenvector( const int& );
  
  /** Set the colormode to map the ith eigenvalue. */
  void SetColorModeToEigenvalue( const int& );
  
  /** Set the colormode to map the volume. */
  void SetColorModeToVolume( void );
  
  /** Set the colormode to map the trace. */
  void SetColorModeToTrace( void );
  
  /** Set the colormode to map the distance to the identity. */
  void SetColorModeToDistanceToIdentity( void );
  
  /** Set the colormode. */
  void SetColorMode( const int& );

  /** Flip tensors along the X axis */
  void FlipX (bool a);
  
  /** Flip tensors along the Y axis */
  void FlipY (bool a);
  
  /** Flip tensors along the Z axis */
  void FlipZ (bool a);

  /** Set a user-defined LUT. */
  void SetLookupTable (vtkLookupTable* lut);

  /** Set a scalar array to color the glyph with. */
  void SetScalars (vtkDataArray* scalars);


  /** Generate the glyph and add them to the RenderWindowInteractor */
  void Update (void);
  
  
  /** Remove the actors from the renderer. */
  void Initialize (void);

  /** Set the current position in voxel coordinates. */
  void SetCurrentPosition (const int&, const int&, const int&);

  /** Set the current position in voxel coordinates. */
  void SetCurrentPosition (int pos[3]);

  /** Set the current position in physical coordinates. */
  void SetCurrentPosition(const double&, const double&, const double&);

  /** Set the current position in physical coordinates. */
  void SetCurrentPosition(double pos[3]);

  /** Get the current position. */
  const int* GetCurrentPosition (void) const
  { return this->CurrentPosition; }

  /** Get the current position. */
  void GetCurrentPosition (int pos[3]) const
  { for( int i=0; i<3; i++)
      pos[i] = this->CurrentPosition[i];
  }

  
  /** Reset the position to the center of the dataset. */
  void ResetPosition (void);

  /** Set the Axial/Coronal/Sagittal slice visibility on or off. */
  void SetAxialSliceVisibility (int);

  /** Set the Axial/Coronal/Sagittal slice visibility on or off. */
  void SetSagittalSliceVisibility (int);

  /** Set the Axial/Coronal/Sagittal slice visibility on or off. */
  void SetCoronalSliceVisibility (int);
  
  
 protected:
  vtkTensorManager();
  ~vtkTensorManager();
  

 private:

  
  vtkRenderWindowInteractor* RenderWindowInteractor;

  vtkStructuredPoints* Input;

  vtkRenderer* Renderer;

  vtkTensorVisuManager* TensorVisuManagerAxial;
  vtkTensorVisuManager* TensorVisuManagerSagittal;
  vtkTensorVisuManager* TensorVisuManagerCoronal;

  vtkMatrix4x4 *DirectionMatrix;
  vtkMatrix4x4 *PhysicalToVoxelCoordinatesTransformMatrix;

  int CurrentPosition[3];
  
};


#endif
