/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkOrientedBoxWidget.h>
#include "vtkMatrixToLinearTransform.h"
#include "vtkObjectFactory.h"
#include "vtkMatrix4x4.h"
#include "vtkPoints.h"
#include "vtkActor.h"
#include "vtkAssemblyNode.h"
#include "vtkAssemblyPath.h"
#include "vtkCallbackCommand.h"
#include "vtkCamera.h"
#include "vtkCellArray.h"
#include "vtkCellPicker.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPlanes.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSphereSource.h"
#include "vtkTransform.h"
#include "vtkReverseSense.h"

vtkCxxRevisionMacro(vtkOrientedBoxWidget, "$Revision: 2 $");
vtkStandardNewMacro(vtkOrientedBoxWidget);


//----------------------------------------------------------------------------
vtkOrientedBoxWidget::vtkOrientedBoxWidget()
{
  this->OrientationMatrix = vtkMatrix4x4::New();
  this->OrientationMatrix->Identity();
  
  this->InvertedOrientationMatrix = vtkMatrix4x4::New();
  this->InvertedOrientationMatrix->Identity();
}

//----------------------------------------------------------------------------
vtkOrientedBoxWidget::~vtkOrientedBoxWidget()
{
  this->OrientationMatrix->Delete();
  this->InvertedOrientationMatrix->Delete();
}


//----------------------------------------------------------------------------
void vtkOrientedBoxWidget::SetOrientationMatrix(vtkMatrix4x4* matrix)
{
  if (matrix == this->OrientationMatrix)
    return;

  if (this->OrientationMatrix)
  {
    this->OrientationMatrix->UnRegister (this);
    this->OrientationMatrix = NULL;
  }

  this->OrientationMatrix = matrix;
  
  if ( this->OrientationMatrix )
  {
    this->OrientationMatrix->Register(this);
  }


  // move all the actors according to the user-matrix
  this->HexActor->SetUserMatrix (matrix);
  this->HexFace->SetUserMatrix (matrix);
  this->HexOutline->SetUserMatrix (matrix);
  
  for (unsigned int i=0; i<7; i++)
    this->Handle[i]->SetUserMatrix (matrix);

  if (matrix->Determinant()<0.0)
  {
    for (unsigned int i=0; i<7; i++)
    {
      if (vtkPolyData *input = vtkPolyData::SafeDownCast (this->Handle[i]->GetMapper()->GetInput()))
      {
	vtkReverseSense *reverse = vtkReverseSense::New();
	reverse->SetInput (input);
	reverse->ReverseNormalsOn();
	reverse->ReverseCellsOff();
	reverse->Update();
	vtkPolyDataMapper::SafeDownCast (this->Handle[i]->GetMapper())->SetInput (reverse->GetOutput());
	reverse->Delete();
      }
    }
  }
  
  if (this->OrientationMatrix)
  { 
    // initialize inverse matrix
    vtkMatrix4x4::Invert (this->OrientationMatrix, this->InvertedOrientationMatrix);
  }
  
}



//----------------------------------------------------------------------------
void vtkOrientedBoxWidget::OnMouseMove()
{
  // See whether we're active
  if ( this->State == vtkBoxWidget::Outside || 
       this->State == vtkBoxWidget::Start )
    {
    return;
    }
  
  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];

  // Do different things depending on state
  // Calculations everybody does
  double focalPoint[4], pickPoint[4], prevPickPoint[4];
  double z, vpn[3];
  
  vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
  if ( !camera )
    {
    return;
    }

  // Compute the two points defining the motion vector
  this->ComputeWorldToDisplay(this->LastPickPosition[0], this->LastPickPosition[1],
                              this->LastPickPosition[2], focalPoint);
  z = focalPoint[2];
  this->ComputeDisplayToWorld(double(this->Interactor->GetLastEventPosition()[0]),
                              double(this->Interactor->GetLastEventPosition()[1]),
                              z, prevPickPoint);
  this->ComputeDisplayToWorld(double(X), double(Y), z, pickPoint);

  // multiply pick points by the inverted orientation matrix
  // to recover the true movement to apply
  this->InvertedOrientationMatrix->MultiplyPoint (prevPickPoint, prevPickPoint);
  this->InvertedOrientationMatrix->MultiplyPoint (pickPoint, pickPoint);
  
  // Process the motion
  if ( this->State == vtkBoxWidget::Moving )
    {
    // Okay to process
    if ( this->CurrentHandle )
      {
      if ( this->RotationEnabled && this->CurrentHandle == this->HexFace )
        {
        camera->GetViewPlaneNormal(vpn);
        this->Rotate(X, Y, prevPickPoint, pickPoint, vpn);
        }
      else if ( this->TranslationEnabled && this->CurrentHandle == this->Handle[6] )
        {
        this->Translate(prevPickPoint, pickPoint);
        }
      else if ( this->TranslationEnabled && this->ScalingEnabled ) 
        {
        if ( this->CurrentHandle == this->Handle[0] )
          {
          this->MoveMinusXFace(prevPickPoint, pickPoint);
          }
        else if ( this->CurrentHandle == this->Handle[1] )
          {
          this->MovePlusXFace(prevPickPoint, pickPoint);
          }
        else if ( this->CurrentHandle == this->Handle[2] )
          {
          this->MoveMinusYFace(prevPickPoint, pickPoint);
          }
        else if ( this->CurrentHandle == this->Handle[3] )
          {
          this->MovePlusYFace(prevPickPoint, pickPoint);
          }
        else if ( this->CurrentHandle == this->Handle[4] )
          {
          this->MoveMinusZFace(prevPickPoint, pickPoint);
          }
        else if ( this->CurrentHandle == this->Handle[5] )
          {
          this->MovePlusZFace(prevPickPoint, pickPoint);
          }
        }
      }
    }
  else if ( this->ScalingEnabled && this->State == vtkBoxWidget::Scaling )
    {
    this->Scale(prevPickPoint, pickPoint, X, Y);
    }

  // Interact, if desired
  this->EventCallbackCommand->SetAbortFlag(1);
  this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
  this->Interactor->Render();
}
