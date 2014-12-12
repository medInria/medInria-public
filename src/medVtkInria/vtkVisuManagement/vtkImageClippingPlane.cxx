/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkImageClippingPlane.h"

#include "vtkDataArray.h"
#include "vtkDataSet.h"
#include "vtkDataSetAttributes.h"
#include "vtkImageData.h"
#include "vtkImageStencil.h"
#include "vtkImageStencilData.h"
#include "vtkImageThreshold.h"
#include "vtkImplicitFunctionToImageStencil.h"
#include "vtkImplicitPlaneWidget.h"
#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkPlane.h"
#include "vtkPointData.h"
#include "vtkProperty.h"
#include "vtkSampleFunction.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"



// -----------------------------------------------------------------------------
// Constructor/Destructor
vtkImageClippingPlaneCallback::vtkImageClippingPlaneCallback()
{
  this->Image = NULL;
  this->BackupImage = NULL;
  this->ImplicitPlaneWidget = NULL;
  this->IRen1 = NULL;
  this->IRen2 = NULL;
  this->IRen3 = NULL;
  this->IRen4 = NULL;
  this->FirstRender = 0;

}

vtkImageClippingPlaneCallback::~vtkImageClippingPlaneCallback()
{
  std::cout << "Deleting vtkImageClippingPlaneCallback\n";
  if ( this->BackupImage )
    this->BackupImage->Delete();

  this->Image = NULL;
  this->BackupImage = NULL;
  this->ImplicitPlaneWidget = NULL;
  this->IRen1 = NULL;
  this->IRen2 = NULL;
  this->IRen3 = NULL;
  this->IRen4 = NULL;
}


// -----------------------------------------------------------------------------
// SetImage

void vtkImageClippingPlaneCallback::SetImage(vtkImageData* img)
{
  this->Image = img;

  if ( this->BackupImage )
    this->BackupImage->Delete();

  this->BackupImage = vtkImageData::New();
  this->BackupImage->DeepCopy(img);

  this->FirstRender = 1;
}


// -----------------------------------------------------------------------------
// Execute

void vtkImageClippingPlaneCallback::Execute(vtkObject* caller, unsigned long event, void* f)
{
  // An image must be defined
  if ( !this->Image || !this->BackupImage )
  {
    std::cerr << "Image not defined in vtkImageClippingPlaneCallback\n";
    return;
  }

  if ( !this->ImplicitPlaneWidget )
  {
    std::cerr << "ImplicitPlaneWidget not defined in vtkImageClippingPlaneCallback\n";
    return;
  }

  if ( !this->IRen1 || !this->IRen2 || !this->IRen3 || !this->IRen4 )
  {
    std::cerr << "Some renderers are not defined in vtkImageClippingPlaneCallback\n";
    return;
  }

  if (this->FirstRender)
  {
//     this->ImplicitPlaneWidget->PlaceWidget (this->Image->GetBounds());
    this->FirstRender = 0;
  }


  // Get the implicit plane
  vtkPlane* plane = vtkPlane::New();
  this->ImplicitPlaneWidget->GetPlane(plane);

  // Transform the implicit function into a stencil
  vtkImplicitFunctionToImageStencil* planeStencil = vtkImplicitFunctionToImageStencil::New();
#if VTK_MAJOR_VERSION>5 || (VTK_MAJOR_VERSION==5 && VTK_MINOR_VERSION>=2)
  planeStencil->SetInformationInput(this->BackupImage);
#else
  planeStencil->GetOutput()->SetSpacing ( this->BackupImage->GetSpacing() );
  planeStencil->GetOutput()->SetOrigin ( this->BackupImage->GetOrigin() );
  planeStencil->GetOutput()->SetWholeExtent ( this->BackupImage->GetWholeExtent() );
  planeStencil->GetOutput()->SetUpdateExtentToWholeExtent();
#endif
  planeStencil->SetInput(plane);
  planeStencil->SetThreshold(0.0);

  // Get min value of the image
  vtkDataArray* at = this->BackupImage->GetPointData()->GetScalars();
  double r[2];
  at->GetRange(r);

  // Override the stencil to the image
  vtkImageStencil* stencil = vtkImageStencil::New();
  stencil->SetInputData(this->BackupImage);
  stencil->SetStencilData(planeStencil->GetOutput());
  stencil->SetBackgroundValue(r[0]);
  stencil->ReverseStencilOn();
  stencil->Update();

  // Output the result
  this->Image->DeepCopy(stencil->GetOutput());

  // Update the renderers
  IRen1->GetRenderWindow()->Render();
  IRen2->GetRenderWindow()->Render();
  IRen3->GetRenderWindow()->Render();
  IRen4->GetRenderWindow()->Render();

  // Clean-up memory
  stencil->Delete();
  planeStencil->Delete();
  plane->Delete();
}


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------


vtkCxxRevisionMacro(vtkImageClippingPlane, "$Revision: 644 $");
vtkStandardNewMacro(vtkImageClippingPlane);


// -----------------------------------------------------------------------------
// Constructor/Destructor

vtkImageClippingPlane::vtkImageClippingPlane()
{
  this->PlaneWidget = NULL;
  this->IRen1       = NULL;
  this->IRen2       = NULL;
  this->IRen3       = NULL;
  this->IRen4       = NULL;
  this->Image       = NULL;
  this->Callback    = NULL;
}

vtkImageClippingPlane::~vtkImageClippingPlane()
{
  std::cout << "Deleting vtkImageClippingPlane\n";

  // 	this->PlaneWidget->RemoveAllObservers();
  // remove the observer that we actually added.
  this->PlaneWidget->RemoveObserver (this->Callback);
  this->PlaneWidget->Off();

  if ( this->Callback )
    this->Callback->Delete();

  if ( this->PlaneWidget )
    this->PlaneWidget->Delete();

  this->PlaneWidget = NULL;
  this->IRen1       = NULL;
  this->IRen2       = NULL;
  this->IRen3       = NULL;
  this->IRen4       = NULL;
  this->Image       = NULL;
  this->Callback    = NULL;
}


// -----------------------------------------------------------------------------
// CreateClippingPlane

void vtkImageClippingPlane::CreateClippingPlane()
{

  if ( !this->IRen1 || !this->IRen2 || !this->IRen3 || !this->IRen4 )
  {
    vtkErrorMacro("Please first set the render window interactor\n");
    return;
  }

  if ( !this->Image )
  {
    vtkErrorMacro("Please first set the image data to clip\n");
    return;
  }

  if ( !this->PlaneWidget )
  {
    this->PlaneWidget = vtkImplicitPlaneWidget::New();
    this->PlaneWidget->NormalToZAxisOn();
    this->PlaneWidget->OutlineTranslationOff();
    this->PlaneWidget->ScaleEnabledOff();
    this->PlaneWidget->GetOutlineProperty()->SetOpacity(0);
    this->PlaneWidget->SetInputData((vtkDataSet*) this->Image);
    this->PlaneWidget->SetInteractor(this->IRen4);
    this->PlaneWidget->SetPlaceFactor(1.25);
    this->PlaneWidget->PlaceWidget();
    this->PlaneWidget->GetPlaneProperty()->SetOpacity(0.3);
    this->PlaneWidget->On();
  }

  if ( !this->Callback )
  {
    this->Callback = vtkImageClippingPlaneCallback::New();
    this->Callback->SetImage(this->Image);
    this->Callback->SetImplicitPlaneWidget(this->PlaneWidget);
    this->Callback->SetRenderWindowInteractor1(this->IRen1);
    this->Callback->SetRenderWindowInteractor2(this->IRen2);
    this->Callback->SetRenderWindowInteractor3(this->IRen3);
    this->Callback->SetRenderWindowInteractor4(this->IRen4);
  }

  this->PlaneWidget->AddObserver(vtkCommand::InteractionEvent, this->Callback, 0.0);
  this->PlaneWidget->InvokeEvent(vtkCommand::InteractionEvent);
}



// -----------------------------------------------------------------------------
// GetMask

void vtkImageClippingPlane::GetMask(vtkImageData* image)
{
  if ( !this->PlaneWidget )
    return;

  // Get the implicit plane
  vtkPlane* plane = vtkPlane::New();
  this->PlaneWidget->GetPlane(plane);

  // Get image properties
  int d[3];
  this->Image->GetDimensions(d);

  double s[3];
  this->Image->GetSpacing(s);

  double o[3];
  this->Image->GetOrigin(o);

  // Sample it over the image domain
  vtkSampleFunction* sample = vtkSampleFunction::New();
  sample->SetImplicitFunction(plane);
  sample->SetModelBounds(o[0], o[0]+d[0]*s[0], o[1], o[1]+d[1]*s[1], o[2], o[2]+d[2]*s[2]);
  sample->SetSampleDimensions(d);
  sample->SetOutputScalarTypeToFloat();
  sample->ComputeNormalsOff();
  sample->CappingOff();

  // Threshold the positive values
  vtkImageThreshold* thresholdFilter = vtkImageThreshold::New();
  thresholdFilter->SetInputConnection(sample->GetOutputPort());
  thresholdFilter->ThresholdByLower(0.0);
  thresholdFilter->ReplaceInOn();
  thresholdFilter->SetInValue(1.0);
  thresholdFilter->ReplaceOutOn();
  thresholdFilter->SetOutValue(0.0);
  thresholdFilter->SetOutputScalarTypeToUnsignedChar();
  thresholdFilter->Update();

  image->DeepCopy(thresholdFilter->GetOutput());

  thresholdFilter->Delete();
  sample->Delete();
}
