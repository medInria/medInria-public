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

// vtk includes
#include <vtkObject.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkGlyph3D.h>
#include <vtkExtractVOI.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkUnsignedIntArray.h>
#include <vtkDoubleArray.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkVectorOrienter.h>
#include <vtkAssignAttribute.h>

class vtkMatrix4x4;

class VTK_VISUMANAGEMENT_EXPORT vtkVectorVisuManager : public vtkObject
{

 public:

    enum ScaleMode
    {
        ScaleByScalar = 0,
        ScaleByVector,
        ScaleByVectorComponents,
        ScalingOff
    };

    enum ColorMode
    {
        ColorByScalar = 0,
        ColorByScale,
        ColorByVector
    };

    enum ViewOrientation
    {
        axial,
        sagittal,
        coronal
    };

  static vtkVectorVisuManager *New();

  void SetInput(vtkImageData* data, vtkMatrix4x4 *matrix);

  /** Set the Volume Of Interest (VOI). Consists in
      6 integers: xmin, xmax, ymin, ymax, zmin, zmax.*/
  void SetVOI(const int&,const int&,const int&,const int&,const int&,const int&);

  /** Get the inputed dataset. */
  vtkGetObjectMacro (Input, vtkImageData)

  /** Get the freshly generated actor. */
  vtkGetObjectMacro (Actor, vtkActor)

  /** Set a scaling factor for the glyhs. */
  void SetGlyphScale(const float& f);
  double GetGlyphScale();

  /** Set the sample rate. 1 over n Vectors will be
      displaid.*/
  void SetSampleRate(const int&,const int&,const int&);

  /** Get the vtkMapper. */
  vtkGetObjectMacro (Mapper, vtkMapper)

  /** Get the vtkGlyph3D. */
  vtkGetObjectMacro (Glyph, vtkGlyph3D)

  void SetScaleMode(ScaleMode mode);
  void SetColorMode(ColorMode mode);

  void SetProjection(bool enable);

 protected:

  vtkVectorVisuManager();
  ~vtkVectorVisuManager();


 private:

  vtkExtractVOI*            VOI;
  vtkAssignAttribute *      Assign;
  vtkVectorOrienter*        Orienter;
  vtkGlyph3D*               Glyph;
  vtkPolyDataMapper*        Mapper;
  vtkActor*                 Actor;

  vtkImageData*             Input;

};



