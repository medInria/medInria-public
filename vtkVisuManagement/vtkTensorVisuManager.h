/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkTensorVisuManager.h 881 2008-06-08 17:46:01Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2008-06-08 19:46:01 +0200 (Sun, 08 Jun 2008) $
Version:   $Revision: 881 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _VTK_TENSOR_VISU_MANAGER_H_
#define _VTK_TENSOR_VISU_MANAGER_H_

#include "vtkINRIA3DConfigure.h"

#include <vtkObject.h>

// vtk includes
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyDataNormals.h"
#include "vtkTensorGlyph.h" 
#include "vtkStructuredPoints.h"
#include "vtkUnstructuredGrid.h"
#include "vtkExtractVOI.h"
#include "vtkLookupTable.h"
#include "vtkFlipTensorImageFilter.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkUnsignedIntArray.h"
#include "vtkDoubleArray.h"

class vtkMatrix4x4;

class VTK_VISUMANAGEMENT_EXPORT vtkTensorVisuManager : public vtkObject
{
 public:
  
  static vtkTensorVisuManager *New();
  vtkTypeRevisionMacro(vtkTensorVisuManager, vtkObject);
  
 
  /** enum of the possible shapes for the glyphs */
  enum GlyphShapeMode
  {
    GLYPH_LINE,        //0
    GLYPH_ARROW,       //1
    GLYPH_DISK,        //2
    GLYPH_CYLINDER,    //3
    GLYPH_CUBE,        //4
    GLYPH_SPHERE,      //5
    GLYPH_SUPERQUADRIC //6
  };
  
  
  /** Set the input as a vtkStructuredPoints dataset. It is then
      mapped through a vtkExtractVOI to limit the visualization
      to one slice. */	
  void SetInput(vtkStructuredPoints* data, vtkMatrix4x4 *matrix = 0);
  
  /** Set the input as a vtkUnstructuredGrid dataset. It consists
      in a set of tensors and positions. Allows to display sparse
      tensors. */	
  void SetInput(vtkUnstructuredGrid* data, vtkMatrix4x4 *matrix = 0);
  
  /** Set the glyph shape */
  void SetGlyphShape (int i);

  /** Set the glyph shape */
  vtkGetMacro (ShapeMode, unsigned int);
  
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
  
  /** Get the inputed dataset. */
  vtkGetObjectMacro (Input, vtkDataSet);
  
  /** Get the freshly generated actor. */
  vtkGetObjectMacro (Actor, vtkActor);

  
  /** Get the freshly generated actor. */
  vtkGetObjectMacro (Normals, vtkPolyDataNormals);

  /** Set a scaling factor for the glyhs. */
  void SetGlyphScale(const float& f);
  double GetGlyphScale (void);
  
  
  /** Set a maximum size for the glyphs. The size
      is clamped to the maximum if it is over this
      value. */
  void SetMaxGlyphSize(const float& f);
  
  /** Set the Volume Of Interest (VOI). Consists in
      6 integers: xmin, xmax, ymin, ymax, zmin, zmax.*/
  void SetVOI(const int&,const int&,const int&,const int&,const int&,const int&);
  
  /** Set the sample rate. 1 over n tensors will be
      displaid.*/
  void SetSampleRate(const int&,const int&,const int&);
  
  /** Set the glyph resolution */
  void SetGlyphResolution (int res);
  vtkGetMacro(GlyphResolution, int);
  
  /** Set the scalar range for the LUT */
  void SetScalarRange(const float&, const float&);
  
  /** Enumeration of the different color code available. */
  enum
  {
    COLOR_BY_EIGENVECTOR,
    COLOR_BY_EIGENVALUE,
    COLOR_BY_VOLUME,
    COLOR_BY_TRACE,
    COLOR_BY_DISTANCE_TO_IDENTITY,
    COLOR_BY_SCALAR
  };
  
  
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
  
  /** Get the vtkPolyData. */
  vtkPolyData* GetPolyData(void) const
  { return this->Normals->GetOutput(); }
  
  /** Get the vtkMapper. */
  vtkGetObjectMacro (Mapper, vtkMapper);

  vtkGetObjectMacro (Glyph, vtkTensorGlyph);

  /** Force the update of the lookup table. */
  void UpdateLUT();
  
  /** Flip tensors along the X axis */
  void FlipX (bool a)
  { this->Fliper->SetFlipX (a); }
  
  /** Flip tensors along the Y axis */
  void FlipY (bool a)
  { this->Fliper->SetFlipY (a); }
  
  /** Flip tensors along the Z axis */
  void FlipZ (bool a)
  { this->Fliper->SetFlipZ (a); }
  
  /** Get the flipper */
  vtkGetObjectMacro (Fliper, vtkFlipTensorImageFilter);
  
  void SetLookupTable (vtkLookupTable* lut);

  vtkSetObjectMacro (Scalars, vtkDataArray);
  
 protected:
  
  vtkTensorVisuManager();
  ~vtkTensorVisuManager();
  
  /** Internal use only. */
  void SetUpLUTToMapEigenVector(void);
  void SetUpLUTToMapEigenValue(void);
  void SetUpLUTToMapVolume(void);
  void SetUpLUTToMapTrace(void);
  void SetUpLUTToMapDistanceToIdentity(void);
  void SetUpLUTToMapScalars(void);
  
 private:
  
  vtkFlipTensorImageFilter* Fliper;
  vtkExtractVOI*            VOI;
  vtkPolyDataAlgorithm*     Shape;
  vtkTensorGlyph*           Glyph;
  vtkPolyDataNormals*       Normals;
  vtkPolyDataMapper*        Mapper;
  vtkActor*                 Actor;  
  vtkLookupTable*           LUT;
  
  
  // scalars arrays
  vtkUnsignedIntArray* EigenvectorArray;
  vtkDoubleArray*      EigenvalueArray;
  vtkDoubleArray*      TraceArray;
  vtkDoubleArray*      VolumeArray;
  vtkDoubleArray*      DistanceArray;
  
  vtkDataArray*  Scalars;
  
  int            ColorMode;
  int            EigenNumber;
  GlyphShapeMode ShapeMode;
  
  vtkDataSet* Input;
  
  double Min;
  double Max;
  
  int GlyphResolution;
  
};


#endif
