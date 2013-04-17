/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "vtkRenderingAddOnExport.h"

#include <vtkSynchronizedView.h>

#include <vtkCommand.h>
#include <vtkTransform.h>
#include <vtkLookupTable.h>
#include <vtkImageData.h>
#include <vtkScalarBarActor.h>
#include <vtkMatrix4x4.h>

#include <itkImage.h>
#include <itkRGBPixel.h>
#include <itkRGBAPixel.h>

#include <vector>


class vtkScalarsToColors;
class vtkImageBlend;
class vtkColorTransferFunction;
class vtkDataSet;
class vtkProperty;
class vtkMapper;
class vtkActor;



/**
   \class vtkViewImage vtkViewImage.h "vtkViewImage2D.h"
   \brief Base class for 2D/3D image viewers, see vtkViewImage2D and vtkViewImage3D.
   \author: Pierre Fillard & Marc Traina & Nicolas Toussaint
*/


class VTK_RENDERINGADDON_EXPORT vtkViewImage : public vtkSynchronizedView
{

 public:

  static vtkViewImage* New();
  vtkTypeRevisionMacro(vtkViewImage, vtkSynchronizedView);

  //BTX
  /** Indices use in Vtk referentials corresponding to the standard x, y and z unitary vectors. */
  enum DirectionIds
  {
    X_ID = 0,
    Y_ID,
    Z_ID,
    NB_DIRECTION_IDS  // The number of DirectionIds
  };
  //ETX


  //BTX
  /** Ids of the 2D plan displayed in the view. Each 2D plan is defined with regard to one of
      the 3 directions (x,y,z). */
  enum PlanIds
  {
    SAGITTAL_ID = 0,
    CORONAL_ID,
    AXIAL_ID,
    NB_PLAN_IDS  // The number of PlanIds
  };
  //ETX


  //BTX
  enum EventIds
  {
    ViewImagePositionChangeEvent=(vtkCommand::UserEvent+1),
    ViewImageWindowLevelChangeEvent,
    ViewImageZoomChangeEvent,
    ViewImageVolumeIndexChangedEvent 
  };
  //ETX


  //BTX
  enum
  {
    FullRange,
    UserDefinedPercentage
  };
  //ETX


  // General Get Methods about the image

  /**
     Get the begining position of the first pixel in the given axis. Note : the
     position (0,0,0) is the center of the first pixel (0,0,0) so the return value can be
     negative, depending of the origin of the image.
  */
  virtual double GetWholeMinPosition(unsigned int p_axis);


  /**
     Get the end position of the last pixel in the given axis.
  */
  virtual  double GetWholeMaxPosition(unsigned int p_axis);


  /**
     Get the id of the axis orthogonal to the given plan
     (x for sagittal, y for coronal and z for axial).
  */
  unsigned int GetOrthogonalAxis(unsigned int p_plan);
  

  /**
     Return the voxel coordinates of the point pos.
  */
  void GetVoxelCoordinates( double pos[3], int p_coordinates[3]);


  /**
     Fill the p_coordinates parameter with the coordinates of the voxel
     corresponding to the current point.
  */
  virtual void GetCurrentVoxelCoordinates(int p_coordinates[3]);


  /**
     Get the image value of the current point in double.
  */
  virtual double GetCurrentPointDoubleValue ();
  

  /**
     Get the current image.
  */
  vtkGetObjectMacro (Image, vtkImageData);


  /**
     Set the current image.
  */
  vtkSetObjectMacro(Image, vtkImageData);


  /**
     
  */
  virtual void SetVolumeIndex ( vtkIdType index );
  vtkGetMacro(VolumeIndex, vtkIdType);
    
  void SyncSetVolumeIndex(vtkIdType index);
    
  /**
     When ITK is set ot ON, we propose the following method to open
     directly an ITK image. Because of the templates, we have to setup
     a method for each type of image. Up to now are supported 3D images
     of types: double, float, (unsigned) int, (unsigned) short and
     (unsigned) char. This class does the ITK->VTK conversion using
     the filter itk::ImageToVTKImageFilter (taken from InsightApplications).
     The problem with this filter is that if it is not registered,
     the output is erased along with the filter (in fact the output has
     the same registered count as the input. For instance, if the input
     image comes from an ITK reader, its reference count is set to 1.
     After conversion, one would like to erase the reader. If the
     converter is not referenced, the output buffer sees its ref count
     goes to 0, which result in a godamn seg fault. To overcome this,
     we reference the current converter in the pointer ImageConverter.
   */

  typedef itk::RGBPixel<unsigned char>  RGBPixelType;
  typedef itk::RGBAPixel<unsigned char> RGBAPixelType;
  typedef itk::Vector<unsigned char, 3> UCharVector3Type;
  typedef itk::Vector<float, 3> FloatVector3Type;

  //BTX
  void SetITKImage (itk::Image<double, 3>*);
  void SetITKImage (itk::Image<float, 3>*);
  void SetITKImage (itk::Image<int, 3>*);
  void SetITKImage (itk::Image<unsigned int, 3>*);
  void SetITKImage (itk::Image<short, 3>*);
  void SetITKImage (itk::Image<unsigned short, 3>*);
  void SetITKImage (itk::Image<long, 3>*);
  void SetITKImage (itk::Image<unsigned long, 3>*);
  void SetITKImage (itk::Image<char, 3>*);
  void SetITKImage (itk::Image<unsigned char, 3>*);
  void SetITKImage (itk::Image<RGBPixelType, 3>*);
  void SetITKImage (itk::Image<RGBAPixelType, 3>*);
  void SetITKImage (itk::Image<UCharVector3Type, 3>*);
  void SetITKImage (itk::Image<FloatVector3Type, 3>*);
  itk::ImageBase<3>* GetITKImage() const;

  virtual void SetITKImage4 (itk::Image<double, 4> *input);
  virtual void SetITKImage4 (itk::Image<float, 4> *input);
  virtual void SetITKImage4 (itk::Image<int, 4> *input);
  virtual void SetITKImage4 (itk::Image<unsigned int, 4> *input);
  virtual void SetITKImage4 (itk::Image<short, 4> *input);
  virtual void SetITKImage4 (itk::Image<unsigned short, 4> *input);
  virtual void SetITKImage4 (itk::Image<long, 4> *input);
  virtual void SetITKImage4 (itk::Image<unsigned long, 4> *input);
  virtual void SetITKImage4 (itk::Image<char, 4> *input);
  virtual void SetITKImage4 (itk::Image<unsigned char, 4> *input);
  virtual void SetITKImage4 (itk::Image<RGBPixelType, 4> *input);
  virtual void SetITKImage4 (itk::Image<RGBAPixelType, 4> *input);
  virtual void SetITKImage4 (itk::Image<UCharVector3Type, 4> *input);
  virtual void SetITKImage4 (itk::Image<FloatVector3Type, 4> *input);
  itk::ImageBase<4>* GetTemporalITKImage() const;
  //ETX


  /**
     Ensure that the displayed point is up to date. Should be overriden by subclasses.
  */
  virtual void UpdatePosition() {};

  
  /**
     Get/Set the transformation to be applied to the image.
  */
  vtkSetObjectMacro (Transform, vtkTransform);
  vtkGetObjectMacro (Transform, vtkTransform);


  /**
     Get the ScalarBarActor.
  */
  vtkGetObjectMacro (ScalarBar, vtkScalarBarActor);

  
  /**
     Set/Get the scalar bar visibility
  */
  void SetScalarBarVisibility (int v)
  { this->ScalarBar->SetVisibility(v);
    this->Modified(); }
  int GetScalarBarVisibility() const
  { return this->ScalarBar->GetVisibility(); }
  vtkBooleanMacro (ScalarBarVisibility, int);
  
  
  /**
     Set/Get a user-defined lookup table. This method is synchronized.
  */
  virtual void SetLookupTable (vtkScalarsToColors* lut)
  {
    this->LookupTable = lut;
    this->ScalarBar->SetLookupTable ( lut );
    this->Modified();
  }
  void SyncSetLookupTable (vtkScalarsToColors* lut);
  virtual vtkScalarsToColors* GetLookupTable() const
  { return this->LookupTable; }

  
  /**
     Set/Get the window/level/zoom parameter.
     This method is synchronized , except it LinkWindowLevel if set to 0.
  */
  virtual void SetWindow (double win)
  {
    this->Window = win;
    this->InvokeEvent (vtkViewImage::ViewImageWindowLevelChangeEvent, NULL);
    this->Modified();
  }
  vtkGetMacro (Window, double);
  void SyncSetWindow(double w);

  
  /**
     This method is called just before windowing. Subclass should give it a meaning.
   */
  virtual void StartWindowing(){};
  void SyncStartWindowing();


  /**
     This method is called just after windowing. Subclass should give it a meaning.
   */
  virtual void EndWindowing(){};
  void SyncEndWindowing();
  
  /**
     Set/Get the window/level/zoom parameter.
     This method is synchronized , except it LinkWindowLevel if set to 0.
  */  
  virtual void SetLevel (double lev)
  {
    this->Level = lev;
    this->InvokeEvent (vtkViewImage::ViewImageWindowLevelChangeEvent, NULL);
    this->Modified();
  }
  vtkGetMacro (Level, double);
  void SyncSetLevel(double l);
  

  /**
     Reset window level to calculated default value.
     This method is synchronized , except it LinkWindowLevel if set to 0.
  */
  virtual void ResetWindowLevel();
  void SyncResetWindowLevel();

  
  /**
     Set (copy) the windows level from given view window level.
  */
  virtual void SetWindowLevelFrom(vtkViewImage* p_view);

  
  /**
     Set/Get the window/level/zoom parameter.
     This method is synchronized unless LinkZoom is set to 0.
  */
  virtual void SetZoom (double zoom)
  {
    this->Zoom = zoom;
    this->InvokeEvent (vtkViewImage::ViewImageZoomChangeEvent, NULL);
    this->Modified();
  }
  vtkGetMacro (Zoom, double);
  void SyncSetZoom (double factor);


  /**
     Reset the current zoom factor.
   */
  virtual void ResetZoom();
  void SyncResetZoom();
  
  
  /**
     Return the Z slice for the given position pos in real world coordinates. The Z slice is
     relative to a given orientation.
  */
  virtual int GetSliceForPosition(const double pos[3], unsigned int p_orientation);


  /**
     Given a slice and an orientation, returns the real world coordinates.
   */
  virtual void GetPositionForSlice ( int slice, int orientation, double pos[3]);

  
  /**
     Set/Get the slice number within the current point is
     @param p_slice : the desired slice
     @param p_orientation : the desired slice type AXIAL, SAGITTAL, CORONAL.
  */
  virtual void SetSlice (unsigned int p_orientation, int p_slice);
  virtual void SyncSetSlice (unsigned int p_orientation, int p_slice);
  virtual int GetSlice(unsigned int p_orientation);

  
  /**
     Set the Slice to display. Z is relative to the displayed plan.
  */
  virtual void SetZSlice(int) { this->Modified(); }
  void SyncSetZSlice (int p_slice);
  // GetZSlice() ??


  /**
     Change the current position of the image. This method is synchronized.
  */
  virtual void SetCurrentPoint (const double p_point[3]);
  void SyncSetCurrentPoint(const double p_point[3]);
  

  /**
     Synonym to SetCurrentPoint().
  */
  void SetPosition (const double p_point[3] )
  { this->SetCurrentPoint ( p_point ); }
  void SyncSetPosition (const double p_point[3])
  { this->SyncSetCurrentPoint (p_point); }


  /**
     Returns the current point.
  */
  virtual const double* GetCurrentPoint() const
  { return CurrentPoint; }
  void GetCurrentPoint(double p_point[3]) const
  {
    p_point[0] = CurrentPoint[0];
    p_point[1] = CurrentPoint[1];
    p_point[2] = CurrentPoint[2];
  }
  

  /**
     Reset the current point to the center of the image in the 3 axes.
     This method is synchronized.
  */
  virtual void ResetCurrentPoint();
  void SyncResetCurrentPoint();


  /**
     Synonym to SyncResetCurrentPoint().
  */
  void SyncResetPosition()
  {
    this->SyncResetCurrentPoint();
  }
  

  /**
     Reset Window-Level, current point and zoom. This method is synchronized.
  */
  virtual void Reset()
  {
    this->ResetWindowLevel();
    this->ResetCurrentPoint();
    this->ResetZoom();
  }
  void SyncReset();
  


  /**
     Set the color window/level link ON or OFF.
  */
  vtkSetMacro (LinkWindowLevel, int);
  vtkGetMacro (LinkWindowLevel, int);


  /**
     Set the position link ON or OFF.
  */
  vtkSetMacro (LinkPosition, int);
  vtkGetMacro (LinkPosition, int);

  
  /**
     Set the zoom link ON or OFF.
  */
  vtkSetMacro (LinkZoom, int);
  vtkGetMacro (LinkZoom, int);


  /**
     Set the zoom link ON or OFF.
  */
  vtkSetMacro (LinkVolumeIndex, int);
  vtkGetMacro (LinkVolumeIndex, int);
    
  /**
     Shift/Scale are used to get the true image intensity if the image
     was scaled before being inputed to the view.
  */
  vtkSetMacro (Shift, double);
  vtkGetMacro (Shift, double);

  
  /**
     Shift/Scale are used to get the true image intensity if the image
     was scaled before being inputed to the view.
  */
  vtkSetMacro (Scale, double);
  vtkGetMacro (Scale, double);


  /**
     Set/Get the image visibility.
   */
  vtkSetMacro (Visibility, int);
  vtkGetMacro (Visibility, int);
  vtkBooleanMacro (Visibility, int);
  

  virtual void PrintSelf(ostream& os, vtkIndent indent);


  /**
     Set/Get method for setting the size data. Size data consists in the
     slice number and voxel size + window/level and is displayed at the
     top left corner of the window.
  */
  virtual void SetSizeData (const char* str);
  virtual const char* GetSizeData() const
  { return SizeData_str.c_str(); }


  /**
     Set/Get the size data visibility.
   */
  virtual void SetSizeDataVisibility (int val);
  vtkBooleanMacro (SizeDataVisibility, int);
  vtkGetMacro (SizeDataVisibility, int);


  virtual void SetPatientNameData (const char* str);
  virtual const char* GetPatientNameData() const
  { return PatientNameData.c_str(); }

  virtual void SetStudyNameData (const char* str);
  virtual const char* GetStudyNameData() const
  { return StudyNameData.c_str(); }

  virtual void SetSerieNameData (const char* str);
  virtual const char* GetSerieNameData() const
  { return SerieNameData.c_str(); }

  
  /**
     Set/Get a mask image and its corresponding LookupTable. The mask image will
     be overlapped to the current image, and the lookup table is used to assess
     the color of the label: label 0 will have color given by entry 0 of the LUT, etc.
     The image has to be of type unsigned char.
     This method is synchronized.
  */
  virtual void SetMaskImage (vtkImageData* mask, vtkLookupTable* lut)
  {
    this->MaskImage = mask;
    this->MaskLUT   = lut;
    this->Modified();
  }
  void SyncSetMaskImage (vtkImageData* mask, vtkLookupTable* lut);
  vtkGetObjectMacro (MaskImage, vtkImageData);
  
  
  /**
     Remove the mask image (if any)
  */
  virtual void RemoveMaskImage(){};
  void SyncRemoveMaskImage ();
  

  /**
     Set an overlapping second image. It uses an internal LUT to assess the color.
     It does not need to be of type unsigned char.
     This method is synchronized.
  */
  virtual void SetOverlappingImage (vtkImageData* image)
  { this->OverlappingImage = image; this->Modified(); }
  void SyncSetOverlappingImage (vtkImageData* image);
  vtkGetObjectMacro (OverlappingImage, vtkImageData);

  
  /**
     Remove the overlapping image (if any)
  */
  virtual void RemoveOverlappingImage(){};
  void SyncRemoveOverlappingImage ();
  
  
  /**
     Add a dataset to the view (polydata or grid).
     The dataset will be cut by planes defining the current slice displayed.
     This results in a loss of dimensionality, i.e. tetrahedron will be displayed
     as triangles, triangles as lines, lines as points.
     A vtkProperty of the dataset can be specified.
     This method is synchronized.
  */
  virtual vtkActor* AddDataSet (vtkDataSet* dataset, vtkProperty* property = NULL);
  vtkActor* SyncAddDataSet (vtkDataSet* dataset, vtkProperty* property = NULL);
  
  
  /**
     This method allows you to remove a previously added dataset off the view.
     It simply removes the actor from the renderer.
     This method is synchronized.
  */
  virtual void RemoveDataSet (vtkDataSet* dataset);
  void SyncRemoveDataSet (vtkDataSet* dataset);
  

  virtual void RemoveAllDataSet ();
  void SyncRemoveAllDataSet ();
  

  /**
     Test if the dataset was already passed to the view.
  */
  virtual bool HasDataSet (vtkDataSet* dataset);


  vtkDataSet* GetDataSet (unsigned int i);
  vtkDataSet* GetDataSetFromActor (vtkActor* actor);
  vtkActor* GetDataSetActor (unsigned int i);
  vtkActor* GetDataSetActor (vtkDataSet* dataset);


  /**
     This method colorizes a given dataset (previously added with AddDataSet()) by one of its arrays
     It doesn't handle colorization by a specific component yet. If the array contains multi-component scalars,
     it takes the norm of the vector.
     This method is synchronized.
  */
  virtual void ColorDataSetByArray(vtkDataSet* dataset, const char* arrayname, vtkColorTransferFunction* transfer);
  void SyncColorDataSetByArray(vtkDataSet* dataset, const char* arrayname, vtkColorTransferFunction* transfer);

  
  /**
     This method changes the actors associated with a given dataset (previously added with AddDataSet())
     to switch between the use of cell array or point data array. Used for switch between color and direction based
     colors of fiber bundles projections.
     This method is synchronized.
  */
  virtual void ModifyActors(vtkDataSet* dataset, bool cellColors);
  void SyncModifyActors(vtkDataSet* dataset, bool cellColors);
  
  
  /**
     Specify how the ResetWindowLevel() method behaves. If set to FullRange,
     ResetWindowLevel() sets the contrast to match the full range of the image
     i.e: 0:range[0] and 255: range[1].
     If set to UserDefinedPercentage, the X% highest and lowest voxels are
     removed to calculate the range. It is more robust to outliers that have
     a very high and low intensity compared to the main element of the image.
     The percentage is set with SetWindowLevelPercentage() (default: 0.1).
   */
  void SetResetWindowLevelMode(const int& mode)
  {
    this->ResetWindowLevelMode = mode;
    this->Modified();
  }

  void SetResetWindowLevelModeToFullRange()
  {
    this->ResetWindowLevelMode = FullRange;
    this->Modified();
  }

  void SetResetWindowLevelModeToUserDefinedPercentage()
  {
    this->ResetWindowLevelMode = UserDefinedPercentage;
    this->Modified();
  }

  /**
     Set the percentage of voxels used to reset the window/level range when
     the reset window/level mode is set to UserDefinedPercentage. Should be
     called before the first call to ResetWindowLevel().
   */
  vtkSetMacro (WindowLevelPercentage, double);
  vtkGetMacro (WindowLevelPercentage, double);


  /**
     Set/Get the DirectionMatrix. DirectionMatrix is used when the image axis are not
     the naive identity matrix: axis permutations and flips are possible. If this matrix
     is not orthogonal, it will be orthogonalized first. Then, the closest possible
     permutation/flip matrix will be inferred and stored. Finally, it is used to feed
     the vtkImageReslice in combination with the ResliceMatrix.
   */
  virtual void SetDirectionMatrix (vtkMatrix4x4*);
  vtkGetObjectMacro (DirectionMatrix, vtkMatrix4x4);

  
  // derived from vtkSynchronizedView
  virtual void Initialize();
  virtual void Uninitialize();
  
  
 protected:
  vtkViewImage();
  ~vtkViewImage();


  /** Register the image. Internal Use Only. */
  virtual void RegisterImage (vtkImageData* image);

  //BTX
  std::vector<vtkDataSet*>      DataSetList;
  std::vector<vtkActor*>        DataSetActorList;
  //ETX

  vtkSetMacro (FirstImage, int);
  vtkGetMacro (FirstImage, int);

  vtkSetMacro (FirstResetWindowLevel, int);
  vtkGetMacro (FirstResetWindowLevel, int);

 private:

  /// VTK image
  vtkImageData*   Image;
  vtkImageData*   OverlappingImage;

  vtkImageData*   MaskImage;
  vtkLookupTable* MaskLUT;

  double          CurrentPoint[3];

  vtkTransform*   Transform;
  vtkScalarsToColors* LookupTable;

  vtkScalarBarActor*  ScalarBar;

  int            LinkWindowLevel;
  int            LinkPosition;
  int            LinkZoom;
  int            LinkVolumeIndex;
  int            LinkRender;

  int            Visibility;
  int            SizeDataVisibility;

  double          Scale;
  double          Shift;

  double          Window;
  double          Level;
  double          Zoom;

  //! The volume to be displayed.
  vtkIdType VolumeIndex;

    //! Template function which implements SetInput for all types. 
    // Defined locally and not exposed.
    template < class T >
    void SetITKImage (typename itk::Image<T, 3> *itkImage);
    
    //! Template function which implements SetInput4 for all types. 
    // Defined locally and not exposed.
    template < class T >
    void SetITKImage4 (typename itk::Image<T, 4> *itkImage);
    
    //! Template function which sets the time step. 
    // Defined locally and not exposed.
    template < class T >
    void SetVolumeIndex (vtkIdType volumeIndex);
    
  /**
     This pointer is used to store internally a reference to the
     current ITK->VTK converter, in order to prevent the image buffer
     to be deleted unexpectdely. See the SetITKImageInXXX for more
     information.
   */
   class vtkViewImageImplementation;
   vtkViewImageImplementation * Impl;

  //BTX    
    itk::ImageBase<3>::Pointer  ITKImage;
    itk::ImageBase<4>::Pointer  ITKImage4;    
  //ETX

  //BTX
  std::string     SizeData_str;
  std::string     PatientNameData;
  std::string     StudyNameData;
  std::string     SerieNameData;
  //ETX

  int           ResetWindowLevelMode;
  double        WindowLevelPercentage;

  int           FirstImage;
  int           FirstResetWindowLevel;

  double        WindowForReset;
  double        LevelForReset;

  vtkMatrix4x4* DirectionMatrix;

};


