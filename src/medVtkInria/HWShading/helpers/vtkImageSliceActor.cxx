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
 * vtkImageSliceActor.cxx
 * by Tim Peeters
 *
 * 2006-05-02	Tim Peeters
 * - First version. Some (a lot) of the code was derived from
 *   bmia::vtkDTISliceActor and vtkImageViewer2. But (hopefully) simplified.
 *
 * 2006-10-04	Tim Peeters
 * - Added double GetSliceLocation() function.
 *
 * 2007-02-21   Paulo Rodrigues
 * - bug with the dimensions of the planes (it was 1 voxel less)
 * - and now the voxel is around the point of the dataset => a glyph appears in the center of the voxel without moving it
 */

#include "vtkImageSliceActor.h"

#include <vtkActor.h>
#include <vtkExtractVOI.h>
#include <vtkImageData.h>
#include <vtkLookupTable.h>
#include <vtkMath.h>
#include <vtkObjectFactory.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkTexture.h>

//vtkCxxRevisionMacro(vtkImageSliceActor, "$Revision: 1.1.1.1 $");
vtkStandardNewMacro(vtkImageSliceActor);

vtkImageSliceActor::vtkImageSliceActor()
{
  this->Slice = 0;
  this->SliceOrientation = vtkImageSliceActor::SLICE_ORIENTATION_XY;

  this->ExtractVOI = vtkExtractVOI::New();
  this->Texture = vtkTexture::New();
  this->PlaneSource = vtkPlaneSource::New();
  this->Actor = vtkActor::New();

  vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();

  this->Texture->SetInputConnection(this->ExtractVOI->GetOutputPort());

/*
  vtkLookupTable* lut = vtkLookupTable::New();
  lut->SetSaturationRange(0.0, 0.0);
  lut->SetHueRange(0.0, 0.0);
  lut->SetValueRange(0.0, 1.0);
  lut->SetTableRange(0.0, 1.0);
  lut->Build();
  this->Texture->SetLookupTable(lut);
  lut->Delete();
  this->Texture->MapColorScalarsThroughLookupTableOff();
*/
//  this->Texture->GetLookupTable()->Print(cout);
  // this->Texture->SetLookupTable(..)?
  mapper->SetInputConnection(this->PlaneSource->GetOutputPort());
  this->Actor->SetMapper(mapper);
  this->Actor->SetTexture(this->Texture);

  mapper->Delete(); mapper = NULL;

  vtkProperty* property = this->Actor->GetProperty();
  property->SetAmbient(1.0); property->SetDiffuse(0.0); property->SetSpecular(0.0);
  property = NULL;

  this->AddPart(this->Actor);
}

vtkImageSliceActor::~vtkImageSliceActor()
{
  this->RemovePart(this->Actor);

  this->ExtractVOI->Delete(); this->ExtractVOI = NULL;
  this->Texture->Delete(); this->Texture = NULL;
//  this->Mapper->Delete(); this->Mapper = NULL;
  this->PlaneSource->Delete(); this->PlaneSource = NULL;
  this->Actor->Delete(); this->Actor = NULL;
}

void vtkImageSliceActor::GetSliceRange(int &min, int &max)
{
  vtkImageData *input = this->GetInput();
  if (input)
    {
    input->UpdateInformation();
    int *w_ext = input->GetWholeExtent();
    min = w_ext[this->SliceOrientation * 2];
    max = w_ext[this->SliceOrientation * 2 + 1];
    }
}

int* vtkImageSliceActor::GetSliceRange()
{
  vtkImageData *input = this->GetInput();
  if (input)
    {
    input->UpdateInformation();
    return input->GetWholeExtent() + this->SliceOrientation * 2;
    }
  return NULL;
}

int vtkImageSliceActor::GetSliceMin() 
{
  int *range = this->GetSliceRange();
  if (range)
    {
    return range[0];
    }
  return 0;
}

int vtkImageSliceActor::GetSliceMax() 
{
  int *range = this->GetSliceRange();
  if (range)
    {
    return range[1];
    }
  return 0;
}

void vtkImageSliceActor::SetSlice(int slice)
{
  int *range = this->GetSliceRange();
  if (range)
    {
//cout<<"--------------------------------------------------------------------------"<<endl;
//cout<<"slice range = "<<range[0]<< " - "<<range[1]<<endl;
    if (slice < range[0])
      {
      slice = range[0];
      }
    else if (slice > range[1])
      {
      slice = range[1];
      }
    }

  if (this->Slice == slice)
    {
    return;
    }

  this->Slice = slice;
  this->Modified();

  this->UpdateDisplayExtent();
}

void vtkImageSliceActor::SetSliceOrientation(int orientation)
{
  if (orientation < vtkImageSliceActor::SLICE_ORIENTATION_YZ ||
      orientation > vtkImageSliceActor::SLICE_ORIENTATION_XY)
    {
    vtkErrorMacro("Error - invalid slice orientation " << orientation);
    return;
    }
  
  if (this->SliceOrientation == orientation)
    {
    return;
    }

  this->SliceOrientation = orientation;

  // Update the viewer 

  int *range = this->GetSliceRange();
  if (range)
    {
    this->Slice = static_cast<int>((range[0] + range[1]) * 0.5);
    }

  this->UpdateDisplayExtent();
}

void vtkImageSliceActor::SetInput(vtkImageData *in) 
{
  this->ExtractVOI->SetInput(in);
  this->UpdateDisplayExtent();
}

vtkImageData* vtkImageSliceActor::GetInput()
{ 
  return vtkImageData::SafeDownCast(this->ExtractVOI->GetInput());
}

void vtkImageSliceActor::SetInputConnection(vtkAlgorithmOutput* input) 
{
  vtkDebugMacro(<<"Setting input connection to "<<input);
  this->ExtractVOI->SetInputConnection(input);
  this->UpdateDisplayExtent();
};

void vtkImageSliceActor::CenterSlice()
{
  vtkImageData* input = this->GetInput();
  if (!input)
    {
    return;
    }

  int *w_ext = input->GetWholeExtent();
//cout<<"===================================================================================================================="<<endl;
//cout<<"w_ext = "<<w_ext[0]<<", "<<w_ext[1]<<", "<<w_ext[2]<<", "<<w_ext[3]<<", "<<w_ext[4]<<", "<<w_ext[5]<<endl;
  int slice_min = w_ext[this->SliceOrientation * 2];
//cout<<"SliceOrientation = "<<SliceOrientation<<endl;
//cout<<"slice_min = "<<slice_min<<endl;
  int slice_max = w_ext[this->SliceOrientation * 2 + 1];
//cout<<"slice_max = "<<slice_max<<endl;
  int center_slice = static_cast<int>((slice_min + slice_max) * 0.5);
//cout<<"center_slice = "<<center_slice<<endl;
  this->SetSlice(center_slice);
}

/**
 *	(paulo)
 *  BUG: if we have a a 128x128x30 dataset, with {1.8,1.8,3.0} as the spacing
 *       the planes should have 128*1.8=230.4 and not 228.6 as the bounds say.
 *		 Corrected the drawing of the plane so 1 more voxel is added to its
 *		 size.
 */
void vtkImageSliceActor::UpdateDisplayExtent()
{
  vtkDebugMacro(<<"Updating display extent...");
  vtkImageData *input = this->GetInput();
  if (!input)
    {
    return;
    }

  input->UpdateInformation();
  //input->Update(); // commented out because the input filename may not have been set yet.
  int *w_ext = input->GetWholeExtent();

  // Is the slice in range? If not, fix it
  int slice_min = w_ext[this->SliceOrientation * 2];
  int slice_max = w_ext[this->SliceOrientation * 2 + 1];
  if (this->Slice < slice_min || this->Slice > slice_max)
    {
    this->Slice = static_cast<int>((slice_min + slice_max) * 0.5);
    }

  double bounds[6]; input->GetBounds(bounds);
  double spacing[3]; input->GetSpacing(spacing);

  //paulo: to correct the planes size (1 voxel is missing on their sizes)
  // TODO: still problems with the Z coordinate?!
	bounds[1] += spacing[0];
	bounds[3] += spacing[1];
	bounds[5] += spacing[2];

  vtkDebugMacro(<<"Input bounds are "<<bounds[0]<<", "<<bounds[1]<<", "<<bounds[2]
	<<", "<<bounds[3]<<", "<<bounds[4]<<", "<<bounds[5]);
  vtkDebugMacro(<<"Input spacing is "<<spacing[0]<<", "<<spacing[1]<<", "<<spacing[2]);


  //int* dims = input->GetDimensions();
//	cout<<" Dimensions are "<<dims[0]<<", "<<dims[1]<<", "<<dims[2]<<"."<<endl;
//   cout<<"Input bounds are "<<bounds[0]<<", "<<bounds[1]<<", "<<bounds[2]
//       <<", "<<bounds[3]<<", "<<bounds[4]<<", "<<bounds[5]<<endl;
//  cout<<"Input spacing is "<<spacing[0]<<", "<<spacing[1]<<", "<<spacing[2]<<endl;
//   cout<<"w_ext are "<<w_ext[0]<<", "<<w_ext[1]<<", "<<w_ext[2]
//       <<", "<<w_ext[3]<<", "<<w_ext[4]<<", "<<w_ext[5]<<endl;

  //cout<<"bounds should be"<<bounds[0]<<", "<<bounds[1]+spacing[0]<<", "<<bounds[2]
  //<<", "<<bounds[3]+spacing[1]<<", "<<bounds[4]<<", "<<bounds[5]+spacing[2]<<endl;
  
  if (!vtkMath::AreBoundsInitialized(bounds))
    {
    int* dims = input->GetDimensions();
    (void)dims;
    vtkDebugMacro(<<"Bounds are not initialized!"
	<<" Dimensions are "<<dims[0]<<", "<<dims[1]<<", "<<dims[2]<<".");
    // Input is not up-to-date. Pipeline must be updated first, so return.
    return;
    } // if

  // XXX: is this correct is slice_min != 0?
  double sliceloc = spacing[this->SliceOrientation]*(double)(this->Slice - slice_min);
  //cout<<"sliceloc = "<<sliceloc<<endl;
  double* cent=NULL;
  // Set the image actor
  switch (this->SliceOrientation)
    {
    case vtkImageSliceActor::SLICE_ORIENTATION_XY:
      this->ExtractVOI->SetVOI(
	w_ext[0], w_ext[1], w_ext[2], w_ext[3], this->Slice, this->Slice);
        this->PlaneSource->SetOrigin(bounds[0], bounds[2], sliceloc);
        this->PlaneSource->SetPoint1(bounds[1], bounds[2], sliceloc);
        this->PlaneSource->SetPoint2(bounds[0], bounds[3], sliceloc);

		cent = this->PlaneSource->GetCenter();
		this->PlaneSource->SetCenter(cent[0]-spacing[0]/2.0, cent[1]-spacing[1]/2.0, cent[2]);
      break;

    case vtkImageSliceActor::SLICE_ORIENTATION_XZ:
      this->ExtractVOI->SetVOI(
        w_ext[0], w_ext[1], this->Slice, this->Slice, w_ext[4], w_ext[5]);
        this->PlaneSource->SetOrigin(bounds[0], sliceloc, bounds[4]);
        this->PlaneSource->SetPoint1(bounds[1], sliceloc, bounds[4]);
        this->PlaneSource->SetPoint2(bounds[0], sliceloc, bounds[5]);

		cent = this->PlaneSource->GetCenter();
		this->PlaneSource->SetCenter(cent[0]-spacing[0]/2.0, cent[1], cent[2]-spacing[2]/2.0);
      break;

    case vtkImageSliceActor::SLICE_ORIENTATION_YZ:
      this->ExtractVOI->SetVOI(
        this->Slice, this->Slice, w_ext[2], w_ext[3], w_ext[4], w_ext[5]);
        this->PlaneSource->SetOrigin(sliceloc, bounds[2], bounds[4]);
        this->PlaneSource->SetPoint1(sliceloc, bounds[3], bounds[4]);
        this->PlaneSource->SetPoint2(sliceloc, bounds[2], bounds[5]);

		cent = this->PlaneSource->GetCenter();
		this->PlaneSource->SetCenter(cent[0], cent[1]-spacing[1]/2.0, cent[2]-spacing[2]/2.0);
      break;
    }
    
//cout << "there should be this many squares:"<<(bounds[1] - bounds[0])/spacing[0] << endl;
    
}

double vtkImageSliceActor::GetSliceLocation()
{
  if (!this->GetInput()) return 0.0;

  int *w_ext = this->GetInput()->GetWholeExtent();
  int slice_min = w_ext[this->SliceOrientation * 2];
  w_ext = NULL;

//  double bounds[6]; input->GetBounds(bounds);
  double spacing[3]; this->GetInput()->GetSpacing(spacing);

  /*
  vtkDebugMacro(<<"Input bounds are "<<bounds[0]<<", "<<bounds[1]<<", "<<bounds[2]
	<<", "<<bounds[3]<<", "<<bounds[4]<<", "<<bounds[5]);
  vtkDebugMacro(<<"Input spacing is "<<spacing[0]<<", "<<spacing[1]<<", "<<spacing[2]);

  if (!vtkMath::AreBoundsInitialized(bounds))
    {
    int* dims = input->GetDimensions();
    vtkDebugMacro(<<"Bounds are not initialized!"
	<<" Dimensions are "<<dims[0]<<", "<<dims[1]<<", "<<dims[2]<<".");
    // Input is not up-to-date. Pipeline must be updated first, so return.
    return;
    } // if
*/
  // XXX: is this correct is slice_min != 0? --> NO?
  double sliceloc = spacing[this->SliceOrientation]*(double)(this->Slice - slice_min);

  return sliceloc;
}

/*
int vtkImageSliceActor::RenderTranslucentGeometry(vtkViewport *viewport)
{
  vtkDebugMacro(<< "vtkImageSliceActor::RenderTranslucentGeometry");
  this->vtkAssembly::RenderTranslucentGeometry(viewport);
}

int vtkImageSliceActor::RenderOpaqueGeometry(vtkViewport *viewport)
{
  vtkDebugMacro(<< "vtkImageSliceActor::RenderOpaqueGeometry");

  vtkImageData *input = this->GetInput();
  if (!input)
    {
    return 0;
    }
  // make sure the data is available
  input->UpdateInformation();
  input->Update();

  this->vtkAssembly::RenderOpaqueGeometry(viewport);
}
*/

void vtkImageSliceActor::UpdateInput()
{
  vtkDebugMacro(<< "vtkImageSliceActor::UpdateInput");
  vtkImageData *input = this->GetInput();
  if (!input)
    {
    vtkErrorMacro(<<"No input!");
    return;
    }

  //int* voi = this->ExtractVOI->GetVOI();
//  cout<<this->ExtractVOI<<" VOI = "<<voi[0]<<", "<<voi[1]<<", "<<voi[2]<<", "<<voi[3]<<", "<<voi[4]<<", "<<voi[5]<<endl;

  this->ExtractVOI->SetInput(NULL);
  // make sure the data is available
  vtkDebugMacro(<<"Updating input dataset");
  input->UpdateInformation();
  input->Update();
  this->ExtractVOI->SetInput(input);

  this->UpdateDisplayExtent();
  // XXX: CenterSlice instead of UpdateDisplayExtent?
}

double* vtkImageSliceActor::GetPlaneNormal()
{
  return this->PlaneSource->GetNormal();
}

void vtkImageSliceActor::GetPlaneNormal(double data[3])
{
  this->PlaneSource->GetNormal(data);
}

double* vtkImageSliceActor::GetPlaneCenter()
{
  return this->PlaneSource->GetCenter();
}

void vtkImageSliceActor::GetPlaneCenter(double data[3])
{
  this->PlaneSource->GetCenter(data);
}

