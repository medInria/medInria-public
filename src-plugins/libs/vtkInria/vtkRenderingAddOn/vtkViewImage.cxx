/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkRenderingAddOn/vtkViewImage.h>

#include "vtkObjectFactory.h"

#include "vtkProp.h"
#include "vtkTextActor.h"
#include "vtkCoordinate.h"
#include "vtkProperty.h"
#include "vtkProperty2D.h"
#include "vtkTextProperty.h"
#include "vtkScalarsToColors.h"
#include "vtkImageMapToWindowLevelColors.h"
#include "vtkImageData.h"
#include "vtkActor.h"
#include "vtkRenderWindow.h"

#include <vtkColorTransferFunction.h>
#include <vtkDataArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkExtractGeometry.h>
#include <vtkDataSet.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>
#include <vtkPlane.h>
#include <vtkPlaneCollection.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkActor2D.h>
#include <vtkClipDataSet.h>
#include <vtkCutter.h>
#include <vtkBox.h>
#include <vtkPolyDataWriter.h>
#include <vtkPolyDataNormals.h>
#include <vtkImageAccumulate.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkMath.h>

#include <itkImageToVTKImageFilter.h>
#include <itkFlipImageFilter.h>
#include <itkExtractImageBufferFilter.h>
#include <itkImage.h>
#include <itkMatrix.h>
#include <vnl/algo/vnl_qr.h>

#include <assert.h>
#include <iostream>
#include <sstream>
#include <cmath>

vtkCxxRevisionMacro(vtkViewImage, "$Revision: 1315 $");
vtkStandardNewMacro(vtkViewImage);



// Enumeration for the supported pixel types
namespace {
    enum ViewImageType {
        IMAGE_VIEW_NONE = 0,
        IMAGE_VIEW_DOUBLE = 1,
        IMAGE_VIEW_FLOAT,
        IMAGE_VIEW_INT,
        IMAGE_VIEW_UNSIGNED_INT,
        IMAGE_VIEW_SHORT,
        IMAGE_VIEW_UNSIGNED_SHORT,
        IMAGE_VIEW_LONG,
        IMAGE_VIEW_UNSIGNED_LONG,
        IMAGE_VIEW_CHAR,
        IMAGE_VIEW_UNSIGNED_CHAR,
        IMAGE_VIEW_RGBPIXELTYPE,
        IMAGE_VIEW_RGBAPIXELTYPE,
        IMAGE_VIEW_UCHARVECTOR3TYPE,
        IMAGE_VIEW_FLOATVECTOR3TYPE};
}

// pIMPL class for vtkViewImage
class vtkViewImage::vtkViewImageImplementation {
public:
    //! Default constructor
    vtkViewImageImplementation () : ImageConverter (0), ImageTemporalFilter (0), TemporalFilterType (IMAGE_VIEW_NONE) {}
    
    itk::ProcessObject::Pointer ImageConverter;
    itk::ProcessObject::Pointer ImageTemporalFilter;
    int TemporalFilterType;
    
    public :
    template < typename T > static ViewImageType GetViewImageType () ;
public:
};

// Template specialisations return the enumeration corresponding to the pixel type.
template <> ViewImageType vtkViewImage::vtkViewImageImplementation::GetViewImageType < double > () { return     IMAGE_VIEW_DOUBLE ; }
template <> ViewImageType vtkViewImage::vtkViewImageImplementation::GetViewImageType < float > () { return     IMAGE_VIEW_FLOAT; }
template <> ViewImageType vtkViewImage::vtkViewImageImplementation::GetViewImageType < int > () { return     IMAGE_VIEW_INT; }
template <> ViewImageType vtkViewImage::vtkViewImageImplementation::GetViewImageType < unsigned int > () { return     IMAGE_VIEW_UNSIGNED_INT; }
template <> ViewImageType vtkViewImage::vtkViewImageImplementation::GetViewImageType < short > () { return     IMAGE_VIEW_SHORT; }
template <> ViewImageType vtkViewImage::vtkViewImageImplementation::GetViewImageType < unsigned short > () { return     IMAGE_VIEW_UNSIGNED_SHORT; }
template <> ViewImageType vtkViewImage::vtkViewImageImplementation::GetViewImageType < long > () { return     IMAGE_VIEW_LONG; }
template <> ViewImageType vtkViewImage::vtkViewImageImplementation::GetViewImageType < unsigned long > () { return     IMAGE_VIEW_UNSIGNED_LONG; }
template <> ViewImageType vtkViewImage::vtkViewImageImplementation::GetViewImageType < char > () { return     IMAGE_VIEW_CHAR; }
template <> ViewImageType vtkViewImage::vtkViewImageImplementation::GetViewImageType < unsigned char > () { return     IMAGE_VIEW_UNSIGNED_CHAR; }
template <> ViewImageType vtkViewImage::vtkViewImageImplementation::GetViewImageType < vtkViewImage::RGBPixelType > () { return     IMAGE_VIEW_RGBPIXELTYPE; }
template <> ViewImageType vtkViewImage::vtkViewImageImplementation::GetViewImageType < vtkViewImage::RGBAPixelType > () { return     IMAGE_VIEW_RGBAPIXELTYPE; }
template <> ViewImageType vtkViewImage::vtkViewImageImplementation::GetViewImageType < vtkViewImage::UCharVector3Type > () { return     IMAGE_VIEW_UCHARVECTOR3TYPE ; }
template <> ViewImageType vtkViewImage::vtkViewImageImplementation::GetViewImageType < vtkViewImage::FloatVector3Type > () { return     IMAGE_VIEW_FLOATVECTOR3TYPE ; }


vtkViewImage::vtkViewImage()
{

  this->Image       = 0;
  this->MaskImage   = 0;
  this->OverlappingImage = 0;
  this->MaskLUT     = 0;
  this->Transform   = 0;
  this->LookupTable = 0;
  this->LinkWindowLevel = 1;
  this->LinkPosition    = 1;
  this->LinkZoom        = 0;
  this->LinkVolumeIndex = 0;    
  this->FirstImage      = 1;
  this->FirstResetWindowLevel = 1;

  this->Visibility = 1;
  this->SizeDataVisibility = 1;
  
  this->Shift = 0.0;
  this->Scale = 1.0;
  this->Level = 128.0;
  this->Window = 255.0;
  this->LevelForReset = 128.0;
  this->WindowForReset = 255.0;
  this->Zoom   = 1.0;

  this->ScalarBar = vtkScalarBarActor::New();
  this->ScalarBar->GetLabelTextProperty()->SetColor (1.0,1.0,1.0);
  this->ScalarBar->GetTitleTextProperty()->SetColor (1.0,1.0,1.0);
  //this->ScalarBar->SetTextPositionToPrecedeScalarBar();
  this->ScalarBar->GetLabelTextProperty()->BoldOff();
  this->ScalarBar->GetLabelTextProperty()->ShadowOff();
  this->ScalarBar->GetLabelTextProperty()->ItalicOff();
  this->ScalarBar->SetNumberOfLabels (3);
  this->ScalarBar->GetLabelTextProperty()->SetFontSize (1);
  this->ScalarBar->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
  this->ScalarBar->SetWidth (0.1);
  this->ScalarBar->SetHeight (0.5);
  this->ScalarBar->SetPosition (0.9,0.3);
  //this->ScalarBar->GetPositionCoordinate()->SetValue ( 0.1, 0.01);
  this->ScalarBar->PickableOff();
  this->ScalarBar->VisibilityOff();

  vtkLookupTable* bwLut = vtkLookupTable::New();
  bwLut->SetTableRange (0, 1);
  bwLut->SetSaturationRange (0, 0);
  bwLut->SetHueRange (0, 0);
  bwLut->SetValueRange (0, 1);
  bwLut->SetAlphaRange (0, 1);
  bwLut->Build();
  this->SetLookupTable (bwLut);
  bwLut->Delete();

  this->CurrentPoint[0] = 0.0;
  this->CurrentPoint[1] = 0.0;
  this->CurrentPoint[2] = 0.0;

  this->ResetWindowLevelMode = UserDefinedPercentage;
  this->WindowLevelPercentage = 0.1;

  // Increase polygon offsets to support some OpenGL drivers
  //vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();
  //vtkMapper::SetResolveCoincidentTopologyPolygonOffsetParameters(10,10);

  this->Impl = new vtkViewImageImplementation;    
  this->ITKImage  = 0;
  this->ITKImage4 = 0;    

  this->DirectionMatrix = vtkMatrix4x4::New();
  this->DirectionMatrix->Identity();
    
  this->VolumeIndex = 0;
}


vtkViewImage::~vtkViewImage()
{
  if( this->Transform )
  {
    this->Transform->Delete();
  }

  if( this->Image )
  {
    this->Image->Delete();
  }


  this->ScalarBar->Delete();

  this->RemoveAllDataSet();

  this->ITKImage = 0;
  this->ITKImage4 = 0;    
    
  delete this->Impl;

  this->DirectionMatrix->Delete();

}


void vtkViewImage::Initialize()
{
  vtkSynchronizedView::Initialize();
  this->SetLookupTable( this->GetLookupTable() );
  this->AddActor (this->ScalarBar);
}


void vtkViewImage::Uninitialize()
{
  this->RemoveActor (this->ScalarBar);
  vtkSynchronizedView::Uninitialize();
}


void vtkViewImage::RegisterImage(vtkImageData* image)
{
  if(!image)  return;

  if( image != this->Image )
  {
    if( this->Image != NULL )
    {
      this->Image->UnRegister (this);
    }

    this->Image = image;
    this->Image->Register (this);
  }
}


double vtkViewImage::GetWholeMinPosition(unsigned int p_axis)
{
  if(!this->Image)
  {
    return -VTK_LARGE_FLOAT;
  }

  return this->Image->GetBounds()[p_axis * 2];
}


double vtkViewImage::GetWholeMaxPosition(unsigned int p_axis)
{
  if(!this->Image)
  {
    return VTK_LARGE_FLOAT;
  }

  return this->Image->GetBounds()[p_axis * 2 + 1];
}


unsigned int vtkViewImage::GetOrthogonalAxis(unsigned int p_plan)
{
  assert(p_plan<NB_DIRECTION_IDS);

  int axis = 0;
  switch(p_plan)
  {
      case SAGITTAL_ID:
        axis = X_ID;
        break;

      case CORONAL_ID:
        axis = Y_ID;
        break;

      case AXIAL_ID:

        axis = Z_ID;
        break;
  }
  
  double axisVec[4] = {0.0, 0.0, 0.0, 0.0};
  axisVec[axis] = 1.0;

  vtkMatrix4x4 *t_directions = vtkMatrix4x4::New();
  vtkMatrix4x4::Transpose (this->GetDirectionMatrix(), t_directions);
  
  //this->GetDirectionMatrix()->MultiplyPoint (axisVec, axisVec);
  t_directions->MultiplyPoint (axisVec, axisVec);
  t_directions->Delete();
  
  for (int i=0; i<3; i++)
    if (fabs (axisVec[i])>0.0 )
    {
      axis = i;
      break;
    }

  /*
  switch(p_plan)
  {
      case SAGITTAL_ID:
        return X_ID;
        break;

      case CORONAL_ID:
        return Y_ID;
        break;

      case AXIAL_ID:

        return Z_ID;
        break;
  }
  return 0;
  */
  return axis;
}


int vtkViewImage::GetSliceForPosition(const double pos[3], unsigned int p_plan)
{

  if( !this->GetImage() )
  {
    return 0;
  }

  assert(p_plan < NB_DIRECTION_IDS);

  double*       spacing = this->GetImage()->GetSpacing();
  double*       origin  = this->GetImage()->GetOrigin();

  int   axis     = this->GetOrthogonalAxis(p_plan);
  double  soft_pos = pos[axis];
  
  // Treat extreme position at the end of the last pixel
  /*
    double  pos_max  = this->GetWholeMaxPosition(axis);
    double  pos_min  = this->GetWholeMinPosition(axis);
    
    if ((soft_pos > pos_max-0.005) && (soft_pos < pos_max+0.005))
    {
    soft_pos = pos_max-0.005;
    }
    if ((soft_pos > pos_min-0.005) && (soft_pos < pos_min+0.005))
    {
    soft_pos = pos_min+0.005;
    }
  */
  return (int)vtkMath::Round((soft_pos-origin[axis])/spacing[axis]);
}


int vtkViewImage::GetSlice(unsigned int p_plan)
{
  const double* pos = this->GetCurrentPoint();
  return this->GetSliceForPosition (pos, p_plan);
}


void vtkViewImage::SetSlice (unsigned int p_plan, int p_zslice)
{
  double pos[3];
  this->GetPositionForSlice (p_zslice, p_plan, pos);
  this->SetCurrentPoint(pos);
}


void vtkViewImage::GetPositionForSlice (int p_zslice, int orientation, double pos[3])
{
  
  if( !this->GetImage() )
  {
    return;
  }

  unsigned int axis = this->GetOrthogonalAxis(orientation);

  
  double* spacing = this->GetImage()->GetSpacing();
  int*    extent  = this->GetImage()->GetWholeExtent();
  double* origin  = this->GetImage()->GetOrigin();
  int     slice   = p_zslice;

  if(slice > extent[2*axis+1])  slice = extent[2*axis+1];
  if(slice < extent[2*axis])    slice = extent[2*axis];
  
  this->GetCurrentPoint(pos);
  pos[axis] = origin[axis] + slice * spacing[axis];
}


void vtkViewImage::SyncSetSlice(unsigned int p_plan, int p_zslice)
{
  if( this->IsLocked() )
  {
    return;
  }

  if (this->GetLinkPosition())
  {
    this->SetSlice(p_plan, p_zslice);
  }
  
  this->Lock();
  for( unsigned int i=0; i<this->Children.size(); i++)
  {
    vtkViewImage* view = vtkViewImage::SafeDownCast (this->Children[i]);
    if( view /*&& view->GetLinkPosition()*/)
    {
      view->SyncSetSlice (p_plan, p_zslice);
      view->Update();
    }
  }
  this->UnLock();
}


void vtkViewImage::SyncSetZSlice(int p_zslice)
{
  if( this->IsLocked() )
  {
    return;
  }

  if (this->GetLinkPosition())
  {
    this->SetZSlice(p_zslice);
  }

  this->Lock();
  for( unsigned int i=0; i<this->Children.size(); i++)
  {
    vtkViewImage* view = vtkViewImage::SafeDownCast (this->Children[i]);
    if( view /*&& view->GetLinkPosition()*/)
    {
      view->SyncSetZSlice (p_zslice);
      view->Update();
    }
  }
  this->UnLock();
}


void vtkViewImage::SetCurrentPoint (const double p_point[3])
{
  this->CurrentPoint[0] = p_point[0];
  this->CurrentPoint[1] = p_point[1];
  this->CurrentPoint[2] = p_point[2];
  this->UpdatePosition();
  this->InvokeEvent (vtkViewImage::ViewImagePositionChangeEvent, NULL);
}


void vtkViewImage::GetVoxelCoordinates(double pos[3], int p_coordinates[3])
{
  if(!this->Image)
  {
    return;
  }
  p_coordinates[this->GetOrthogonalAxis(SAGITTAL_ID)] = this->GetSliceForPosition(pos, SAGITTAL_ID);
  p_coordinates[this->GetOrthogonalAxis(CORONAL_ID)]  = this->GetSliceForPosition(pos, CORONAL_ID);
  p_coordinates[this->GetOrthogonalAxis(AXIAL_ID)]    = this->GetSliceForPosition(pos, AXIAL_ID);
}


void vtkViewImage::GetCurrentVoxelCoordinates(int p_coordinates[3])
{
  p_coordinates[this->GetOrthogonalAxis(SAGITTAL_ID)] = this->GetSlice(SAGITTAL_ID);
  p_coordinates[this->GetOrthogonalAxis(CORONAL_ID)]  = this->GetSlice(CORONAL_ID);
  p_coordinates[this->GetOrthogonalAxis(AXIAL_ID)]    = this->GetSlice(AXIAL_ID);
}


void vtkViewImage::SyncSetCurrentPoint(const double p_point[3])
{

  if( this->IsLocked() )
  {
    return;
  }

  if (this->GetLinkPosition())
  {
    this->SetCurrentPoint (p_point);
  }

  this->Lock();

  for( unsigned int i=0; i<this->Children.size(); i++)
  {
    vtkViewImage* view = vtkViewImage::SafeDownCast( this->Children[i] );
    if( view /*&& view->GetLinkPosition()*/)
    {
      view->SyncSetCurrentPoint (p_point);
      view->Update();
    }
  }

  this->UnLock();

  

}


double vtkViewImage::GetCurrentPointDoubleValue ()
{
  if (!this->Image)
  {
    return -1;
  }

  int coordinates[3];
  this->GetCurrentVoxelCoordinates(coordinates);
  
  void *scalar = this->GetImage()->GetScalarPointer(coordinates);

  if( !scalar ) return -1.0;

  switch (this->GetImage()->GetScalarType()) {
  case VTK_DOUBLE:
    return (*(double*)scalar);
  case VTK_FLOAT:
    return (double) (*(float*)scalar);
  case VTK_UNSIGNED_LONG:
    return (double) (*(unsigned long*)scalar);
  case VTK_LONG:
    return (double) (*(long*)scalar);
  case VTK_UNSIGNED_INT:
    return (double) (*(unsigned int*)scalar);
  case VTK_INT:
    return (double) (*(int*)scalar);
  case VTK_UNSIGNED_SHORT:
    return (double) (*(unsigned short*)scalar);
  case VTK_SHORT:
    return (double) (*(short*)scalar);
  case VTK_UNSIGNED_CHAR:
    return (double) (*(unsigned char*)scalar);
  case VTK_CHAR:
    return (double) (*(char*)scalar);
  }
  return -1;
}


void vtkViewImage::SyncResetCurrentPoint()
{
  if( this->IsLocked() )
  {
    return;
  }

  if (this->GetLinkPosition())
  {
    this->ResetCurrentPoint();
  }
  
  this->Lock();
  for( unsigned int i=0; i<this->Children.size(); i++)
  {
    vtkViewImage* view = vtkViewImage::SafeDownCast( this->Children[i] );
    if( view /*&& view->GetLinkPosition()*/ )
    {
      view->SyncResetCurrentPoint ();
      view->Update();
    }
  }
  this->UnLock();

}


void vtkViewImage::ResetCurrentPoint()
{

  if (!this->Image)
  {
    return;
  }
  
  double* bounds  = this->GetImage()->GetBounds();
  double  pos[3] =
    {
      (bounds[0]+bounds[1])/2.0,
      (bounds[2]+bounds[3])/2.0,
      (bounds[4]+bounds[5])/2.0
    };
  
  this->SetCurrentPoint(pos);
}


void vtkViewImage::SyncSetWindow (double w)
{
  if( this->IsLocked() )
  {
    return;
  }


  if (this->GetLinkWindowLevel())
  {
    this->SetWindow (w);
  }


  // this boolean is used so that the other observe won't call
  // SetCurrentPoint again and again and again...
  this->Lock();
  for( unsigned int i=0; i<this->Children.size(); i++)
  {

    vtkViewImage* view = vtkViewImage::SafeDownCast (this->Children[i]);

    if( view /*&& view->GetLinkWindowLevel()*/ )
    {
      view->SyncSetWindow (w);
      view->Update();
    }
  }

  this->UnLock();
}


void vtkViewImage::SyncStartWindowing ()
{
  if( this->IsLocked() )
  {
    return;
  }


  if (this->GetLinkWindowLevel())
  {
    this->StartWindowing ();
  }


  // this boolean is used so that the other observe won't call
  // SetCurrentPoint again and again and again...
  this->Lock();
  for( unsigned int i=0; i<this->Children.size(); i++)
  {

    vtkViewImage* view = vtkViewImage::SafeDownCast (this->Children[i]);

    if( view /*&& view->GetLinkWindowLevel()*/ )
    {
      view->SyncStartWindowing ();
      view->Update();
    }
  }

  this->UnLock();
}


void vtkViewImage::SyncEndWindowing ()
{
  if( this->IsLocked() )
  {
    return;
  }

  
  if (this->GetLinkWindowLevel())
  {
    this->EndWindowing ();
  }


  // this boolean is used so that the other observe won't call
  // SetCurrentPoint again and again and again...
  this->Lock();
  for( unsigned int i=0; i<this->Children.size(); i++)
  {

    vtkViewImage* view = vtkViewImage::SafeDownCast (this->Children[i]);

    if( view /*&& view->GetLinkWindowLevel()*/ )
    {
      view->SyncEndWindowing ();
      view->Update();
    }
  }

  this->UnLock();
}


void vtkViewImage::SyncSetLevel (double l)
{

  if( this->IsLocked() )
  {
    return;
  }


  if (this->GetLinkWindowLevel())
  {
    this->SetLevel (l);
  }

  // this boolean is used so that the other observe won't call
  // SetCurrentPoint again and again and again...
  this->Lock();
  for( unsigned int i=0; i<this->Children.size(); i++)
  {
    vtkViewImage* view = vtkViewImage::SafeDownCast (this->Children[i]);

    if( view /*&& view->GetLinkWindowLevel()*/ )
    {
      view->SyncSetLevel (l);
      view->Update();
    }
  }

  this->UnLock();

}


void vtkViewImage::SetWindowLevelFrom(vtkViewImage* p_view)
{
  if (p_view)
  {
    this->SetWindow( p_view->GetWindow() );
    this->SetLevel( p_view->GetLevel() );
  }
}



void vtkViewImage::SyncResetWindowLevel()
{

  if( this->IsLocked() )
  {
    return;
  }

  if (this->GetLinkWindowLevel())
  {
    this->ResetWindowLevel();
  }

  this->Lock();
  for( unsigned int i=0; i<this->Children.size(); i++)
  {
    vtkViewImage* view = vtkViewImage::SafeDownCast(this->Children[i]);
    if( view /*&& view->GetLinkWindowLevel()*/ )
    {
      view->SyncResetWindowLevel ();
      view->Update();
    }
  }
  this->UnLock();
}


void vtkViewImage::ResetWindowLevel()
{

  if (!this->Image)
  {
    return;
  }
  

  this->Image->UpdateInformation();
  int* extent = this->Image->GetWholeExtent();
  if( extent[0]==extent[1] && extent[2]==extent[3] && extent[4]==extent[5] ) // image has a null extent -> GetScalarRange() will crash
  {
    return;
  }
  
  this->Image->SetUpdateExtentToWholeExtent();
  this->Image->Update();
  
  
  if( this->Image->GetScalarType() == VTK_UNSIGNED_CHAR  && (this->Image->GetNumberOfScalarComponents()==3 || this->Image->GetNumberOfScalarComponents()==4) )
  {
    return;
  }


  double* range = this->Image->GetScalarRange();

  double window = range[1]-range[0];
  double level = 0.5*(range[1]+range[0]);

  if( this->ResetWindowLevelMode ==  FullRange )
  {
    // nothing to do
  }
  else if( this->ResetWindowLevelMode == UserDefinedPercentage )
  {

    if( this->FirstResetWindowLevel )
    {
      vtkImageAccumulate* histogram = vtkImageAccumulate::New();
      histogram->SetInput ( this->Image );
      
      int binCounts = 1000;
      
      int h_extent[6] = {0, binCounts-1, 0, 0, 0, 0};
      histogram->SetComponentExtent (h_extent);
      
      double spacing[3] = { (range[1]-range[0])/(double)(binCounts), 0.0, 0.0};
      histogram->SetComponentSpacing (spacing);
      
      double origin[3] = {range[0], 0.0, 0.0};
      histogram->SetComponentOrigin (origin);
      histogram->Update();
      
      vtkImageData* output = histogram->GetOutput();
      vtkIntArray*  ptData = vtkIntArray::SafeDownCast (output->GetPointData()->GetScalars());
      
      if( !ptData)
      {
	vtkErrorMacro( << "Error: Cannot cast point data to integers.");
	return;
      }
      
      double voxelCount = histogram->GetVoxelCount();
      double onePercent = voxelCount/100.0;
      
      int start=1;
      double currentPercent = 0.0;
      while( currentPercent<this->WindowLevelPercentage && start<binCounts )
      {
	double tuple;
	ptData->GetTuple (start, &tuple);
	currentPercent += tuple/onePercent;
	start++;
      }
      
      currentPercent = 0.0;
      int end = binCounts-1;
      while( currentPercent<this->WindowLevelPercentage && end>0 )
      {
	double tuple;
	ptData->GetTuple (end, &tuple);
	currentPercent += tuple/onePercent;
	end--;
      }
      
      double binLength = (range[1]-range[0])/(double)binCounts;
      
      window = (end-start)*binLength;
      level = 0.5*(start + end)*binLength;

      this->WindowForReset = window;
      this->LevelForReset = level;
      this->FirstResetWindowLevel = 0;
      
      histogram->Delete();
    }
    else
    {
      window = this->WindowForReset;
      level  = this->LevelForReset;
    }
  }
  
  
  window = (window-this->GetShift())/this->GetScale();
  level = (level-this->GetShift())/this->GetScale();


  this->SetWindow ( window );
  this->SetLevel ( level );
}


bool vtkViewImage::HasDataSet (vtkDataSet* dataset)
{
  if( !dataset )
  {
    return false;
  }

  bool res = false;
  for( unsigned int i=0; i<this->DataSetList.size(); i++)
  {
    if( dataset == this->DataSetList[i] )
    {
      res = true;
      break;
    }
  }

  return res;
}


vtkActor* vtkViewImage::SyncAddDataSet (vtkDataSet* dataset,  vtkProperty* property)
{

  if( this->IsLocked() )
  {
    return NULL;
  }


  vtkActor* actor = this->AddDataSet (dataset, property);

  this->Lock();
  for( unsigned int i=0; i<this->Children.size(); i++)
  {
    vtkViewImage* view = vtkViewImage::SafeDownCast (this->Children[i]);

    if( view )
    {
      view->SyncAddDataSet (dataset, property);
    }
  }
  this->UnLock();

  return actor;
}


vtkActor* vtkViewImage::AddDataSet (vtkDataSet* dataset,  vtkProperty* property)
{
  vtkErrorMacro(<<"You should not use this function here\nPlease use vtkViewImage2D or vtkViewImage3D classes instead (or any derived class).");
  return NULL;
}


void vtkViewImage::SyncRemoveDataSet (vtkDataSet* dataset)
{

  if( this->IsLocked() )
  {
    return;
  }


  this->RemoveDataSet (dataset);

  this->Lock();
  for( unsigned int i=0; i<this->Children.size(); i++)
  {
    vtkViewImage* view = vtkViewImage::SafeDownCast (this->Children[i]);

    if( view )
    {
      view->SyncRemoveDataSet (dataset);
    }
  }
  this->UnLock();

}


void vtkViewImage::RemoveDataSet (vtkDataSet* dataset)
{

  if (!dataset)
  {
    return;
  }

  std::vector<vtkActor*>   t_actorlist = this->DataSetActorList;
  std::vector<vtkDataSet*> t_datasetlist = this->DataSetList;
  this->DataSetActorList.clear();
  this->DataSetList.clear();

  for (unsigned int i=0; i<t_datasetlist.size(); i++)
  {
    if (dataset != t_datasetlist[i])
    {
      this->DataSetList.push_back (t_datasetlist[i]);
      this->DataSetActorList.push_back (t_actorlist[i]);
      }
    else
    {
      this->RemoveActor (t_actorlist[i]);
    }
  }
}


void vtkViewImage::SyncRemoveAllDataSet ()
{

  if( this->IsLocked() )
  {
    return;
  }


  this->RemoveAllDataSet ();

  this->Lock();
  for( unsigned int i=0; i<this->Children.size(); i++)
  {
    vtkViewImage* view = vtkViewImage::SafeDownCast (this->Children[i]);

    if( view )
    {
      view->SyncRemoveAllDataSet ();
    }
  }
  this->UnLock();
}


void vtkViewImage::RemoveAllDataSet ()
{
  for (unsigned int i=0; i<this->DataSetActorList.size(); i++)
  {
    this->RemoveActor (this->DataSetActorList[i]);
  }
  this->DataSetActorList.clear();
  this->DataSetList.clear();

}


void vtkViewImage::SyncModifyActors(vtkDataSet* dataset, bool cellColors)
{

  if( this->IsLocked() )
  {
    return;
  }


  this->ModifyActors (dataset, cellColors);


  this->Lock();
  for( unsigned int i=0; i<this->Children.size(); i++)
  {
    vtkViewImage* view = vtkViewImage::SafeDownCast (this->Children[i]);

    if( view )
    {
      view->SyncModifyActors (dataset, cellColors);
    }
  }
  this->UnLock();

}


void vtkViewImage::ModifyActors(vtkDataSet* dataset, bool cellColors)
{
  bool doit = true;


  if (!dataset)
  {
    doit = false;
  }

  vtkMapper* mapper = NULL;

  if (doit)
  {
    for (unsigned int i=0; i<this->DataSetList.size(); i++)
    {
      if (dataset == this->DataSetList[i])
      {
        mapper = this->DataSetActorList[i]->GetMapper();
        break;
      }
    }
    if (!mapper)
    {
      doit = false;
    }
  }

  if (doit)
  {
    mapper->Modified();

    if ( cellColors )
      mapper->SetScalarModeToUseCellData();
    else
      mapper->SetScalarModeToUsePointData();
  }
}


void vtkViewImage::SyncColorDataSetByArray(vtkDataSet* dataset, const char* arrayname, vtkColorTransferFunction* transfer)
{

  if( this->IsLocked() )
  {
    return;
  }


  this->ColorDataSetByArray (dataset, arrayname, transfer);


  this->Lock();
  for( unsigned int i=0; i<this->Children.size(); i++)
  {
    vtkViewImage* view = vtkViewImage::SafeDownCast (this->Children[i]);

    if( view )
    {
      view->SyncColorDataSetByArray (dataset, arrayname, transfer);
    }
  }
  this->UnLock();
}


void vtkViewImage::ColorDataSetByArray(vtkDataSet* dataset, const char* arrayname, vtkColorTransferFunction* transfer)
{

  bool doit = true;
  
  if (!dataset)
  {
    doit = false;
  }

  vtkDataArray* array = NULL;
  vtkMapper* mapper = NULL;

  if (doit)
  {
    for (unsigned int i=0; i<this->DataSetList.size(); i++)
    {
      if (dataset == this->DataSetList[i])
      {
        mapper = this->DataSetActorList[i]->GetMapper();
        break;
      }
    }
    if (!mapper)
    {
      doit = false;
    }
  }

  if (doit)
  {
    mapper->Modified();

    if (dataset->GetCellData())
    {
      array = dataset->GetCellData()->GetArray (arrayname);
      if (array)
      {
        mapper->SetScalarModeToUseCellFieldData();
      }
    }

    if (!array && dataset->GetPointData())
    {
      array = dataset->GetPointData()->GetArray (arrayname);
      if (array)
      {
        mapper->SetScalarModeToUsePointFieldData();
      }

    }

    if (!array)
    {
      mapper->SetScalarModeToDefault();
      mapper->SetInterpolateScalarsBeforeMapping(0);
      doit = false;
    }
  }

  if (doit)
  {
    mapper->SetLookupTable (transfer);
    mapper->SetScalarRange (array->GetRange()[0], array->GetRange()[1]);
    mapper->SetInterpolateScalarsBeforeMapping(1);
    mapper->SelectColorArray (array->GetName());
  }
}



vtkDataSet* vtkViewImage::GetDataSet (unsigned int i)
{
  if ( i>=this->DataSetList.size() )
    return NULL;

  return this->DataSetList[i];
}


vtkDataSet* vtkViewImage::GetDataSetFromActor (vtkActor* actor)
{
  for (unsigned int i=0; i<this->DataSetActorList.size(); i++)
  {
    if (actor == this->DataSetActorList[i])
      return this->DataSetList[i];
  }
  return NULL;
}


vtkActor* vtkViewImage::GetDataSetActor (unsigned int i)
{
  if ( i>=this->DataSetActorList.size() )
    return NULL;

  return this->DataSetActorList[i];
}


vtkActor* vtkViewImage::GetDataSetActor (vtkDataSet* dataset)
{
  if (!dataset)
    return NULL;

  for (unsigned int i=0; i<this->DataSetList.size(); i++)
  {
    if (dataset == this->DataSetList[i])
      return this->DataSetActorList[i];
  }
  return NULL;

}


void vtkViewImage::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


void vtkViewImage::SyncSetZoom (double factor)
{

  if( this->IsLocked() )
  {
    return;
  }

  if (this->GetLinkZoom())
  {
    this->SetZoom (factor);
  }

  this->Lock();
  for( unsigned int i=0; i<this->Children.size(); i++)
  {
    vtkViewImage* view = vtkViewImage::SafeDownCast (this->Children[i]);

    if( view /*&& view->GetLinkZoom()*/ )
    {
      view->SyncSetZoom (factor);
      view->Update();
    }
  }

  this->UnLock();
}


void vtkViewImage::SyncSetLookupTable (vtkScalarsToColors* lut)
{

  if( this->IsLocked() )
  {
    return;
  }

  this->SetLookupTable (lut);

  this->Lock();
  for( unsigned int i=0; i<this->Children.size(); i++)
  {
    vtkViewImage* view = vtkViewImage::SafeDownCast (this->Children[i]);

    if( view )
    {
      view->SyncSetLookupTable (lut);
      view->Update();
    }
  }

  this->UnLock();
}


void vtkViewImage::SyncResetZoom ()
{

  if( this->IsLocked() )
  {
    return;
  }

  if (this->GetLinkZoom())
  {
    this->ResetZoom ();
  }

  this->Lock();
  for( unsigned int i=0; i<this->Children.size(); i++)
  {
    vtkViewImage* view = vtkViewImage::SafeDownCast (this->Children[i]);

    if( view )
    {
      view->SyncResetZoom ();
      view->Update();
    }
  }

  this->UnLock();
}


void vtkViewImage::ResetZoom()
{
  this->ResetCamera();
  this->Zoom = 1.0;
}


void vtkViewImage::SyncReset ()
{

  if( this->IsLocked() )
  {
    return;
  }

  this->Reset ();

  this->Lock();
  for( unsigned int i=0; i<this->Children.size(); i++)
  {
    vtkViewImage* view = vtkViewImage::SafeDownCast (this->Children[i]);

    if( view )
    {
      view->SyncReset ();
      view->Update();
    }
  }

  this->UnLock();
}


void vtkViewImage::SetSizeData (const char* str)
{
  this->SizeData_str = str;
  this->Modified();
}


void vtkViewImage::SetPatientNameData (const char* str)
{
  this->PatientNameData = str;
  this->Modified();
}


void vtkViewImage::SetStudyNameData (const char* str)
{
  this->StudyNameData = str;
  this->Modified();
}


void vtkViewImage::SetSerieNameData (const char* str)
{
  this->SerieNameData = str;
  this->Modified();
}


void vtkViewImage::SetSizeDataVisibility (int val)
{
  this->SizeDataVisibility = val;
  if( val )
  {
    this->SetUpLeftAnnotation ( this->GetSizeData() );
  }
  else
  {
    this->SetUpRightAnnotation ("");
  }
  
}


void vtkViewImage::SyncSetMaskImage (vtkImageData* mask, vtkLookupTable* lut)
{

  if( this->IsLocked() )
  {
    return;
  }

  this->SetMaskImage (mask, lut);

  this->Lock();
  for( unsigned int i=0; i<this->Children.size(); i++)
  {
    vtkViewImage* view = vtkViewImage::SafeDownCast (this->Children[i]);

    if( view )
    {
      view->SyncSetMaskImage (mask, lut);
      view->Update();
    }
  }

  this->UnLock();
}


void vtkViewImage::SyncSetOverlappingImage (vtkImageData* image)
{

  if( this->IsLocked() )
  {
    return;
  }

  this->SetOverlappingImage (image);

  this->Lock();
  for( unsigned int i=0; i<this->Children.size(); i++)
  {
    vtkViewImage* view = vtkViewImage::SafeDownCast (this->Children[i]);

    if( view )
    {
      view->SyncSetOverlappingImage (image);
      view->Update();
    }
  }

  this->UnLock();
}



void vtkViewImage::SyncRemoveMaskImage ()
{

  if( this->IsLocked() )
  {
    return;
  }

  this->RemoveMaskImage ();

  this->Lock();
  for( unsigned int i=0; i<this->Children.size(); i++)
  {
    vtkViewImage* view = vtkViewImage::SafeDownCast (this->Children[i]);

    if( view )
    {
      view->SyncRemoveMaskImage ();
      view->Update();
    }
  }

  this->UnLock();
}


void vtkViewImage::SyncRemoveOverlappingImage ()
{

  if( this->IsLocked() )
  {
    return;
  }

  this->RemoveOverlappingImage ();

  this->Lock();
  for( unsigned int i=0; i<this->Children.size(); i++)
  {
    vtkViewImage* view = vtkViewImage::SafeDownCast (this->Children[i]);

    if( view )
    {
      view->SyncRemoveOverlappingImage ();
      view->Update();
    }
  }

  this->UnLock();
}


void vtkViewImage::SetDirectionMatrix (vtkMatrix4x4 *mat)
{
  if (!mat)
    return;

  this->DirectionMatrix->DeepCopy (mat);

  double A[3][3];
  double Q[3][3];
  double M[3][3];

  for (int i=0; i<3; i++)
    for (int j=0; j<3; j++)
      A[i][j] = mat->GetElement (i,j);
  
  vtkMath::Orthogonalize3x3 (A, Q);


  double detQ = vtkMath::Determinant3x3 (Q);
  

  // copy-pasted from nifti1_io.c: nifti_mat44_to_orientation()

  double vbest = -666.0;
  int ibest = 1;
  int jbest = 2;
  int kbest = 3;
  int pbest = 1;
  int qbest = 1;
  int rbest = 1;
  
  for( int i=1; i <= 3; i++ )
  {
    for( int j=1; j <= 3; j++ )
    {
      if( i == j )
	continue;

      for( int k=1; k <= 3; k++ )
      {
	if( i == k || j == k )
	  continue;
	
	A[0][0] = A[0][1] = A[0][2] =
	  A[1][0] = A[1][1] = A[1][2] =
	  A[2][0] = A[2][1] = A[2][2] = 0.0;
	
	for( int p=-1; p <= 1; p+=2 )
	{
	  for( int q=-1; q <= 1; q+=2 )
	  {
	    for( int r=-1; r <= 1; r+=2 )
	    {
	      A[0][i-1] = p;
	      A[1][j-1] = q;
	      A[2][k-1] = r;
	      
	      double detA = vtkMath::Determinant3x3(A);
	      
	      if( detA * detQ <= 0.0 )
		continue;
	      
	      vtkMath::Multiply3x3 (A, Q, M);
	      
	      double val = M[0][0] + M[1][1] + M[2][2];
	      if( val > vbest )
	      {
		vbest = val;
		ibest = i;
		jbest = j;
		kbest = k;
		pbest = p;
		qbest = q;
		rbest = r;
	      }
	    }
	  }
	}
      }
    }
  }


  this->DirectionMatrix->Zero();
  this->DirectionMatrix->SetElement (ibest-1, 0, pbest);
  this->DirectionMatrix->SetElement (jbest-1, 1, qbest);
  this->DirectionMatrix->SetElement (kbest-1, 2, rbest);
  this->DirectionMatrix->SetElement (3, 3, 1.0);

  this->Modified();
  
}

void vtkViewImage::SyncSetVolumeIndex (vtkIdType volumeIndex)
{
    if( this->IsLocked() )
     {
        return;
     }
    
    if (this->GetLinkVolumeIndex())
     {
        this->SetVolumeIndex (volumeIndex);
     }
    
    this->Lock();
    for( unsigned int i=0; i<this->Children.size(); i++)
     {
        vtkViewImage* view = vtkViewImage::SafeDownCast (this->Children[i]);
        
        if( view  )
         {
            view->SyncSetVolumeIndex (volumeIndex);
            view->Update();
         }
     }
    
    this->UnLock();    
}

template < class T >
void vtkViewImage::SetVolumeIndex (vtkIdType volumeIndex)
{
    typedef typename itk::Image<T, 4> ImageType4d;
    typedef typename itk::Image<T, 3> ImageType3d;
    typedef typename itk::ExtractImageBufferFilter< ImageType4d, ImageType3d > ExtractFilterType;
    
    // Since we store the extractor type using an enum, the dynamic cast should always succeed, 
    // unless the Filter was already NULL.
    ExtractFilterType * extractor = dynamic_cast < ExtractFilterType *> ( this->Impl->ImageTemporalFilter.GetPointer () ) ;
    if ( extractor )
     {
        unsigned int volumeLimit = extractor->GetInput()->GetLargestPossibleRegion().GetSize()[3] -1;
        if (volumeIndex<0)
            volumeIndex = 0;
        if (volumeIndex>(vtkIdType)volumeLimit)
            volumeIndex = volumeLimit;
        typename ImageType4d::RegionType region = extractor->GetExtractionRegion ();
        region.SetIndex (3, volumeIndex);
        extractor->SetExtractionRegion (region);
        extractor->UpdateLargestPossibleRegion();
     }
}

//----------------------------------------------------------------------------
void vtkViewImage::SetVolumeIndex ( vtkIdType index )
{
    if ( this->VolumeIndex != index ) {
        
        if ( this->Impl->ImageTemporalFilter.IsNotNull ()) {
            
            switch ( this->Impl->TemporalFilterType ) {
                    
                default:
                case IMAGE_VIEW_NONE : break;
                    // Macro calls template method for correct argument type.
#define ViewImageCaseEntry( type , enumName )		\
case enumName :				\
{						\
this->SetVolumeIndex < type > ( index );	\
break ;					\
}
                    
                    ViewImageCaseEntry( double, IMAGE_VIEW_DOUBLE );
                    ViewImageCaseEntry( float, IMAGE_VIEW_FLOAT );
                    ViewImageCaseEntry( int, IMAGE_VIEW_INT );
                    ViewImageCaseEntry( unsigned int, IMAGE_VIEW_UNSIGNED_INT );
                    ViewImageCaseEntry( short, IMAGE_VIEW_SHORT );
                    ViewImageCaseEntry( unsigned short, IMAGE_VIEW_UNSIGNED_SHORT );
                    ViewImageCaseEntry( long, IMAGE_VIEW_LONG );
                    ViewImageCaseEntry( unsigned long, IMAGE_VIEW_UNSIGNED_LONG );
                    ViewImageCaseEntry( char, IMAGE_VIEW_CHAR );
                    ViewImageCaseEntry( unsigned char, IMAGE_VIEW_UNSIGNED_CHAR );
                    ViewImageCaseEntry( RGBPixelType, IMAGE_VIEW_RGBPIXELTYPE );
                    ViewImageCaseEntry( RGBAPixelType, IMAGE_VIEW_RGBAPIXELTYPE );
                    ViewImageCaseEntry( UCharVector3Type, IMAGE_VIEW_UCHARVECTOR3TYPE  );
                    ViewImageCaseEntry( FloatVector3Type, IMAGE_VIEW_FLOATVECTOR3TYPE  );
                    
            };
        }
        this->VolumeIndex = index;
        this->GetImage ()->UpdateInformation ();
        this->GetImage ()->PropagateUpdateExtent ();
        this->InvokeEvent( vtkViewImage::ViewImageVolumeIndexChangedEvent );
        this->Modified ();
    }
}


template <class T>
inline void vtkViewImage::SetITKImage (typename itk::Image<T, 3> *itkImage)
{									
    if( !itkImage )
     {									
         return;								
     }
    if (this->ITKImage==itkImage)
        return;
    
    typedef itk::ImageToVTKImageFilter< itk::Image<T, 3> > ConverterType; 
    typename ConverterType::Pointer myConverter = ConverterType::New();		
    myConverter->SetInput ( itkImage );				
    myConverter->Update();						
    typename itk::Image<T, 3>::DirectionType directions = itkImage->GetDirection(); 
    vtkMatrix4x4 *matrix = vtkMatrix4x4::New();				
    matrix->Identity();							
    for (int i=0; i<3; i++)						
        for (int j=0; j<3; j++)						
            matrix->SetElement (i, j, directions (i,j));			
    this->SetDirectionMatrix ( matrix );				
    matrix->Delete();							
    this->SetImage ( myConverter->GetOutput() );			
    this->Impl->ImageConverter = myConverter;					
    this->ITKImage = itkImage;						
}


template <class T>
inline void vtkViewImage::SetITKImage4 (typename itk::Image<T, 4> *itkImage)
{									
    if( !itkImage )
     {									
         return;								
     }
    if (this->ITKImage4 ==itkImage)
        return;
    
    typedef typename itk::Image<T, 4> ImageType4d;
    typedef typename itk::Image<T, 3> ImageType3d;
    typedef typename itk::ExtractImageBufferFilter< ImageType4d, ImageType3d > ExtractFilterType;
    typename ExtractFilterType::Pointer extractor = ExtractFilterType::New();
    typename ImageType4d::SizeType size = itkImage->GetLargestPossibleRegion().GetSize();
    
    unsigned int volumeIndex = this->VolumeIndex;
    unsigned int volumeLimit = itkImage->GetLargestPossibleRegion().GetSize()[3]-1;

    if (volumeIndex>volumeLimit)
        volumeIndex = volumeLimit;
    
    typename ImageType4d::IndexType index = {{0,0,0, volumeIndex}};
    
    size[3] = 0;
    typename ImageType4d::RegionType region;
    region.SetSize (size);
    region.SetIndex (index);
    
    extractor->SetExtractionRegion (region);
    extractor->SetInput ( itkImage );
    
    this->Impl->ImageTemporalFilter = extractor;
    this->Impl->TemporalFilterType = vtkViewImage::vtkViewImageImplementation::GetViewImageType <T> ();
    this->ITKImage4 = itkImage;
    typename ImageType3d::Pointer itkImage3 = extractor->GetOutput ();
    extractor->UpdateLargestPossibleRegion();
    this->SetITKImage( itkImage3 );
}

#define vtkImplementSetITKImageMacro(type)				\
void vtkViewImage::SetITKImage(itk::Image<type, 3> *itkImage) \
{									\
SetITKImage < type > (itkImage);					\
}
vtkImplementSetITKImageMacro (double);
vtkImplementSetITKImageMacro (float);
vtkImplementSetITKImageMacro (int);
vtkImplementSetITKImageMacro (unsigned int);
vtkImplementSetITKImageMacro (short);
vtkImplementSetITKImageMacro (unsigned short);
vtkImplementSetITKImageMacro (long);
vtkImplementSetITKImageMacro (unsigned long);
vtkImplementSetITKImageMacro (char);
vtkImplementSetITKImageMacro (unsigned char);
vtkImplementSetITKImageMacro (RGBPixelType);
vtkImplementSetITKImageMacro (RGBAPixelType);
vtkImplementSetITKImageMacro (UCharVector3Type);
vtkImplementSetITKImageMacro (FloatVector3Type);

itk::ImageBase<3>* vtkViewImage::GetITKImage() const
{
    return this->ITKImage;
}

#define vtkImplementSetITKImage4Macro(type)				\
void vtkViewImage::SetITKImage4 (itk::Image<type, 4> *itkImage) \
{									\
SetITKImage4 < type > (itkImage);					\
}

vtkImplementSetITKImage4Macro (double);
vtkImplementSetITKImage4Macro (float);
vtkImplementSetITKImage4Macro (int);
vtkImplementSetITKImage4Macro (unsigned int);
vtkImplementSetITKImage4Macro (short);
vtkImplementSetITKImage4Macro (unsigned short);
vtkImplementSetITKImage4Macro (long);
vtkImplementSetITKImage4Macro (unsigned long);
vtkImplementSetITKImage4Macro (char);
vtkImplementSetITKImage4Macro (unsigned char);
vtkImplementSetITKImage4Macro (RGBPixelType);
vtkImplementSetITKImage4Macro (RGBAPixelType);
vtkImplementSetITKImage4Macro (UCharVector3Type);
vtkImplementSetITKImage4Macro (FloatVector3Type);

itk::ImageBase<4>* vtkViewImage::GetTemporalITKImage() const
{
    return this->ITKImage4;
}


