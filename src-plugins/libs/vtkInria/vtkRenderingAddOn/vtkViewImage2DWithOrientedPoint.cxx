/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkRenderingAddOn/vtkViewImage2DWithOrientedPoint.h>

#include "vtkImageData.h"
#include "vtkLookupTable.h"
#include <vtkRenderingAddOn/vtkImageOrientedPointWidgetCallback.h>
#include <vtkImageOrientedPointWidget.h>

#include "vtkImageActor.h"
#include "vtkGlyphSource2D.h"
#include "vtkMapper.h"
#include "vtkInteractorObserver.h"

#include "vtkPoints.h"
#include <sstream>
#include <assert.h>
#include <cmath>

#include <vtkObjectFactory.h>

vtkCxxRevisionMacro(vtkViewImage2DWithOrientedPoint, "$Revision: 1284 $");
vtkStandardNewMacro(vtkViewImage2DWithOrientedPoint);


vtkViewImage2DWithOrientedPoint::vtkViewImage2DWithOrientedPoint()
{

  this->OrientedPointWidget = vtkImageOrientedPointWidget::New();
  this->OrientedPointCbk    = vtkImageOrientedPointWidgetCallback::New();
  
  // Setting register widget properties
  this->OrientedPointWidget->GlobalWarningDisplayOff();
  this->OrientedPointWidget->SetDefaultRenderer ( this->GetRenderer() );
  this->OrientedPointWidget->SetCaptureRadius(3.5);
  this->OrientedPointWidget->ProjectToPlaneOn();  
  this->OrientedPointWidget->SetProjectionNormalToZAxes();
  this->OrientedPointWidget->SetProjectionPosition(0.1);
  this->OrientedPointWidget->AutoCloseOn();
  this->OrientedPointWidget->SetViewProp( this->GetImageActor() );
  
  this->OrientedPointWidget->SetPriority(10.0);

  this->OrientedPointCbk->SetView (this);  
  this->AddObserver (vtkCommand::KeyPressEvent, this->OrientedPointCbk);
  this->OrientedPointWidget->AddObserver (vtkCommand::EndInteractionEvent, this->OrientedPointCbk);
  this->OrientedPointWidget->AddObserver (vtkCommand::MouseMoveEvent,      this->OrientedPointCbk);
  //this->OrientedPointWidget->SetKeyPressActivationValue ('j'); // disable "i"

  this->RegistrationData.position[0]=0.0;
  this->RegistrationData.position[1]=0.0;
  this->RegistrationData.position[2]=0.0;

  this->RegistrationData.direction[0]=1.0;
  this->RegistrationData.direction[1]=0.0;
  this->RegistrationData.direction[2]=0.0;

  this->RegistrationData.orientation=this->GetOrientation();

}

vtkViewImage2DWithOrientedPoint::~vtkViewImage2DWithOrientedPoint()
{
  this->OrientedPointCbk->Delete();
  this->OrientedPointWidget->Delete();
}

void vtkViewImage2DWithOrientedPoint::PrepareForDelete()
{
  this->OrientedPointWidget->Off();
  this->RemoveObserver (this->OrientedPointCbk);
}

void vtkViewImage2DWithOrientedPoint::UpdateRegistrationData ()
{
  if( !this->GetImage() )
  {
    return;
  }
  
  this->RegistrationData.position[0]=this->OrientedPointCbk->GetPosition()[0];
  this->RegistrationData.position[1]=this->OrientedPointCbk->GetPosition()[1];
  this->RegistrationData.position[2]=this->OrientedPointCbk->GetPosition()[2];
  
  this->RegistrationData.direction[0]=this->OrientedPointCbk->GetDirection()[0];
  this->RegistrationData.direction[1]=this->OrientedPointCbk->GetDirection()[1];
  this->RegistrationData.direction[2]=this->OrientedPointCbk->GetDirection()[2];

  this->RegistrationData.orientation = this->GetOrientation();
  
}

void vtkViewImage2DWithOrientedPoint::SetImage (vtkImageData* image)
{
  vtkViewImage2D::SetImage (image);
  this->OrientedPointWidget->SetInteractor ( this->GetRenderWindowInteractor() );
  this->OrientedPointWidget->SetEnabled (0);
  this->OrientedPointWidget->Off();
  this->OrientedPointWidget->InteractionOff();
}

void
vtkViewImage2DWithOrientedPoint::ValidateRegistering()
{
}


void vtkViewImage2DWithOrientedPoint::SetManualRegisteringOn ()
{
  if (!this->GetImage())
  {
    return;
  }
  
  this->OrientedPointWidget->On();
}

void vtkViewImage2DWithOrientedPoint::SetManualRegisteringOff ()
{
  if (!this->GetImage())
  {
    return;
  }
    
  this->OrientedPointWidget->Off();
}

void vtkViewImage2DWithOrientedPoint::SetManualRegisteringMode (bool a)
{
  if(a)
  {
    this->SetManualRegisteringOn();
  }
  else
  { 
    this->SetManualRegisteringOff();
  }
  
}


 
