/*============================================================================

The Hardware Shading (HWShading) module is protected by the
following copyright:
Copyright (c) 2007 Biomedical Image Analysis (BMIA) - Department of
Biomedical Engineering - Eindhoven University of Technology.
All rights reserved. See Copyright.txt for details.
The HWShading implementation was originally written by Tim Peeters (BMIA - TUe)
and published at the "11th International Fall Workshop on Vision, Modeling,
and Visualization 2006" (VMV'06):
"Visualization of the Fibrous Structure of the Heart", by T. Peeters et al.
See http://timpeeters.com/research/vmv2006 for more information.
This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.
============================================================================*/
/**
 * vtkImageSliceActor.h
 * by Tim Peeters
 *
 * 2006-05-02	Tim Peeters
 * - First version
 *
 * 2006-10-04	Tim Peeters
 * - Added double GetSliceLocation() function.
 */

#pragma once

#include "medVtkInriaExport.h"

#include <vtkAssembly.h>

class vtkActor;
class vtkAlgorithmOutput;
class vtkExtractVOI;
class vtkImageData;
class vtkPlaneSource;
//class vtkTexture;
#include <vtkTexture.h> // for forwarding some function calls to this->Texture
#include <vtkLookupTable.h>

/**
 * Convenience class for showing a slice of a 3D volume.
 * This is done by taking a slice of the volume and mapping it onto a plane
 * using a 2D vtkTexture. Input may be mapped trough a lookup table
 * or unsinged chars can be used directly as a texture.
 */
class MEDVTKINRIA_EXPORT vtkImageSliceActor : public vtkAssembly
{
public:
  static vtkImageSliceActor* New();
//  vtkTypeRevisionMacro(vtkImageSliceActor,vtkAssembly); // gives a vtable link error..:S
//  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set/Get the input image to the actor.
  virtual void SetInput(vtkImageData *in);
  virtual vtkImageData *GetInput();
  virtual void SetInputConnection(vtkAlgorithmOutput* input);

  // Description:
  // Set/Get the lookup table to map the data through.
  virtual void SetLookupTable(vtkLookupTable* lut)
    { this->Texture->SetLookupTable(lut); };
  virtual vtkLookupTable* GetLookupTable()
  { return vtkLookupTable::SafeDownCast(this->Texture->GetLookupTable()); };

  // Description:
  // Turn on/off the mapping of color scalars through the lookup table.
  // The default is Off. If Off, unsigned char scalars will be used
  // directly as texture. If On, scalars will be mapped through the
  // lookup table to generate 4-component unsigned char scalars.
  // This ivar does not affect other scalars like unsigned short, float,
  // etc. These scalars are always mapped through lookup tables.
  virtual int GetMapColorScalarsThroughLookupTable()
    { return this->Texture->GetMapColorScalarsThroughLookupTable(); };
  virtual void SetMapColorScalarsThroughLookupTable(int map)
    { this->Texture->SetMapColorScalarsThroughLookupTable(map); };
  virtual void MapColorScalarsThroughLookupTableOn()
    { this->Texture->MapColorScalarsThroughLookupTableOn(); };
  virtual void MapColorScalarsThroughLookupTableOff()
    { this->Texture->MapColorScalarsThroughLookupTableOff(); };

  // Description:
  // Turn on/off linear interpolation of the texture map when rendering.
  virtual int GetInterpolate()
    { return this->Texture->GetInterpolate(); };
  virtual void SetInterpolate(int interpolate)
    { this->Texture->SetInterpolate(interpolate); };
  virtual void InterpolateOn()
    { this->Texture->InterpolateOn(); };
  virtual void InterpolateOff()
    { this->Texture->InterpolateOff(); };

  // Description:
  // Set/get the slice orientation
  //BTX
  enum
  {
    SLICE_ORIENTATION_YZ = 0,
    SLICE_ORIENTATION_XZ = 1,
    SLICE_ORIENTATION_XY = 2
  };
  //ETX
  vtkGetMacro(SliceOrientation, int);
  virtual void SetSliceOrientation(int orientation);
  virtual void SetOrientation(int orientation)
    { this->SetSliceOrientation(orientation); }
  virtual void SetSliceOrientationToXY()
    { this->SetSliceOrientation(vtkImageSliceActor::SLICE_ORIENTATION_XY); };
  virtual void SetSliceOrientationToYZ()
    { this->SetSliceOrientation(vtkImageSliceActor::SLICE_ORIENTATION_YZ); };
  virtual void SetSliceOrientationToXZ()
    { this->SetSliceOrientation(vtkImageSliceActor::SLICE_ORIENTATION_XZ); };

  // Description:
  // Set/Get the current slice to display (depending on the orientation
  // this can be in X, Y or Z).
  vtkGetMacro(Slice, int);
  virtual void SetSlice(int s);

  /**
   * Return the location of the current slice.
   */
  virtual double GetSliceLocation();

  // Description:
  // Set the current slice to the center of the dataset in the current orientation.
  virtual void CenterSlice();
  
  // Description:
  // Return the minimum and maximum slice values (depending on the orientation
  // this can be in X, Y or Z).
  virtual int GetSliceMin();
  virtual int GetSliceMax();
  virtual void GetSliceRange(int range[2])
    { this->GetSliceRange(range[0], range[1]); }
  virtual void GetSliceRange(int &min, int &max);
  virtual int* GetSliceRange();

  // Description:
  // Get the center/normal of the plane that is being rendered.
  // Forwards these calls to the PlaneSource that is used.
  virtual double* GetPlaneNormal();
  virtual void GetPlaneNormal(double data[3]);
  virtual double* GetPlaneCenter();
  virtual void GetPlaneCenter(double data[3]);

  // Description:
  // Re-implemented from vtkAssembly. These functions make sure the data information
  // is up-to-date and call the corresponding superclass functions.
  //virtual int RenderTranslucentGeometry(vtkViewport *viewport);
  //virtual int RenderOpaqueGeometry(vtkViewport *viewport);

  virtual void UpdateInput();

protected:
  vtkImageSliceActor();
  ~vtkImageSliceActor();

  int Slice;
  int SliceOrientation;
  vtkExtractVOI* ExtractVOI;
  vtkTexture* Texture;
  vtkPlaneSource* PlaneSource;
  vtkActor* Actor;

  virtual void UpdateDisplayExtent();

private:
  vtkImageSliceActor(const vtkImageSliceActor&);  // Not implemented.
  void operator=(const vtkImageSliceActor&);  // Not implemented.
}; // vtkImageSliceActor


