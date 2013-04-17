/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkRenderingAddOn/vtkViewImage2D.h>

#include "vtkInteractorObserver.h"
#include "vtkInteractorStyleSwitch.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRendererCollection.h"
#include "vtkProp.h"
#include "vtkTextActor.h"
#include "vtkCoordinate.h"
#include "vtkProperty.h"
#include "vtkProperty2D.h"
#include "vtkTextProperty.h"
#include <vtkCornerAnnotation.h>
#include <vtkLightCollection.h>
#include <vtkLight.h>
#include <vtkOrientationAnnotation.h>

#include "assert.h"
#include <iostream>
#include <sstream>
#include <cmath>

#include <vtkRenderingAddOn/vtkViewImage2DCommand.h>
#include <vtkRenderingAddOn/vtkViewImage2DFullCommand.h>
#include <vtkRenderingAddOn/vtkInteractorStyleImage2D.h>
#include "vtkCamera.h"
#include "vtkImageReslice.h"
#include "vtkRenderWindow.h"
#include "vtkTransform.h"
#include "vtkImageMapToWindowLevelColors.h"
#include "vtkImageMapToColors.h"
#include "vtkImageActor.h"
#include "vtkImageData.h"
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkLineSource.h"
#include "vtkLookupTable.h"
#include <vtkRenderingAddOn/vtkImageBlendWithMask.h>
#include "vtkImageBlend.h"
#include <vtkPolyDataMapper2D.h>
#include <vtkActor2D.h>
#include <vtkMatrix4x4.h>

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
#include <vtkTransform.h>
#include <vtkMath.h>
#include <vtkRulerWidget.h>
#include <vtkDistanceWidget.h>


vtkCxxRevisionMacro(vtkViewImage2D, "$Revision: 1367 $");
vtkStandardNewMacro(vtkViewImage2D);

// Initialize static member that controls display convention (0: radiologic, 1: neurologic)
static int vtkViewImage2DDisplayConventions = 0;


void vtkViewImage2D::SetViewImage2DDisplayConventions(int val)
{
  vtkViewImage2DDisplayConventions = val;
}


int vtkViewImage2D::GetViewImage2DDisplayConventions()
{
  return vtkViewImage2DDisplayConventions;
}


vtkViewImage2D::vtkViewImage2D()
{
  this->FirstRender = 1;
  this->ShowSliceNumber  = 1;
  this->InteractionStyle = SELECT_INTERACTION;
  this->LeftButtonInteractionStyle = SELECT_INTERACTION;
  this->MiddleButtonInteractionStyle = SELECT_INTERACTION;
  this->RightButtonInteractionStyle = SELECT_INTERACTION;
  this->WheelInteractionStyle = SELECT_INTERACTION;
  this->Show2DAxis = 1;
  this->RulerWidgetVisibility = 1;
  this->DistanceWidgetVisibility = 0;
  this->CursorFollowMouse = 1;

  this->Conventions = RADIOLOGIC;

  this->InitialParallelScale = 1.0;


  this->ImageReslice     = vtkImageReslice::New();
  this->ImageActor       = vtkImageActor::New();
  this->WindowLevelForCorner = vtkImageMapToWindowLevelColors::New();
  this->WindowLevel      = vtkImageMapToColors::New();
  this->MaskFilter       = vtkImageBlendWithMask::New();
  this->Blender          = vtkImageBlend::New();

  this->WindowLevel->SetOutputFormatToRGB();

  
  this->OrientationMatrix = vtkMatrix4x4::New();
  this->OrientationMatrix->Identity();
  this->ScreenToRealWorldMatrix = vtkMatrix4x4::New();
  this->ScreenToRealWorldMatrix->Zero();
  this->ScreenToRealWorldMatrix->SetElement (2, 2, 1.0);

  this->AxialResliceMatrix = vtkMatrix4x4::New();
  this->AxialResliceMatrix->Zero();
  this->AxialResliceMatrix->SetElement (0, 0, 1.0);
  this->AxialResliceMatrix->SetElement (1, 1, -1.0);
  this->AxialResliceMatrix->SetElement (2, 2,  1.0);
  this->AxialResliceMatrix->SetElement (3, 3,  1.0);

  this->CoronalResliceMatrix = vtkMatrix4x4::New();
  this->CoronalResliceMatrix->Zero();
  this->CoronalResliceMatrix->SetElement (0, 0, 1.0);
  this->CoronalResliceMatrix->SetElement (1, 2, 1.0);
  this->CoronalResliceMatrix->SetElement (2, 1, 1.0);
  this->CoronalResliceMatrix->SetElement (3, 3, 1.0);
  
  this->SagittalResliceMatrix = vtkMatrix4x4::New();
  this->SagittalResliceMatrix->Zero();
  this->SagittalResliceMatrix->SetElement (0, 2, 1.0);
  this->SagittalResliceMatrix->SetElement (1, 0, 1.0);
  this->SagittalResliceMatrix->SetElement (2, 1, 1.0);
  this->SagittalResliceMatrix->SetElement (3, 3, 1.0);
  
  /*
    this->BGActor          = vtkImageActor::New();
    this->BGWindowLevel    = vtkImageMapToColors::New();
    this->BGImage = vtkImageData::New();
  */
  this->BGActor          = 0;
  this->BGWindowLevel    = 0;
  this->BGImage = 0;

  this->HorizontalLineSource = vtkLineSource::New();
  this->VerticalLineSource   = vtkLineSource::New();
  this->HorizontalLineActor  = vtkActor::New();
  this->VerticalLineActor    = vtkActor::New();

  this->DataSetCutPlane      = vtkPlane::New();
  this->DataSetCutBox        = vtkBox::New();


  this->DataSetCutPlane->SetOrigin (0,0,0);
  this->DataSetCutPlane->SetNormal (0,0,1);
  this->DataSetCutBox->SetBounds (0,0,0,0,0,0);
  this->BoxThickness = 2;


  // set the filters properties
  this->Blender->SetBlendModeToNormal();
  this->Blender->SetOpacity (0,0.25 );
  this->Blender->SetOpacity (1, 0.75);


  // set up the vtk pipeline
  this->ImageReslice->SetOutputDimensionality(2);
  this->ImageReslice->InterpolateOff();
  this->ImageReslice->SetInputConnection( this->WindowLevel->GetOutputPort() );


  this->AuxInput     = this->WindowLevel->GetOutput();
  this->ResliceInput = this->WindowLevel->GetOutput();


  // Interactor Style
  this->SetInteractionStyle( vtkViewImage2D::SELECT_INTERACTION );


  // Initialize cursor lines
  vtkPolyDataMapper* mapper =  vtkPolyDataMapper::New();
  mapper->SetInputConnection( this->HorizontalLineSource->GetOutputPort() );

  this->HorizontalLineActor->SetMapper(mapper);
  this->HorizontalLineActor->GetProperty()->SetColor (1.0,0.0,0.0);
  mapper->Delete();
  this->HorizontalLineActor->SetVisibility (0);
  this->HorizontalLineActor->PickableOff();
  this->HorizontalLineActor->DragableOff();
 


  vtkPolyDataMapper* mapper2 =  vtkPolyDataMapper::New();
  mapper2->SetInputConnection( this->VerticalLineSource->GetOutputPort() );
  this->VerticalLineActor->SetMapper(mapper2);
  this->VerticalLineActor->GetProperty()->SetColor (1.0,0.0,0.0);
  mapper2->Delete();
  this->VerticalLineActor->SetVisibility (0);
  this->VerticalLineActor->PickableOff();
  this->VerticalLineActor->DragableOff();

  this->GetCornerAnnotation()->SetWindowLevel ( this->WindowLevelForCorner );

  this->Orientation = vtkViewImage::NB_PLAN_IDS;
  this->ViewOrientation = vtkViewImage::NB_PLAN_IDS;
  this->SetOrientation ( vtkViewImage::AXIAL_ID );

  if( vtkViewImage2D::GetViewImage2DDisplayConventions()==0 )
  {
    this->SetConventionsToRadiological();
  }
  else
  {
    this->SetConventionsToNeurological();
  }

  this->RulerWidget = vtkRulerWidget::New();
  this->RulerWidget->KeyPressActivationOff();

  this->DistanceWidget = vtkDistanceWidget::New();
  //this->DistanceWidget->KeyPressActivationOff();
  this->DistanceWidget->SetKeyPressActivationValue ('d');
  this->DistanceWidget->CreateDefaultRepresentation();


  this->SetBackgroundColor(0,0,0);

  /**
     For the moment we do not transpose patient / study / series information from DICOM exam to views.
     In the meanwhile we let the information blank :
  */
  this->SetPatientNameData (" ");
  this->SetStudyNameData (" ");
  this->SetSerieNameData (" ");
  /**
     By default the feature allowing the axes to follow the mouse movement is desabled
  */
  this->CursorFollowMouse = 0;

  
  this->SliceNavigation = 1;
}



vtkViewImage2D::~vtkViewImage2D()
{
  this->ImageActor->Delete();
  this->WindowLevel->Delete();
  this->WindowLevelForCorner->Delete();
  this->ImageReslice->Delete();
  this->MaskFilter->Delete();
  this->Blender->Delete();

  //this->BGActor->Delete();
  //this->BGWindowLevel->Delete();
  /*
    if( this->BGImage )
    {
    this->BGImage->Delete();
    }
  */

  this->OrientationMatrix->Delete();
  this->ScreenToRealWorldMatrix->Delete();

  this->HorizontalLineSource->Delete();
  this->HorizontalLineActor->Delete();

  this->VerticalLineSource->Delete();
  this->VerticalLineActor->Delete();

  this->DataSetCutPlane->Delete();
  this->DataSetCutBox->Delete();

  this->RulerWidget->Delete();

  this->DistanceWidget->Delete();

  this->AxialResliceMatrix->Delete();
  this->CoronalResliceMatrix->Delete();
  this->SagittalResliceMatrix->Delete();
}


void vtkViewImage2D::InternalUpdate()
{
  this->UpdateImageActor();
  //  this->UpdatePosition();
  this->InitializeImagePositionAndSize();
}


void vtkViewImage2D::InitializeInteractor()
{
  if( this->GetRenderWindowInteractor() )
  {
    vtkInteractorStyleImage2D *interactor = vtkInteractorStyleImage2D::New();
    interactor->SetView(this);

    vtkViewImage2DCommand *cbk = vtkViewImage2DCommand::New();
    cbk->SetView(this);

    interactor->AddObserver(vtkCommand::KeyPressEvent, cbk);
    interactor->AddObserver(vtkCommand::WindowLevelEvent, cbk);
    interactor->AddObserver(vtkCommand::StartWindowLevelEvent, cbk);
    interactor->AddObserver(vtkCommand::ResetWindowLevelEvent, cbk);
    interactor->AddObserver(vtkCommand::EndWindowLevelEvent, cbk);
    interactor->AddObserver(vtkCommand::PickEvent, cbk);
    interactor->AddObserver(vtkCommand::StartPickEvent, cbk);
    interactor->AddObserver(vtkCommand::EndPickEvent, cbk);
    interactor->AddObserver(vtkViewImage2DCommand::ResetZoomEvent, cbk);
    interactor->AddObserver(vtkViewImage2DCommand::ResetPositionEvent, cbk);
    interactor->AddObserver(vtkViewImage2DCommand::StartZSliceMoveEvent, cbk);
    interactor->AddObserver(vtkViewImage2DCommand::ZSliceMoveEvent, cbk);
    interactor->AddObserver(vtkViewImage2DCommand::EndZSliceMoveEvent, cbk);
    interactor->AddObserver(vtkViewImage2DCommand::StartMeasureEvent, cbk);
    interactor->AddObserver(vtkViewImage2DCommand::MeasureEvent, cbk);
    interactor->AddObserver(vtkViewImage2DCommand::EndMeasureEvent, cbk);
    interactor->AddObserver(vtkViewImage2DCommand::FullPageEvent, cbk);
    interactor->AddObserver(vtkViewImage2DCommand::ZoomEvent, cbk);
    interactor->AddObserver(vtkViewImage2DCommand::DefaultMoveEvent, cbk);

    this->GetRenderWindowInteractor()->SetInteractorStyle( interactor );

    this->RulerWidget->SetInteractor ( this->GetRenderWindowInteractor() );

    this->DistanceWidget->SetInteractor ( this->GetRenderWindowInteractor() );

    if( !this->GetFirstImage() && this->RulerWidgetVisibility )
    {
       this->RulerWidget->On();
    }

    if( !this->GetFirstImage() && this->DistanceWidgetVisibility )
    {
       this->DistanceWidget->On();
    }

    interactor->Delete();
    cbk->Delete();
  }
}


void vtkViewImage2D::Initialize()
{
  this->Superclass::Initialize();

  if (this->GetRenderer())
  {
    this->GetRenderer()->TwoSidedLightingOff();
  }

  this->InitializeInteractor();
}


void vtkViewImage2D::UninitializeInteractor()
{
  this->RulerWidget->SetInteractor ( NULL );
  this->DistanceWidget->SetInteractor ( NULL );

  if ( this->GetRenderWindowInteractor() )
  {
    this->GetRenderWindowInteractor()->SetInteractorStyle( NULL );
  }
}


void vtkViewImage2D::Uninitialize()
{
  this->RemoveActor( this->HorizontalLineActor );
  this->RemoveActor( this->VerticalLineActor );
  this->RemoveActor( this->ImageActor );
  for( unsigned int i=0; i<this->DataSetActorList.size(); i++)
    this->RemoveActor( this->DataSetActorList[i] );

  this->UninitializeInteractor();
  
  vtkViewImage::Uninitialize();
}


void vtkViewImage2D::SetVisibility (int state)
{
  vtkViewImage::SetVisibility ( state );
  this->ImageActor->SetVisibility ( state );
}


void vtkViewImage2D::UpdateImageActor()
{
  if( !this->GetImage() )
  {
    return;
  }

  this->ImageReslice->GetInput()->SetUpdateExtentToWholeExtent();
  this->ImageReslice->GetInput()->Update();
 
  this->ImageReslice->GetOutput()->UpdateInformation();
  this->ImageActor->GetInput()->SetUpdateExtent( this->ImageReslice->GetOutput()->GetWholeExtent() );
  this->ImageActor->SetDisplayExtent( this->ImageReslice->GetOutput()->GetWholeExtent() );
  this->FirstRender = 1;
}


void vtkViewImage2D::InitializeImagePositionAndSize()
{
  if( !this->GetImage() )
  {
    return;
  }


  if (this->FirstRender)
  {
    // make sur the input is up-to-date
    this->ImageActor->GetInput()->Update();


    this->GetRenderer()->GetActiveCamera()->OrthogonalizeViewUp();
    this->GetRenderer()->GetActiveCamera()->ParallelProjectionOn();


    // Get the current position of the camera:
    //double Pos[3];
    //this->GetRenderer()->GetActiveCamera()->GetPosition(Pos);


    // Get the bounds of the image: coordinates in the real world
    double bnds[6];
    this->ImageActor->GetBounds(bnds);


    // extension of the image:
    double xs = (bnds[1] - bnds[0]);
    double ys = (bnds[3] - bnds[2]);

    double bnds_x = xs/2.0;
    double bnds_y = ys/2.0;

    //this->GetRenderer()->GetActiveCamera()->SetParallelScale(xs < ys ? bnds_y:bnds_x);
    //this->InitialParallelScale = this->GetRenderer()->GetActiveCamera()->GetParallelScale();
    this->InitialParallelScale = xs < ys ? bnds_y:bnds_x;

    /**
       Somehow, when the axes are present, they screw up the ResetZoom
       function because they are taken into account when computing the
       bounds of all actors. We need to switch them off and on after
       the call to ResetZoom().
    */
    this->VerticalLineActor->SetVisibility (0);
    this->HorizontalLineActor->SetVisibility (0);

    double zoom = this->GetZoom();
    this->ResetZoom();
    this->SetZoom (zoom);

    if( this->Show2DAxis ){
      this->VerticalLineActor->SetVisibility (1);
      this->HorizontalLineActor->SetVisibility (1);
    }

    this->FirstRender = 0;

  }
}

 

void vtkViewImage2D::SetShowSliceNumber(int p_showSliceNumber)
{
  this->ShowSliceNumber = p_showSliceNumber;

  if (!this->ShowSliceNumber)
  {
    this->SetUpRightAnnotation("");
  }

  this->Modified();
}


int vtkViewImage2D::GetWholeZMin()
{
  return 0;
}


int vtkViewImage2D::GetWholeZMax()
{
  if(!this->GetImage())
  {
    return 0;
  }

  int* ext = this->GetImage()->GetWholeExtent();

  assert(this->Orientation<vtkViewImage::NB_DIRECTION_IDS);

  int axis = this->GetOrthogonalAxis(this->Orientation);
  return ext[2*axis+1];
}


int vtkViewImage2D::GetZSlice()
{
  return this->GetSlice(this->Orientation);
}


void vtkViewImage2D::SetZSlice(int p_zslice)
{
  this->SetSlice(this->Orientation, p_zslice);
  this->Modified();
}


void vtkViewImage2D::UpdatePosition ()
{
  if( !this->GetImage() )
  {
    return;
  }

  double x=0;
  double y=0;
  double max_x=0;
  double max_y=0;
  double min_x=0;
  double min_y=0;
  double pos[4];

  this->GetCurrentPoint(pos);
  pos[3] = 1.0;

  double spacing[3];
  this->GetImage()->GetSpacing( spacing );

  double min_bounds[4] = { this->GetWholeMinPosition(0),
			   this->GetWholeMinPosition(1),
			   this->GetWholeMinPosition(2),
			   0.0 };

  double max_bounds[4] = { this->GetWholeMaxPosition(0),
			   this->GetWholeMaxPosition(1),
			   this->GetWholeMaxPosition(2),
			   0.0 };
  

  // new efficient way to get the reslice origin and the axis coordinates.
  double reslice[4], pos2[4];
  this->ScreenToRealWorldMatrix->MultiplyPoint (pos, reslice);
  this->ImageReslice->SetResliceAxesOrigin(reslice[0], reslice[1], reslice[2]);

  vtkMatrix4x4* mat = vtkMatrix4x4::New();
  vtkMatrix4x4::Transpose (this->OrientationMatrix, mat);
  mat->MultiplyPoint (pos, pos2);
  mat->MultiplyPoint (min_bounds, min_bounds);
  mat->MultiplyPoint (max_bounds, max_bounds);
  mat->Delete();

  x = pos2[0];
  y = pos2[1];

  max_x = max_bounds[0];
  max_y = max_bounds[1];
  
  min_x = min_bounds[0];
  min_y = min_bounds[1];


  this->HorizontalLineSource->SetPoint1(min_x, y, 0.001);
  this->HorizontalLineSource->SetPoint2(max_x, y, 0.001);
  this->VerticalLineSource->SetPoint1(x, min_y, 0.001);
  this->VerticalLineSource->SetPoint2(x, max_y, 0.001);
  

  this->ImageReslice->Update(); // needed to update input Extent


  int imCoor[3];
  this->GetCurrentVoxelCoordinates(imCoor);
  
  int dims[3];
  this->GetImage()->GetDimensions (dims);
  
  std::ostringstream os;
  std::ostringstream os2;
  
  int orthogonalAxis = this->GetOrthogonalAxis (this->Orientation);

  int xaxis = 0;
  int yaxis = 1;
  
  switch( this->Orientation )
  {
      case vtkViewImage::Z_ID:
	xaxis = this->GetOrthogonalAxis(SAGITTAL_ID);
	yaxis = this->GetOrthogonalAxis(CORONAL_ID);
	break;
	  
      case vtkViewImage::Y_ID:
	xaxis = this->GetOrthogonalAxis(SAGITTAL_ID);
	yaxis = this->GetOrthogonalAxis(AXIAL_ID);
	break;
	
      case vtkViewImage::X_ID:
      default:
	xaxis = this->GetOrthogonalAxis(CORONAL_ID);
	yaxis = this->GetOrthogonalAxis(AXIAL_ID);
	break;
  }

  os << "Image size: " << dims[xaxis] << " x " << dims[yaxis] << "\n";
  os << "Voxel size: " << spacing[xaxis] << " x " << spacing[yaxis] << "\n";
  os << "X: " << imCoor[xaxis] << " px Y: " << imCoor[yaxis] << " px Value: "
     << this->GetCurrentPointDoubleValue() << std::endl;
  os << "X: " << pos[xaxis] << " mm Y: " << pos[yaxis] << " mm\n";

  os2 << "Zoom: " << this->GetZoom()*100.0 << " %\n";
  if ( this->ShowSliceNumber )
    os2 << "Slice: " << imCoor[orthogonalAxis] << " / " << dims[orthogonalAxis]-1 << std::endl;
  os2 << "Location: " << pos[orthogonalAxis] << " mm";

  os << "<window_level>";
  
  this->SetUpLeftAnnotation( os.str().c_str() );
  this->SetDownLeftAnnotation( os2.str().c_str() );


  vtkMatrix4x4 *t_direction = this->GetDirectionMatrix();
  t_direction->MultiplyPoint (reslice, reslice);
  
  this->DataSetCutPlane->SetOrigin (reslice[0], reslice[1], reslice[2]);

  //unsigned int direction = this->GetOrthogonalAxis (this->GetOrientation());
  unsigned int direction = this->GetOrientation();
    
  switch(direction)
  {
      case X_ID :
        //this->DataSetCutPlane->SetNormal (1,0,0);
	this->DataSetCutBox->SetBounds (this->DataSetCutPlane->GetOrigin()[0],this->DataSetCutPlane->GetOrigin()[0]+this->BoxThickness,
					this->GetWholeMinPosition(1),this->GetWholeMaxPosition(1),
					this->GetWholeMinPosition(2),this->GetWholeMaxPosition(2));

	break;
      case Y_ID :
        //this->DataSetCutPlane->SetNormal (0,1,0);
	this->DataSetCutBox->SetBounds (this->GetWholeMinPosition(0),this->GetWholeMaxPosition(0),
					this->DataSetCutPlane->GetOrigin()[1],this->DataSetCutPlane->GetOrigin()[1]+this->BoxThickness,
					this->GetWholeMinPosition(2),this->GetWholeMaxPosition(2));
	break;
      case Z_ID :
        //this->DataSetCutPlane->SetNormal (0,0,1);
	this->DataSetCutBox->SetBounds (this->GetWholeMinPosition(0),this->GetWholeMaxPosition(0),
					this->GetWholeMinPosition(1), this->GetWholeMaxPosition(1),
					this->DataSetCutPlane->GetOrigin()[2], this->DataSetCutPlane->GetOrigin()[2]+this->BoxThickness);
	break;
  }

  if( this->DataSetList.size() )
  {

    this->ResetAndRestablishZoomAndCamera();

    /*
      We need to correct for the origin of the actor. Indeed, the ImageActor
      has always position 0 in Z in axial view, in X in sagittal view and
      in Y in coronal view. The projected dataset have an origin that depends
      on the required slice and can be negative. In that case, the projected
      data are behind the image actor and thus not visible. Here, we correct
      this by translating the actor so that it becomes visible.
     */
    for( unsigned int i=0; i<this->DataSetActorList.size(); i++)
    {
      double Pos[3];
      this->DataSetActorList[i]->GetPosition (Pos);

      switch(direction)
      {
          case X_ID :
            Pos[0] = -1.0*reslice[0] + 1.0;
            break;

          case Y_ID:
            Pos[1] = -1.0*reslice[1] + 1.0;
            break;

          case Z_ID:
            Pos[2] = -1.0*reslice[2] + 1.0;
            break;
      }

      this->DataSetActorList[i]->SetPosition (Pos);
    }

  }

  this->Modified();
}


void vtkViewImage2D::SetWindow (double w)
{

  if( w<0.0 )
  {
    w = 0.0;
  }

  double shiftScaleWindow = this->GetShift() + w*this->GetScale();

  vtkViewImage::SetWindow ( shiftScaleWindow );
  this->WindowLevelForCorner->SetWindow( shiftScaleWindow );

  double v_min = this->GetLevel() - 0.5*this->GetWindow();
  double v_max = this->GetLevel() + 0.5*this->GetWindow();

  if( this->GetLookupTable() && this->WindowLevel->GetLookupTable())
  {

    this->GetLookupTable()->SetRange ( (v_min-0.5*this->GetShift())/this->GetScale(),
                                       (v_max-1.5*this->GetShift())/this->GetScale());
    this->WindowLevel->GetLookupTable()->SetRange (v_min, v_max);
  }

  if (this->BGWindowLevel && this->BGWindowLevel->GetLookupTable())
      this->BGWindowLevel->GetLookupTable()->SetRange (v_min, v_max);


}


void vtkViewImage2D::SetLevel (double l)
{

  double shiftScaleLevel = this->GetShift() + l*this->GetScale();

  vtkViewImage::SetLevel ( shiftScaleLevel );
  this->WindowLevelForCorner->SetLevel( shiftScaleLevel );

  double v_min = this->GetLevel() - 0.5*this->GetWindow();
  double v_max = this->GetLevel() + 0.5*this->GetWindow();

  if( this->GetLookupTable() && this->WindowLevel->GetLookupTable())
  {
    this->GetLookupTable()->SetRange ( (v_min-0.5*this->GetShift())/this->GetScale(),
                                       (v_max-1.5*this->GetShift())/this->GetScale());
    this->WindowLevel->GetLookupTable()->SetRange (v_min, v_max);
  }
}


double vtkViewImage2D::GetColorWindow()
{
  return this->GetWindow();
}


double  vtkViewImage2D::GetColorLevel()
{
  return this->GetLevel();
}


void vtkViewImage2D::SetTransform(vtkTransform* p_transform)
{
  this->ImageReslice->SetResliceTransform(p_transform);
  this->Modified();
}


void vtkViewImage2D::SetMatrix(vtkMatrix4x4* p_matrix, bool centered)
{
    if(!p_matrix)
	return;

    if(this->ImageActor) {
	if(centered) {
	    //we need to decompose the matrix.:
	    //1. move the image to the center
	    //2. rotate the image with rotation in p_matrix
	    //3. move the image back to its place
	    //4. apply the translation contained in p_matrix

	    double *propCenter = 0;
	    double *propPosition = 0;

	    propCenter = this->ImageActor->GetCenter();
	    propPosition = this->ImageActor->GetPosition();

	    vtkMatrix4x4 *finalMat = vtkMatrix4x4::New();

	    vtkMatrix4x4 *toCenterMat = vtkMatrix4x4::New();
	    toCenterMat->Identity();
	    (*toCenterMat)[0][3] = - ( propCenter[0] - propPosition[0] );
	    (*toCenterMat)[1][3] = - ( propCenter[1] - propPosition[1] );
	    (*toCenterMat)[2][3] = - ( propCenter[2] - propPosition[2] );

	    vtkMatrix4x4 *rotationMat = vtkMatrix4x4::New();
	    rotationMat->Identity();
	    (*rotationMat)[0][0] = (*p_matrix)[0][0];
	    (*rotationMat)[0][1] = (*p_matrix)[0][1];
	    (*rotationMat)[0][2] = (*p_matrix)[0][2];

	    (*rotationMat)[1][0] = (*p_matrix)[1][0];
	    (*rotationMat)[1][1] = (*p_matrix)[1][1];
	    (*rotationMat)[1][2] = (*p_matrix)[1][2];

	    (*rotationMat)[2][0] = (*p_matrix)[2][0];
	    (*rotationMat)[2][1] = (*p_matrix)[2][1];
	    (*rotationMat)[2][2] = (*p_matrix)[2][2];

	    vtkMatrix4x4 *fromCenterMat = vtkMatrix4x4::New();
	    fromCenterMat->Identity();
	    (*fromCenterMat)[0][3] = ( propCenter[0] - propPosition[0] );
	    (*fromCenterMat)[1][3] = ( propCenter[1] - propPosition[1] );
	    (*fromCenterMat)[2][3] = ( propCenter[2] - propPosition[2] );

 	    vtkMatrix4x4 *translationMat = vtkMatrix4x4::New();
	    translationMat->Identity();
	    (*translationMat)[0][3] = (*p_matrix)[0][3];
	    (*translationMat)[1][3] = (*p_matrix)[1][3];
	    (*translationMat)[2][3] = (*p_matrix)[2][3];

	    vtkMatrix4x4::Multiply4x4(rotationMat, toCenterMat, finalMat);
	    vtkMatrix4x4::Multiply4x4(fromCenterMat, finalMat, finalMat);
	    vtkMatrix4x4::Multiply4x4(translationMat, finalMat, finalMat);

	    this->ImageActor->SetUserMatrix(finalMat);
	    finalMat->Delete();
	    toCenterMat->Delete();
	    rotationMat->Delete();
	    translationMat->Delete();
	}
	else {
	    this->ImageActor->SetUserMatrix(p_matrix);
	}
	this->ImageActor->GetUserMatrix()->PrintSelf(std::cout, vtkIndent());
    }

    this->Modified();
}


void vtkViewImage2D::SetImage(vtkImageData* image)
{
  if(!image)
  {
    return;
  }

  int* extent = image->GetExtent();
  if( extent[1]<extent[0] || extent[3]<extent[2] || extent[5]<extent[4] )
  {
    vtkErrorMacro ( << "Image extent is not valid: " << extent[0] << " "
		    << extent[1] << " "
		    << extent[2] << " "
		    << extent[3] << " "
		    << extent[4] << " "
		    << extent[5]);
    return;
  }

  vtkViewImage::SetImage( image );

  // check if there is a mask image. If yes, then we check
  // if the new image size and spacing agrees with the mask image.
  // If not, we remove the mask image
  if( this->GetMaskImage() )
  {
    int*    dims        = image->GetDimensions();
    double* spacing     = image->GetSpacing();
    int*    maskDims    = this->GetMaskImage()->GetDimensions();
    double* maskSpacing = this->GetMaskImage()->GetSpacing();

    if( dims[0]!=maskDims[0] || dims[1]!=maskDims[1] || dims[2]!=maskDims[2] ||
        spacing[0]!=maskSpacing[0] || spacing[1]!=maskSpacing[1] || spacing[2]!=maskSpacing[2] )
    {
      this->RemoveMaskImage();
    }
  }


  if( image->GetScalarType() == VTK_UNSIGNED_CHAR  && (image->GetNumberOfScalarComponents()==3 || image->GetNumberOfScalarComponents()==4) )
  {
    this->AuxInput = image;
  }
  else
  {
    this->AuxInput = this->WindowLevel->GetOutput();

    this->WindowLevel->SetInput (image);
    double range[2];
    image->GetScalarRange (range);
    if ( this->WindowLevel->GetLookupTable() )
    {
      this->WindowLevel->GetLookupTable()->SetRange (range);
    }
  }


  if( this->GetOverlappingImage() )
  {
    this->Blender->SetInput (0, this->AuxInput );
  }
  else
  {
    if( this->GetMaskImage() )
    {
      this->MaskFilter->SetImageInput ( this->AuxInput );
    }
    else
    {
      this->ImageReslice->SetInput ( this->AuxInput );
      this->ResliceInput = this->AuxInput;
    }
  }

  this->ImageActor->SetInput( this->ImageReslice->GetOutput() );

  this->AddActor(this->HorizontalLineActor);
  this->AddActor(this->VerticalLineActor);

  this->AddActor(this->ImageActor);


  // Save the camera focal and position, and zoom, before calling InternalUpdate()
  double focal[3], pos[3], zoom;
  this->GetCameraFocalAndPosition (focal, pos);
  zoom = this->GetZoom();
  

  this->SetupAnnotations();
  this->InternalUpdate();

  
  // Check that the current displayed point lies within the bounds of the image. If not,
  // we camp it to the nearest acceptable position.
  double *wextent = image->GetBounds();
  double position[3];
  this->GetCurrentPoint ( position );
  position[0]<wextent[0]?position[0]=wextent[0]:position[0];
  position[0]>wextent[1]?position[0]=wextent[1]:position[0];
  position[1]<wextent[2]?position[1]=wextent[2]:position[1];
  position[1]>wextent[3]?position[1]=wextent[3]:position[1];
  position[2]<wextent[4]?position[2]=wextent[4]:position[2];
  position[2]>wextent[5]?position[2]=wextent[5]:position[2];
  this->SetCurrentPoint ( position );


  
  /**
     The first time ResetWindowLevel is called, optimal window/level values are
     stored internally for efficiency purposes when Reset is called a second time.
     When a new image is set, we have to force the calculation of the new optimal
     window/level values.
   */
  this->SetFirstResetWindowLevel ( 1 );

  // restore the window/level, zoom and focal in case this is not the first time we render an image.

  if( !this->GetFirstImage() )
  {
    this->SetWindow( this->GetWindow() );
    this->SetLevel( this->GetLevel() );
    this->SetZoom ( zoom );
    this->SetCameraFocalAndPosition (focal, pos);
  }

  if( this->GetRenderWindowInteractor() && this->RulerWidgetVisibility )
    this->RulerWidget->On();

  if( this->GetRenderWindowInteractor() && this->DistanceWidgetVisibility )
    this->DistanceWidget->On();

  this->SetFirstImage (0);

}


void vtkViewImage2D::SetLookupTable (vtkScalarsToColors* lut)
{

  if( !lut )
  {
    return;
  }

  vtkViewImage::SetLookupTable (lut);

  double v_min = this->GetLevel() - 0.5*this->GetWindow();
  double v_max = this->GetLevel() + 0.5*this->GetWindow();

  /**
     In the case of a shift/scale, one must set the lut range to values
     without this shift/scale, because the object can be shared by different
     views.
   */
  lut->SetRange ( (v_min-0.5*this->GetShift())/this->GetScale(),
                  (v_max-1.5*this->GetShift())/this->GetScale() );


  vtkScalarsToColors *windowLevelLUT = 0;

  if (vtkLookupTable* realLut = vtkLookupTable::SafeDownCast (lut))
  {
    vtkLookupTable* newLut = vtkLookupTable::New();
    newLut->DeepCopy (realLut);
    newLut->SetRange (v_min, v_max);
    windowLevelLUT = newLut;
  }
  else
  {
    vtkErrorMacro ( << "Error: Cannot cast vtkScalarsToColors to concrete type.");
    return;
  }

  /**
     Due to the same problem as above (shift/scale), one must copy the lut
     so that it does not change values of the shared object.
   */
  
  this->WindowLevel->SetLookupTable (windowLevelLUT);
  windowLevelLUT->Delete();
}


void vtkViewImage2D::SetBGLookupTable (vtkScalarsToColors* lut)
{
  this->BGLookupTable = lut;
  double v_min = this->GetLevel() - 0.5*this->GetWindow();
  double v_max = this->GetLevel() + 0.5*this->GetWindow();

  /**
     In the case of a shift/scale, one must set the lut range to values
     without this shift/scale, because the object can be shared by different
     views.
   */
  lut->SetRange ( (v_min-0.5*this->GetShift())/this->GetScale(),
                  (v_max-1.5*this->GetShift())/this->GetScale() );


  vtkLookupTable* realLut = vtkLookupTable::SafeDownCast (lut);

  if( !realLut )
  {
    std::cerr << "Error: Cannot cast vtkScalarsToColors to vtkLookupTable." << std::endl;
    return;
  }

  /**
     Due to the same problem as above (shift/scale), one must copy the lut
     so that it does not change values of the shared object.
   */
  vtkLookupTable* newLut = vtkLookupTable::New();
  newLut->DeepCopy (realLut);
  newLut->SetRange (v_min, v_max);
  this->BGWindowLevel->SetLookupTable (newLut);
  newLut->Delete();

  this->Modified();
}


vtkScalarsToColors* vtkViewImage2D::GetBGLookupTable() const
{
    return this->BGLookupTable;
}


void vtkViewImage2D::SetOrientation(unsigned int p_orientation)
{
    this->ViewOrientation = p_orientation;
  
    unsigned int sliceorientation = 0;
    double dot = 0;
    
    if ( this->GetDirectionMatrix() )
     {
        for (unsigned int i=0; i<3; i++)
         {
            if (dot < std::abs (this->GetDirectionMatrix()->GetElement (i, p_orientation)))
             {
                dot = std::abs (this->GetDirectionMatrix()->GetElement (i, p_orientation));
                sliceorientation = i;
             }
         }
     }
    
    if( (sliceorientation > vtkViewImage::NB_PLAN_IDS - 1) || (this->Orientation == sliceorientation) )
     {
        return;
     }
    
    this->Orientation = sliceorientation;
    
  
  // setup the OrientationMatrix and the ScreenToRealWorldMatrix. Then, we have no
  // need to check the view's orientation to determine what slice to display: a
  // simple matrix multiplication will tell it for us.
  this->OrientationMatrix->Zero();
  this->ScreenToRealWorldMatrix->Zero();


  vtkMatrix4x4 *directions = this->GetDirectionMatrix();
  vtkMatrix4x4 *resultMatrix  = vtkMatrix4x4::New();
  vtkMatrix4x4 *resliceMatrix = 0;


  switch (this->Orientation)
  {
    
      case vtkViewImage::SAGITTAL_ID:
	resliceMatrix = this->GetSagittalResliceMatrix();
	break;

      case vtkViewImage::CORONAL_ID:
	resliceMatrix = this->GetCoronalResliceMatrix();
	break;

      case vtkViewImage::AXIAL_ID:
      default:
	resliceMatrix = this->GetAxialResliceMatrix();
	break;
  }


  vtkMatrix4x4 *t_directions = vtkMatrix4x4::New();
  vtkMatrix4x4::Transpose (directions, t_directions);
  vtkMatrix4x4::Multiply4x4 (t_directions, resliceMatrix, resultMatrix);

  this->OrientationMatrix->DeepCopy (resultMatrix);
  
  int indexForScreen = 0;
  this->SliceNavigation = 1;
  for (int i=0; i<3; i++)
  {
    if (resultMatrix->GetElement (i,2)<0.0)
      this->SliceNavigation = -1;
    
    resultMatrix->SetElement (i, 2, fabs(resultMatrix->GetElement (i,2)) );
    if (resultMatrix->GetElement (i,2)>0.0)
      indexForScreen = i;
    this->OrientationMatrix->SetElement (i, 2, 0.0);
  }
  this->ScreenToRealWorldMatrix->SetElement (indexForScreen, indexForScreen, 1.0);

  this->ImageReslice->SetResliceAxes ( resultMatrix );
  

  // unsigned int direction = this->GetOrthogonalAxis (this->GetOrientation());
  switch(this->Orientation)
  {
      case X_ID :
	this->DataSetCutPlane->SetNormal (1,0,0);
	break;
      case Y_ID :
	this->DataSetCutPlane->SetNormal (0,1,0);
	break;
      case Z_ID :
	this->DataSetCutPlane->SetNormal (0,0,1);
	break;
  }

  vtkMatrix4x4* matrix = vtkMatrix4x4::New();
  switch(this->Orientation)
  {
      case SAGITTAL_ID:
	vtkMatrix4x4::Transpose (this->SagittalResliceMatrix, matrix);
	break;
	
      case CORONAL_ID:
	vtkMatrix4x4::Transpose (this->CoronalResliceMatrix, matrix);
	break;
	
      case AXIAL_ID:
      default:
	vtkMatrix4x4::Transpose (this->AxialResliceMatrix, matrix);
	break;
  }

  for (unsigned int i=0; i<this->DataSetActorList.size(); i++)
  {
    this->DataSetActorList[i]->SetUserMatrix (matrix);
  }


  //this->ImageReslice->Modified();

  this->UpdateImageActor(); // make sure update extent are up to date
  this->UpdatePosition(); // make sure reslicer origin are ok
  this->InitializeImagePositionAndSize(); // reset and center the view
  this->SetupAnnotations(); // make sure annotations are ok

  resultMatrix->Delete();
  t_directions->Delete();
  matrix->Delete();
  
  this->Modified();
}


void vtkViewImage2D::SetupAnnotations()
{
  if( !this->GetImage() )
  {
    return;
  }
  
  //  int    *dims    = this->GetImage()->GetDimensions();
  //  double *spacing = this->GetImage()->GetSpacing();

  //  std::ostringstream os;
  // os << "Image Size: ";

  std::ostringstream os;
  
  if( strcmp (this->GetPatientNameData(),"")!=0 )
    os << this->GetPatientNameData() << "\n";
  else
    os << "No Patient Name\n";

  if( strcmp (this->GetStudyNameData(),"")!=0 )
    os << this->GetStudyNameData() << "\n";
  else
    os << "No Study Name\n";

  if( strcmp (this->GetSerieNameData(),"")!=0 )
    os << this->GetSerieNameData() << "\n";
  else
    os << "No Serie Name\n";

  this->SetUpRightAnnotation ( os.str().c_str() );

  if( this->Orientation == vtkViewImage::SAGITTAL_ID )
  {
    //    os << dims[1] << " x " << dims[2] << std::endl;
    //    os << "Voxel Size: " << spacing[1] << " x " << spacing[2] << " mm";
    
    this->SetNorthAnnotation("S");
    this->SetSouthAnnotation("I");
    this->SetEastAnnotation ("P");
    this->SetWestAnnotation("A");

    this->HorizontalLineActor->GetProperty()->SetColor (1.0,0.0,0.0);
    this->VerticalLineActor->GetProperty()->SetColor (0.0,1.0,0.0);
  }

  
  if( this->Orientation == vtkViewImage::CORONAL_ID )
  {
    //    os << dims[0] << " x " << dims[2] << std::endl;
    //    os << "Voxel Size: " << spacing[0] << " x " << spacing[2] << " mm";
    
    this->HorizontalLineActor->GetProperty()->SetColor (1.0,0.0,0.0);
    this->VerticalLineActor->GetProperty()->SetColor (0.0,0.0,1.0);
    
    this->SetNorthAnnotation("S");
    this->SetSouthAnnotation("I");
    if( this->Conventions == RADIOLOGIC)
    {
      this->SetEastAnnotation ("L");
      this->SetWestAnnotation("R");
    }
    else
    {
      this->SetEastAnnotation ("R");
      this->SetWestAnnotation("L");
    }
  }
  
  if( this->Orientation == vtkViewImage::AXIAL_ID )
  {
    //    os << dims[0] << " x " << dims[1] << std::endl;
    //    os << "Voxel Size: " << spacing[0] << " x " << spacing[1] << " mm";
    
    this->SetNorthAnnotation("A");
    this->SetSouthAnnotation("P");
    if( this->Conventions == RADIOLOGIC )
    {
      this->SetEastAnnotation ("L");
      this->SetWestAnnotation("R");
    }
    else
    {
      this->SetEastAnnotation ("R");
      this->SetWestAnnotation("L");
    }

    this->HorizontalLineActor->GetProperty()->SetColor (0.0,1.0,0.0);
    this->VerticalLineActor->GetProperty()->SetColor (0.0,0.0,1.0);
  }
  
  //  this->SetSizeData ( os.str().c_str() );
  this->Modified();  
}


void vtkViewImage2D::ScreenCoordinatesToPosition (double sc[2], double pos[3] )
{
  double p_sc[4];
  p_sc[0] = sc[0];
  p_sc[1] = sc[1];
  p_sc[2] = 0.0;
  p_sc[3] = 0.0;

  double p_pos[4];
  this->GetCurrentPoint (p_pos);
  p_pos[3] = 0.0;

  this->OrientationMatrix->MultiplyPoint (p_sc, p_sc);  
  this->ScreenToRealWorldMatrix->MultiplyPoint (p_pos, p_pos);

  for( int i=0; i<3; i++)
    pos[i] = p_sc[i] + p_pos[i];
}


void vtkViewImage2D::SetInterpolationMode(int i)
{
  this->ImageActor->SetInterpolate(i);
  this->Modified();
}


int vtkViewImage2D::GetInterpolationMode()
{
  return this->ImageActor->GetInterpolate();
}


void vtkViewImage2D::SetRulerWidgetVisibility (int val)
{
  this->RulerWidgetVisibility = val;
  if (this->GetRenderWindowInteractor()) 
  {
    if( this->RulerWidgetVisibility )
    {
      this->RulerWidget->On();
    }
    else
    {
      this->RulerWidget->Off();
    }
  }
}


void vtkViewImage2D::SetDistanceWidgetVisibility (int val)
{
  this->DistanceWidgetVisibility = val;
  if (this->GetRenderWindowInteractor())
  {
    if( this->DistanceWidgetVisibility )
    {
      this->DistanceWidget->On();
    }
    else
    {
      this->DistanceWidget->Off();
    }
  }
}


void vtkViewImage2D::SetMaskImage (vtkImageData* mask, vtkLookupTable* lut)
{

  if( !this->GetImage() || !mask || !lut)
  {
    return;
  }


  vtkViewImage::SetMaskImage (mask, lut);


  // check if the mask dimensions match the image dimensions
  int dim1[3], dim2[3];
  this->GetImage()->GetDimensions (dim1);
  mask->GetDimensions (dim2);
  if (    (dim1[0] != dim2[0]) ||
          (dim1[1] != dim2[1]) ||
          (dim1[2] != dim2[2]) )
  {
    vtkErrorMacro("Dimensions of the mask image do not match");
    return;
  }


  // check if the scalar range match the number of entries in the LUT
  double range[2];
  mask->GetScalarRange (range);
  int numLUT = lut->GetNumberOfTableValues();
  if( numLUT<(int)(range[1])+1 )
  {
    vtkErrorMacro("The number of LUT entries is less than the range of the mask.");
    return;
  }

  if( this->GetOverlappingImage() )
  {
    this->MaskFilter->SetImageInput ( this->Blender->GetOutput() );
  }
  else
  {
    this->MaskFilter->SetImageInput ( this->AuxInput );
  }
  this->MaskFilter->SetMaskInput (mask);
  this->MaskFilter->SetLookupTable (lut);
  this->MaskFilter->Update();


  this->ImageReslice->SetInputConnection( this->MaskFilter->GetOutputPort() );
  this->ResliceInput = this->MaskFilter->GetOutput();

}


void vtkViewImage2D::RemoveMaskImage()
{

  if( this->GetOverlappingImage() )
  {
    this->ImageReslice->SetInputConnection( this->Blender->GetOutputPort() );
    this->ResliceInput = this->Blender->GetOutput();
  }
  else
  {
    this->ImageReslice->SetInput( this->AuxInput );
    this->ResliceInput = this->AuxInput;
  }

  vtkViewImage::SetMaskImage (0,0);
}


void vtkViewImage2D::SetOverlappingImage (vtkImageData* image)
{
  vtkViewImage::SetOverlappingImage ( image );
  
  if( !this->GetImage() || !image )
  {
    return;
  }

  this->Blender->RemoveAllInputs();
  this->Blender->AddInput ( this->AuxInput );
  this->Blender->AddInput ( image );

  if( this->GetMaskImage() )
  {
    this->MaskFilter->SetInputConnection ( this->Blender->GetOutputPort() );
  }
  else
  {
    this->ImageReslice->SetInputConnection ( this->Blender->GetOutputPort() );
    this->ResliceInput = this->Blender->GetOutput();
  }
}


void vtkViewImage2D::RemoveOverlappingImage()
{

  if( this->GetMaskImage() )
  {
    this->MaskFilter->SetInput (this->AuxInput);
  }
  else
  {
    this->ImageReslice->SetInput (this->AuxInput);
    this->ResliceInput = this->AuxInput;
  }

  vtkViewImage::SetOverlappingImage ( 0 );
}


void vtkViewImage2D::SetOverlappingImageOpacity (double val)
{
  if (val<0.0)
    val = 0.0;
  if (val>1.0)
    val = 1.0;
  
  this->Blender->SetOpacity (0, 1.0-val);
  this->Blender->SetOpacity (1, val);
}


double vtkViewImage2D::GetOverlappingImageOpacity()
{
  return this->Blender->GetOpacity (1);
}


vtkActor* vtkViewImage2D::AddDataSet (vtkDataSet* dataset,  vtkProperty* property)
{

  bool doit = true;

  if (!dataset)
    doit = false;

  if( this->HasDataSet (dataset) )
  {
    doit = false;
  }

  
  vtkImageData* imagedata = vtkImageData::SafeDownCast(dataset);

  if (imagedata)
  {
    this->SetImage(imagedata);
  }
  else
  {
    if ( !this->GetImage() && !this->GetBGImage() )
    {
      doit = false;
    }
    // don't constrain the memory of input datasets anymore.
    /**

       if (dataset->GetActualMemorySize() > 20000)
       {
       vtkWarningMacro(<< "DataSet is to big for this projection type !\n");
       doit = false;
       }

    */

    if (doit)
    {

      vtkMatrix4x4* matrix = vtkMatrix4x4::New();
      switch(this->Orientation)
      {
	  case SAGITTAL_ID:
	    vtkMatrix4x4::Transpose (this->SagittalResliceMatrix, matrix);
	    break;

	  case CORONAL_ID:
	    vtkMatrix4x4::Transpose (this->CoronalResliceMatrix, matrix);
	    break;

	  case AXIAL_ID:
	  default:
	    vtkMatrix4x4::Transpose (this->AxialResliceMatrix, matrix);
	    break;
      }

      vtkCutter* cutter = vtkCutter::New();
      cutter->SetCutFunction ( this->DataSetCutPlane );

      // Very strangely in some cases (ex : landmarks)
      // the cutter increments the RefCount of the input dataset by 2
      // making some memory leek...
      // I could not manage to know what is wrong here
      cutter->SetInput (dataset);
      cutter->Update();

      if (!cutter->GetOutput())
      {
	vtkWarningMacro(<< "Unable to cut this dataset...");
	matrix->Delete();
 	cutter->Delete();
	return NULL;
      }

      vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
      mapper->SetInput (cutter->GetOutput());

      vtkActor* actor = vtkActor::New();
      actor->SetUserMatrix (matrix);
      actor->SetMapper (mapper);
      if (property)
      {
	actor->SetProperty (property);
      }
      
      //actor->PickableOff();

      this->AddActor (actor);

      this->DataSetList.push_back (dataset);
      this->DataSetActorList.push_back (actor);

      this->ResetAndRestablishZoomAndCamera();

      actor->Delete();
      mapper->Delete();
      matrix->Delete();
      cutter->Delete();
    }
  }

  return this->GetDataSetActor(dataset);

}


vtkActor* vtkViewImage2D::AddPolyData (vtkPolyData* polydata,  vtkProperty* property, double thickness)
{

  bool doit = true;
  if (!polydata || this->HasDataSet (polydata) || !this->GetImage())
  {
    doit = false;
  }


  if (doit)
  {

    if (thickness)
    {
      this->BoxThickness = thickness;
    }


    vtkClipDataSet* clipper = vtkClipDataSet::New();
    clipper->GenerateClippedOutputOff();
    clipper->InsideOutOn ();
    clipper->SetInput (polydata);

    /*
    unsigned int  direction = this->GetOrthogonalAxis (this->GetOrientation());
    switch(direction)
    {

	case X_ID :
	  this->DataSetCutBox->SetBounds (this->DataSetCutPlane->GetOrigin()[0]-0.5*this->BoxThickness,this->DataSetCutPlane->GetOrigin()[0]+0.5*this->BoxThickness,
					  this->GetWholeMinPosition(1),this->GetWholeMaxPosition(1),
					  this->GetWholeMinPosition(2),this->GetWholeMaxPosition(2));

	  break;
	case Y_ID :
	  this->DataSetCutBox->SetBounds (this->GetWholeMinPosition(0),this->GetWholeMaxPosition(0),
					  this->DataSetCutPlane->GetOrigin()[1]-0.5*this->BoxThickness,this->DataSetCutPlane->GetOrigin()[1]+0.5*this->BoxThickness,
					  this->GetWholeMinPosition(2),this->GetWholeMaxPosition(2));
	  break;
	case Z_ID :
	  this->DataSetCutBox->SetBounds (this->GetWholeMinPosition(0),this->GetWholeMaxPosition(0),
					  this->GetWholeMinPosition(1),this->GetWholeMaxPosition(1),
					  this->DataSetCutPlane->GetOrigin()[2]-0.5*this->BoxThickness,this->DataSetCutPlane->GetOrigin()[2]+0.5*this->BoxThickness);
	  break;
    }
    */ // -> already done in SetOrientation()

    clipper->SetClipFunction ( this->DataSetCutBox );
    clipper->Update();


    vtkMatrix4x4* matrix = vtkMatrix4x4::New();
    for (unsigned int i=0; i<3; i++)
    {
      for (unsigned int j=0; j<3; j++)
      {
	matrix->SetElement(i, j, this->ImageReslice->GetResliceAxes()->GetElement(j,i));
      }
    }
    matrix->SetElement(3,3,1);



    vtkDataSetMapper* mapper = vtkDataSetMapper::New();
    mapper->SetInput (clipper->GetOutput());

    vtkActor* actor = vtkActor::New();
    actor->SetMapper (mapper);
    actor->SetUserMatrix (matrix);
    if (property)
    {
      actor->SetProperty (property);
    }


    this->AddActor (actor);
    this->DataSetList.push_back (polydata);
    this->DataSetActorList.push_back (actor);

    this->ResetAndRestablishZoomAndCamera();

    actor->Delete();
    mapper->Delete();
    matrix->Delete();
    clipper->Delete();
  }

  return this->GetDataSetActor(polydata);

}


vtkActor* vtkViewImage2D::SyncAddPolyData (vtkPolyData* polydata,  vtkProperty* property, double thickness)
{

  if( this->IsLocked() )
  {
    return NULL;
  }

  vtkActor* actor = this->AddPolyData (polydata, property, thickness);

  this->Lock();
  for( unsigned int i=0; i<this->Children.size(); i++)
  {
    vtkViewImage2D* view = vtkViewImage2D::SafeDownCast (this->Children[i]);
    if( view )
    {
      view->SyncAddPolyData (polydata, property, thickness);
    }
  }
  this->UnLock();

  return actor;
}


void vtkViewImage2D::ResetZoom()
{
  vtkViewImage::ResetZoom();
  this->SetZoom ( 1.0 );
}


void vtkViewImage2D::SetShow2DAxis(int show)
{
  this->Show2DAxis = show;
  this->HorizontalLineActor->SetVisibility(show);
  this->VerticalLineActor->SetVisibility(show);
  this->Modified();
}


void vtkViewImage2D::SetZoom (double factor)
{

  if ( !this->GetRenderer() )
  {
    return;
  }

  vtkViewImage::SetZoom ( factor ); //this->GetZoom() ); //*factor );


  vtkCamera *camera = this->GetRenderer()->GetActiveCamera();


  camera->SetParallelScale(this->InitialParallelScale / this->GetZoom() );
  //camera->GetParallelScale() / factor );


  if ( this->GetRenderWindowInteractor() && this->GetRenderWindowInteractor()->GetLightFollowCamera() )
  {
    this->GetRenderer()->UpdateLightsGeometryToFollowCamera();
  }


  if( this->GetImage() )
  {
    int imCoor[3];
    this->GetCurrentVoxelCoordinates(imCoor);
    double pos[3];
    this->GetCurrentPoint (pos);
    int* dims = this->GetImage()->GetDimensions();
    std::ostringstream os;
    os << "Zoom: " << this->GetZoom()*100.0 << " %\n";
    int orthogonalAxis = this->GetOrthogonalAxis (this->Orientation);
    if ( this->ShowSliceNumber )
      os << "Slice: " << imCoor[orthogonalAxis] << " / " << dims[orthogonalAxis]-1 << std::endl;
    os << "Location: " << pos[orthogonalAxis] << " mm";

    this->SetDownLeftAnnotation( os.str().c_str() );
  }
  
}


void vtkViewImage2D::SetConventions (int val)
{
  if( val==vtkViewImage2D::RADIOLOGIC )
    this->SetConventionsToRadiological();
  else if( val==vtkViewImage2D::NEUROLOGIC )
    this->SetConventionsToNeurological();
  else
    vtkErrorMacro ( << "Conventions not supported.");
}


void vtkViewImage2D::SetConventionsToRadiological()
{
  this->Conventions = RADIOLOGIC;

  this->AxialResliceMatrix->SetElement (0, 0, 1.0);
  this->CoronalResliceMatrix->SetElement (0, 0, 1.0);

  unsigned int orientation = this->GetOrientation();
  this->Orientation = vtkViewImage::NB_PLAN_IDS;
  this->SetOrientation ( orientation );

  this->ResetAndRestablishZoomAndCamera();
  this->Modified();
}


void vtkViewImage2D::SetConventionsToNeurological()
{
  this->Conventions = NEUROLOGIC;

  this->AxialResliceMatrix->SetElement (0, 0, -1.0);
  this->CoronalResliceMatrix->SetElement (0, 0, -1.0);

  unsigned int orientation = this->GetOrientation();
  this->Orientation = vtkViewImage::NB_PLAN_IDS;
  this->SetOrientation ( orientation );
  
  this->ResetAndRestablishZoomAndCamera();
  this->Modified();
}


void vtkViewImage2D::SetDirectionMatrix (vtkMatrix4x4 *mat)
{
  vtkViewImage::SetDirectionMatrix (mat);

  // force to reset the reslice matrix
  this->Orientation = vtkViewImage::NB_PLAN_IDS;
  this->SetOrientation ( this->ViewOrientation );

  this->ResetAndRestablishZoomAndCamera();
  this->Modified();
}

void vtkViewImage2D::SyncSetCameraFocalAndPosition (double focal[3], double pos[3])
{
	this->SyncSetCameraFocalAndPosition( focal, pos, this->Orientation );
}

void vtkViewImage2D::SyncSetCameraFocalAndPosition (double focal[3], double pos[3], unsigned int orientation)
{
    if( this->IsLocked() )
	{
		return;
	}
	
	if( this->GetLinkCameraFocalAndPosition() && this->Orientation==orientation)
	{
		this->SetCameraFocalAndPosition (focal, pos);
	}
	
	this->Lock();
	for( unsigned int i=0; i<this->Children.size(); i++)
	{
		vtkSynchronizedView* view = vtkSynchronizedView::SafeDownCast (this->Children[i]);
		
		if( view )
		{
			if (vtkViewImage2D *view2d = vtkViewImage2D::SafeDownCast(view) ) 
			{
			  view2d->SyncSetCameraFocalAndPosition(focal, pos, orientation);
			}
			else 
			{
			  view->SyncSetCameraFocalAndPosition (focal, pos);
			}
			view->Update();
		}
	}
	this->UnLock();	
}

void vtkViewImage2D::ResetAndRestablishZoomAndCamera()
{

  if( !this->GetRenderer() )
  {
    return;
  }

  double zoom = this->GetZoom();
  vtkCamera *camera = this->GetRenderer()->GetActiveCamera();
  double c_position[3], focal[3];
  camera->GetPosition(c_position);
  camera->GetFocalPoint (focal);

  this->ResetZoom();
  double focal2[3], pos2[3];
  camera->GetFocalPoint (focal2);
  camera->GetPosition (pos2);

  camera->SetFocalPoint (focal[0], focal[1], focal2[2]);
  camera->SetPosition (c_position[0], c_position[1], pos2[2]);

  this->SetZoom ( zoom/this->GetZoom() );

}


void vtkViewImage2D::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Orientation = " << this->Orientation << "\n";
  double* bounds   = 0;
  double* spacing  = 0;
  double* origin   = 0;
  int*    extent   = 0;
  int*    wextent  = 0;
  int*    uextent  = 0;

  if( !this->GetImage() )
  {
    return;
  }


  this->GetImage()->Update();
  this->ImageReslice->GetOutput()->Update();
  this->WindowLevel->GetOutput()->Update();

  this->GetImage()->GetBounds();
  this->ImageReslice->GetOutput()->GetBounds();
  this->WindowLevel->GetOutput()->GetBounds();
  this->ImageActor->GetBounds();

  extent  = this->GetImage()->GetExtent();
  uextent = this->GetImage()->GetUpdateExtent();
  wextent = this->GetImage()->GetWholeExtent();
  origin  = this->GetImage()->GetOrigin();
  spacing = this->GetImage()->GetSpacing();
  os << indent << "InputImage   - "<<this->GetImage()->GetNumberOfPoints()<<" points.\n"
     << "  extent : ("
     << extent[0] << "/" << extent[1] << " ; "
     << extent[2] << "/" << extent[3] << " ; "
     << extent[4] << "/" << extent[5] << ") \n"
     << " uextent : ("
     << uextent[0] << "/" << uextent[1] << " ; "
     << uextent[2] << "/" << uextent[3] << " ; "
     << uextent[4] << "/" << uextent[5] << ") \n"
     << " wextent : ("
     << wextent[0] << "/" << wextent[1] << " ; "
     << wextent[2] << "/" << wextent[3] << " ; "
     << wextent[4] << "/" << wextent[5] << ") \n"
     << "  origin : ("
     << origin[0] << " , " << origin[1] << " , " << origin[2] << ")\n"
     << "  spacing : ("
     << spacing[0] << " , " << spacing[1] << " , " << spacing[2] << ")\n";

  extent  = this->ImageReslice->GetOutput()->GetExtent();
  uextent = this->ImageReslice->GetOutput()->GetUpdateExtent();
  wextent = this->ImageReslice->GetOutput()->GetWholeExtent();
  origin  = this->ImageReslice->GetOutput()->GetOrigin();
  spacing = this->ImageReslice->GetOutput()->GetSpacing();
  os << indent << "Resliced Image   - "<<this->ImageReslice->GetOutput()->GetNumberOfPoints()<<" points.\n"
     << "  extent : ("
     << extent[0] << "/" << extent[1] << " ; "
     << extent[2] << "/" << extent[3] << " ; "
     << extent[4] << "/" << extent[5] << ") \n"
     << " uextent : ("
     << uextent[0] << "/" << uextent[1] << " ; "
     << uextent[2] << "/" << uextent[3] << " ; "
     << uextent[4] << "/" << uextent[5] << ") \n"
     << " wextent : ("
     << wextent[0] << "/" << wextent[1] << " ; "
     << wextent[2] << "/" << wextent[3] << " ; "
     << wextent[4] << "/" << wextent[5] << ") \n"
     << "  origin : ("
     << origin[0] << " , " << origin[1] << " , " << origin[2] << ")\n"
     << "  spacing : ("
     << spacing[0] << " , " << spacing[1] << " , " << spacing[2] << ")\n";

  extent  = this->WindowLevel->GetOutput()->GetExtent();
  uextent = this->WindowLevel->GetOutput()->GetUpdateExtent();
  wextent = this->WindowLevel->GetOutput()->GetWholeExtent();
  origin  = this->WindowLevel->GetOutput()->GetOrigin();
  spacing = this->WindowLevel->GetOutput()->GetSpacing();
  os << indent << "WindowLevel Image   - "<<this->WindowLevel->GetOutput()->GetNumberOfPoints()<<" points.\n"
     << "  extent : ("
     << extent[0] << "/" << extent[1] << " ; "
     << extent[2] << "/" << extent[3] << " ; "
     << extent[4] << "/" << extent[5] << ") \n"
     << " uextent : ("
     << uextent[0] << "/" << uextent[1] << " ; "
     << uextent[2] << "/" << uextent[3] << " ; "
     << uextent[4] << "/" << uextent[5] << ") \n"
     << " wextent : ("
     << wextent[0] << "/" << wextent[1] << " ; "
     << wextent[2] << "/" << wextent[3] << " ; "
     << wextent[4] << "/" << wextent[5] << ") \n"
     << "  origin : ("
     << origin[0] << " , " << origin[1] << " , " << origin[2] << ")\n"
     << "  spacing : ("
     << spacing[0] << " , " << spacing[1] << " , " << spacing[2] << ")\n";

  bounds = this->GetImage()->GetBounds();
  os << indent << "InputImage \n"
     << "           bounds : ("
     << bounds[0] << "/" << bounds[1] << " ; "
     << bounds[2] << "/" << bounds[3] << " ; "
     << bounds[4] << "/" << bounds[5] << ") \n";
  bounds = this->ImageReslice->GetOutput()->GetBounds();
  os << indent << "Resliced Image \n"
     << "           bounds : ("
     << bounds[0] << "/" << bounds[1] << " ; "
     << bounds[2] << "/" << bounds[3] << " ; "
     << bounds[4] << "/" << bounds[5] << ") \n";
  bounds = this->WindowLevel->GetOutput()->GetBounds();
  os << indent << "WindowLevel Image \n"
     << "           bounds : ("
     << bounds[0] << "/" << bounds[1] << " ; "
     << bounds[2] << "/" << bounds[3] << " ; "
     << bounds[4] << "/" << bounds[5] << ") \n";
  bounds = this->ImageActor->GetBounds();
  os << indent << "Image Actor  bounds : ("
     << bounds[0] << "/" << bounds[1] << " ; "
     << bounds[2] << "/" << bounds[3] << " ; "
     << bounds[4] << "/" << bounds[5] << ") \n";

}


void vtkViewImage2D::SetBGImage (vtkImageData *image, int slice, vtkTransform* transform)
{
    if (!this->GetRenderer() || !image || slice < 0)
	return;

    int dim = image->GetDataDimension();
    vtkImageActor* actor = vtkImageActor::New();
    vtkImageMapToColors*  windowLevel = vtkImageMapToColors::New();
    int* w_ext = image->GetWholeExtent();
    vtkLookupTable* lut = vtkLookupTable::New();


//  vtkImageData * input = vtkImageData::New();
// case image->GetScalarTypr() == VTK_UNSIGNED_CHAR?

    vtkLookupTable* imgLut =  vtkLookupTable::SafeDownCast (this->WindowLevel->GetLookupTable());
    if(imgLut)
    {
	lut->DeepCopy(imgLut);
	windowLevel->SetLookupTable(lut);
    }

    double range[2];
    image->GetScalarRange (range);
    if ( windowLevel->GetLookupTable() )
    {
	windowLevel->GetLookupTable()->SetRange (range);
    }


    windowLevel->SetInput(image);
    actor->SetInput(windowLevel->GetOutput());

    if (dim==3)
    {
	int nbSl;
	switch (this->Orientation)
	{
	case vtkViewImage::AXIAL_ID:
	    nbSl = w_ext[5] - w_ext[4] + 1;
	    if (slice > nbSl)
	    {
		std::cout<<"Error: cannot add a background image, the slice number is too high "<<std::endl;
		return;
	    }
	    else
		actor->SetDisplayExtent( w_ext[0], w_ext[1], w_ext[2], w_ext[3], slice, slice);
	    break;


	case vtkViewImage::CORONAL_ID:
	    nbSl = w_ext[3] - w_ext[2] + 1;
	    if (slice > nbSl)
	    {
		std::cout<<"Error: can't add a background image, the slice number is too high "<<std::endl;
		return;
	    }
	    else
		actor->SetDisplayExtent( w_ext[0], w_ext[1], slice ,slice, w_ext[4], w_ext[5]);
	    break;


	case vtkViewImage::SAGITTAL_ID:
	    nbSl = w_ext[1] - w_ext[0] + 1;
	    if (slice > nbSl)
	    {
		std::cout<<"Error: can't add a background image, the slice number is too high "<<std::endl;
		return;
	    }
	    else
	    actor->SetDisplayExtent( slice, slice, w_ext[2], w_ext[3], w_ext[4], w_ext[5]);
	    break;
	}
    }
    else
    {
	slice = 0;
	actor->SetDisplayExtent( w_ext[0], w_ext[1], w_ext[2], w_ext[3], 0, 0);
    }

    if (transform)
    {
    actor->SetUserTransform(transform);
    }

    vtkTransform * transform2 = vtkTransform::New();
    transform2->RotateX(180);
    actor->SetUserTransform(transform2);

    actor->PickableOff();
    actor->DragableOff();

    if (this->BGActor && this->GetRenderer()->HasViewProp(this->BGActor) )
    {
	this->GetRenderer()->RemoveActor(BGActor);
    }

    this->BGActor = actor;
    this->BGWindowLevel = windowLevel;
    this->BGImage = image;

    if (this->ImageActor)
    {
	this->GetRenderer()->RemoveActor(ImageActor);
	this->GetRenderer()->AddViewProp(this->BGActor);
	this->GetRenderer()->AddViewProp(ImageActor);
    }
    else
	this->GetRenderer()->AddViewProp(this->BGActor);


    actor->Delete();
    windowLevel->Delete();
    lut->Delete();
    transform2->Delete();

}


void vtkViewImage2D::RemoveBGImage()
{

    if (this->BGActor && this->GetRenderer()->HasViewProp(this->BGActor))
    {
	this->GetRenderer()->RemoveActor(this->BGActor);
    }

    this->BGImage = 0;
    this->BGWindowLevel = 0;

}


void vtkViewImage2D::SetBGImageOpacity(double opacity)
{
    if (!this->BGActor)
	return;
    this->BGActor->SetOpacity(opacity);
}


double vtkViewImage2D::GetBGImageOpacity() const
{
    if (!this->BGActor)
	return -1.0;
    return this->BGActor->GetOpacity();
}


void vtkViewImage2D::SetOpacity(double opacity)
{
  this->ImageActor->SetOpacity(opacity);
}

double vtkViewImage2D::GetOpacity() const
{
  return this->ImageActor->GetOpacity();
}


void vtkViewImage2D::Clear()
{
  // maybe containing some bugs or missing process
  this->Reset();
  this->FirstRender = 1;
  this->SetFirstImage (1);
  
  this->RemoveBGImage();
  
  if (this->ImageActor && this->GetRenderer()->HasViewProp(this->ImageActor) )
  {
    this->ImageActor->SetPosition(0.0, 0.0, 0.0);
    this->ImageActor->SetScale(1.0);
    this->ImageActor->SetOrientation( this->ImageActor->GetOrientation()[0],
				      this->ImageActor->GetOrientation()[0],
				      0);

    this->ImageActor->SetUserMatrix(vtkMatrix4x4::New());
    this->GetRenderer()->RemoveActor(this->ImageActor);
  }
  this->RemoveActor( this->HorizontalLineActor );
  this->RemoveActor( this->VerticalLineActor );
  
  if ( this->GetImage() )
  {
    vtkViewImage::SetImage( 0 );
  }
  ///////////////////
  // useful?
  
  //this->RemoveAllDataSet();
  //   this->ImageReslice->RemoveAllInputs();
  //     this->WindowLevel->RemoveAllInputs();
  //     this->Blender->RemoveAllInputs();
  //     this->MaskFilter->RemoveAllInputs();
  // this->ImageActor       = vtkImageActor::New(); // remove inputs?
  // this->BGActor          = vtkImageActor::New();
  //  this->BGWindowLevel->RemoveAllInputs();
  
  // this->AuxInput     = this->WindowLevel->GetOutput();
  // this->ResliceInput = this->WindowLevel->GetOutput();
  // this->RemoveAllDataSet();
  // this->InternalUpdate(); 
}
