/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkDistanceWidget2.h"

#include "vtkCaptionActor2D.h"

#include "vtkActor.h"
#include "vtkAssemblyNode.h"
#include "vtkAssemblyPath.h"
#include "vtkCallbackCommand.h"
#include "vtkCamera.h"
#include "vtkCellPicker.h"
#include "vtkCommand.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPlanes.h"
#include "vtkPointWidget.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSphereSource.h"
#include "vtkMath.h"
#include "vtkTextProperty.h"
#include "vtkTextActor.h"

#include <sstream>
#include <cmath>


vtkStandardNewMacro(vtkDistanceWidget2);

//----------------------------------------------------------------------------
vtkDistanceWidget2::vtkDistanceWidget2()
{
  this->Caption = vtkCaptionActor2D::New();

  this->SetAlign (false);
  this->LineProperty->SetColor (0.85,0.85,1);
  this->HandleProperty->SetColor (0.75,0.75,1);
  this->KeyPressActivationOff();
  
//   this->ClampToBoundsOn();
  
  this->Caption->SetAttachmentPoint (0,0,0);
  this->Caption->SetCaption (".");
  this->Caption->GetCaptionTextProperty()->SetColor (0.7, 0.7, 0);
  this->Caption->GetCaptionTextProperty()->ShadowOff();
  this->Caption->GetCaptionTextProperty()->BoldOn();
  this->Caption->GetCaptionTextProperty()->ItalicOn();
  this->Caption->GetTextActor()->SetTextScaleModeToNone ();  
  this->Caption->GetCaptionTextProperty()->SetFontSize (15);
  this->Caption->SetLeaderGlyphSize(0.05);
  this->Caption->BorderOff();
  this->Caption->SetPadding (5);
}

//----------------------------------------------------------------------------
vtkDistanceWidget2::~vtkDistanceWidget2()
{
  this->Caption->Delete();
}


//----------------------------------------------------------------------------
void vtkDistanceWidget2::PlaceWidget(double bds[6])
{
  this->Superclass::PlaceWidget (bds);

  this->Caption->SetAttachmentPoint (this->GetPoint2());
}


//----------------------------------------------------------------------------
void vtkDistanceWidget2::SetEnabled(int enabling)
{
  if (this->CurrentRenderer && !enabling)
    this->CurrentRenderer->RemoveViewProp (this->Caption);

  this->Superclass::SetEnabled (enabling);
  
  if (this->CurrentRenderer && enabling)
    this->CurrentRenderer->AddViewProp (this->Caption);
}

    
//----------------------------------------------------------------------------
void vtkDistanceWidget2::OnMouseMove()
{
  // See whether we're active
  if ( this->State == vtkLineWidget::Outside ||
       this->State == vtkLineWidget::Start )
    {
    return;
    }

  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];

  // Do different things depending on state
  // Calculations everybody does
  double focalPoint[4], pickPoint[4], prevPickPoint[4];
  double z;

  vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
  if ( ! camera )
    {
    return;
    }

  // Compute the two points defining the motion vector
  this->ComputeWorldToDisplay(this->LastPickPosition[0],
                              this->LastPickPosition[1],
                              this->LastPickPosition[2], focalPoint);
  z = focalPoint[2];
  this->ComputeDisplayToWorld(
    double(this->Interactor->GetLastEventPosition()[0]),
    double(this->Interactor->GetLastEventPosition()[1]),
    z, prevPickPoint);
  this->ComputeDisplayToWorld(double(X), double(Y), z, pickPoint);

  // Process the motion
  int forward=0;
  if ( this->State == vtkLineWidget::MovingHandle )
    {
    forward = this->ForwardEvent(vtkCommand::MouseMoveEvent);
    }
  else if ( this->State == vtkLineWidget::MovingLine )
    {
    forward = this->ForwardEvent(vtkCommand::MouseMoveEvent);
    }
  else if ( this->State == vtkLineWidget::Scaling )
    {
    this->Scale(prevPickPoint, pickPoint, X, Y);
    }

  // Interact, if desired
  this->EventCallbackCommand->SetAbortFlag(1);
  this->InvokeEvent(vtkCommand::InteractionEvent,nullptr);

  double pt1[3], pt2[3], pt[3];
  this->GetPoint1(pt1);
  this->GetPoint2(pt2);
  pt[0] = pt1[0] - pt2[0];
  pt[1] = pt1[1] - pt2[1];
  pt[2] = pt1[2] - pt2[2];
  
  double distance = std::sqrt (pt[0]*pt[0] +pt[1]*pt[1] + pt[2]*pt[2]);
  
  char buffer[32];
  snprintf(buffer, 32,
	  "distance: %g mm",
	  distance);
  this->Caption->SetCaption (buffer);
  this->Caption->SetAttachmentPoint (this->GetPoint2());
  
  if ( ! forward )
  {
    this->Interactor->Render();
  }
}

