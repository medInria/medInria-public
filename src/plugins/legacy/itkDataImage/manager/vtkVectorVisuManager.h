#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

// vtk includes
#include <vtkObject.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkGlyph3D.h>
#include <vtkGlyph3DMapper.h>
#include <vtkExtractVOI.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkUnsignedIntArray.h>
#include <vtkDoubleArray.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkVectorOrienter.h>
#include <vtkAssignAttribute.h>
#include <vtkPolydataNormalsOrienter.h>

class vtkMatrix4x4;

class vtkVectorVisuManager : public vtkObject
{

 public:

    enum ColorMode
    {
        ColorByVectorMagnitude = 0,
        ColorByVectorDirection,
        ColorByUserColor
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
  int* GetSampleRate(){return this->VOI->GetSampleRate();}

  /** Get the vtkGlyph3DMapper. */
  vtkGetObjectMacro (GlyphMapper, vtkGlyph3DMapper)

  void SetColorMode(ColorMode mode);
  ColorMode GetColorMode(){return this->CurrentColorMode;}

  void SetUserColor(double color[3]);
  const double* GetUserColor() {return UserColor;}

  void SetProjection(bool enable);
  bool GetProjection(){return this->Orienter->GetProjection();}

 protected:

  vtkVectorVisuManager();
  ~vtkVectorVisuManager();

  void SetUpLUTToMapVectorDirection();
  void SetColorByVectorMagnitude();
  void SetColorByUserColor();


 private:

  vtkExtractVOI*            VOI;
  vtkAssignAttribute *      Assign;
  vtkVectorOrienter*        Orienter;
  vtkGlyph3DMapper*         GlyphMapper;

  vtkActor*                 Actor;
  vtkPolyDataNormals*       Normals;

  vtkImageData*             Input;

  vtkDoubleArray*           ValueArray;

  ColorMode                 CurrentColorMode;
  double                    UserColor[3];
};



