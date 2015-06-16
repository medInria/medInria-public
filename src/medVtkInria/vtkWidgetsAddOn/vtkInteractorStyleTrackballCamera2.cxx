/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkInteractorStyleTrackballCamera2.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkTransform.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkCallbackCommand.h"

vtkCxxRevisionMacro(vtkInteractorStyleTrackballCamera2, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkInteractorStyleTrackballCamera2);

vtkInteractorStyleTrackballCamera2::vtkInteractorStyleTrackballCamera2()
{  
  this->Transform = vtkTransform::New();

  (this->UpAxis)[0] = 0;
  (this->UpAxis)[1] = 0;
  (this->UpAxis)[2] = 1;
}

vtkInteractorStyleTrackballCamera2::~vtkInteractorStyleTrackballCamera2()
{
  this->Transform->Delete();
}

void vtkInteractorStyleTrackballCamera2::OnLeftButtonDown() 
{
  vtkInteractorStyleTrackballCamera::OnLeftButtonDown();

  if (!this->Interactor->GetShiftKey() && !this->Interactor->GetControlKey()) {

    vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
  
    double viewup[3]; camera->GetViewUp(viewup);
    double upaxis[3];
    upaxis[0] = 0;
    upaxis[1] = 0;
    upaxis[2] = 1;
      
    if(vtkMath::Dot(viewup, upaxis) < 0)
      (this->UpAxis)[2] = -1;
    else
      (this->UpAxis)[2] = 1;
  }
}

void vtkInteractorStyleTrackballCamera2::OnMouseWheelForward()
{
    vtkInteractorStyleTrackballCamera::OnMouseWheelForward();
    this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
}

void vtkInteractorStyleTrackballCamera2::OnMouseWheelBackward()
{
    vtkInteractorStyleTrackballCamera::OnMouseWheelBackward();
    this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
}

void vtkInteractorStyleTrackballCamera2::Rotate()
{
  if (this->CurrentRenderer == NULL)
    return;
    
  vtkRenderWindowInteractor *rwi = this->Interactor;

  int dx = rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0];
  int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];
  
  int *size = this->CurrentRenderer->GetRenderWindow()->GetSize();

  double delta_elevation = -20.0 / size[1];
  double delta_azimuth = -20.0 / size[0];
  
  double rxf = dx * delta_azimuth * this->MotionFactor;
  double ryf = dy * delta_elevation * this->MotionFactor;
  
  vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
  this->Azimuth(rxf);
  this->Elevation(ryf);
  camera->OrthogonalizeViewUp();

  if(this->AutoAdjustCameraClippingRange)
     this->CurrentRenderer->ResetCameraClippingRange();
  
  if(rwi->GetLightFollowCamera()) 
     this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();

  rwi->Render();
}

void vtkInteractorStyleTrackballCamera2::Azimuth(double angle)
{
  vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();

  double axis[3];
  double newPosition[3];
  double fp[3]; camera->GetFocalPoint(fp);
  double position[3]; camera->GetPosition(position);

  camera->GetViewUp(axis);

  this->Transform->Identity();
  this->Transform->Translate(+fp[0],+fp[1],+fp[2]);
  this->Transform->RotateWXYZ(angle, axis);
  this->Transform->Translate(-fp[0],-fp[1],-fp[2]);
  this->Transform->TransformPoint(position, newPosition);

  camera->SetPosition(newPosition);
  camera->SetViewUp(this->Transform->TransformVector(camera->GetViewUp()));
}

void vtkInteractorStyleTrackballCamera2::Elevation(double angle)
{
  vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();

  double axis[3], newPosition[3];
  double fp[3]; camera->GetFocalPoint(fp);
  double position[3]; camera->GetPosition(position);

  this->Transform->Identity();

  axis[0] = -camera->GetViewTransformMatrix()->GetElement(0,0);
  axis[1] = -camera->GetViewTransformMatrix()->GetElement(0,1);
  axis[2] = -camera->GetViewTransformMatrix()->GetElement(0,2);

  this->Transform->Translate(+fp[0],+fp[1],+fp[2]);
  this->Transform->RotateWXYZ(angle, axis);
  this->Transform->Translate(-fp[0],-fp[1],-fp[2]);
  this->Transform->TransformPoint(position, newPosition);

  camera->SetPosition(newPosition);
  camera->SetViewUp(this->Transform->TransformVector(camera->GetViewUp()));
}
