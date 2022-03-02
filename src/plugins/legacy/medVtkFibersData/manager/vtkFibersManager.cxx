/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkFibersManager.h"

#include <vtkObjectFactory.h>

#include <vtkCommand.h>
#include <vtkBoxWidget.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkLimitFibersToVOI.h>
#include <vtkLimitFibersToROI.h>
#include <vtkTubeFilter.h>
#include <vtkRibbonFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRendererCollection.h>
#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkScalarsToColors.h>
#include <vtkMaskPolyData.h>
#include <vtkProperty.h>
#include <vtkCornerAnnotation.h>
#include <vtkCleanPolyData.h>
#include <vtkProperty.h>
#include <vtkCellArray.h>
#include <vtkRenderWindow.h>
#include <vtkCellPicker.h>
#include <vtkPolyLine.h>
#include <vtkTextProperty.h>
#include <vtkCornerAnnotation.h>

#include <sstream>
#include <assert.h>
#include <cmath>

#include <stdio.h>

#include "vtkFiberKeyboardCallback.h"
#include "vtkFibersManagerCallback.h"
#include "vtkFiberPickerCallback.h"

vtkStandardNewMacro(vtkFibersManager);


int vtkFibersManager::vtkFiberRenderingStyle = 0;


vtkFibersManager::vtkFibersManager()
{
  this->Input                  = nullptr;
  this->RenderWindowInteractor = nullptr;
  this->Renderer               = nullptr;

  this->BoxWidget    = vtkBoxWidget::New();  
  this->Squeezer     = vtkMaskPolyData::New();
  this->Cleaner      = vtkCleanPolyData::New();
  this->TubeFilter   = vtkTubeFilter::New();
  this->RibbonFilter = vtkRibbonFilter::New();
  this->Actor        = vtkActor::New();
  this->MaximumNumberOfFibers = 20000;
  
  this->Callback         = vtkFibersManagerCallback::New();
  this->PickerCallback   = vtkFiberPickerCallback::New();
  this->KeyboardCallback = vtkFiberKeyboardCallback::New();
  
  this->KeyboardCallback->SetFiberPickerCallback ( this->PickerCallback );
  this->KeyboardCallback->SetFiberManager(this);
  
  this->BoxWidget->SetKeyPressActivationValue ('0');
  this->BoxWidget->RotationEnabledOff();
  this->BoxWidget->SetPlaceFactor (1.0);
  this->BoxWidget->AddObserver (vtkCommand::InteractionEvent, this->Callback);
  this->BoxWidgetVisibility = false;

  // create the pipeline
  this->Squeezer->ReleaseDataFlagOn();
  this->Cleaner->SetInputConnection ( this->Squeezer->GetOutputPort() );
  this->Cleaner->ReleaseDataFlagOn();
  
  this->TubeFilter->SetInputConnection( this->Cleaner->GetOutputPort() );
  this->TubeFilter->SetRadius (0.15);
  this->TubeFilter->SetNumberOfSides (4);
  this->TubeFilter->CappingOn();
  this->TubeFilter->ReleaseDataFlagOn();

  this->RibbonFilter->SetInputConnection( this->Cleaner->GetOutputPort() );
  this->RibbonFilter->SetWidth (0.15);
  this->RibbonFilter->ReleaseDataFlagOn();

  this->PickerCallback->SetInputConnection ( this->Callback->GetOutputPort() );
  this->PickerCallback->SetFibersManager (this);
  


    vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
    mapper->UseLookupTableScalarRangeOn();
    this->Mapper = mapper;


  this->Mapper->SetInputConnection ( this->Callback->GetOutputPort() );
  this->Mapper->SetScalarModeToUsePointData();

  this->HelpMessage = vtkCornerAnnotation::New();
  this->HelpMessage->SetNonlinearFontScaleFactor (0.25);

  vtkTextProperty* textProperty = vtkTextProperty::New();
  textProperty->SetColor (0.0,0.0,0.0);
  this->HelpMessage->SetTextProperty ( textProperty );
  textProperty->Delete();

  char helpMessage[]="h: Show/Hide this message\n"
    "b: Show/hide magic box\n"
    "4: Use lines\n"
    "5: Use ribbons\n"
    "6: Use tubes\n"
    "n: Navigate into visible\n"
    "u: Reset to initial state\n"
    "P: Pick fiber\n"
    "d: Delete picked fiber\n";
  
  this->HelpMessage->SetText (2, helpMessage );

  this->SetRenderingMode( vtkFibersManager::GetRenderingMode() );
}

vtkFibersManager::~vtkFibersManager()
{
  this->Disable();
  
  if( this->RenderWindowInteractor )
  {
    this->RenderWindowInteractor->Delete();
  }
    
  if (this->Renderer)
    this->Renderer->Delete();
  
  this->BoxWidget->RemoveObserver (this->Callback);
  
  this->BoxWidget->Delete();
  this->Callback->Delete();
  this->PickerCallback->Delete();
  this->KeyboardCallback->Delete();
  this->Squeezer->Delete();
  this->Cleaner->Delete();
  this->TubeFilter->Delete();
  this->RibbonFilter->Delete();
  this->HelpMessage->Delete();

  this->Actor->SetMapper(nullptr);
  
  this->Mapper->Delete();
  this->Actor->Delete();
}

void vtkFibersManager::Enable()
{
  if (this->RenderWindowInteractor)
  {
    vtkCellPicker* picker = vtkCellPicker::New();
    picker->AddObserver (vtkCommand::EndPickEvent, this->PickerCallback, 0.0 );
    picker->SetTolerance (0.001);
    
    this->RenderWindowInteractor->SetPicker ( picker );
    this->RenderWindowInteractor->AddObserver (vtkCommand::CharEvent, this->KeyboardCallback, 0.0 );
    
    picker->Delete();

    this->BoxWidget->SetInteractor (this->RenderWindowInteractor);

    if (this->BoxWidgetVisibility)
      this->BoxWidget->On();
  }

  if (this->Renderer)
  {
    this->Renderer->AddActor ( this->Actor );
    this->Renderer->AddActor ( this->PickerCallback->GetPickedActor() );
    this->Renderer->AddActor ( this->HelpMessage );
  }
}

void vtkFibersManager::Disable()
{
  if (this->Renderer)
  {
    this->Renderer->RemoveActor ( this->Actor );
    this->Renderer->RemoveActor ( this->PickerCallback->GetPickedActor() );
    this->Renderer->RemoveActor ( this->HelpMessage );
  }
  
  if( this->RenderWindowInteractor )
  { 
    this->RenderWindowInteractor->RemoveObserver(this->KeyboardCallback);
    this->RenderWindowInteractor->SetPicker (0);
  }
  this->BoxWidget->SetInteractor (0);
}

void vtkFibersManager::SetRenderWindowInteractor (vtkRenderWindowInteractor* rwin)
{
  if( rwin != this->RenderWindowInteractor )
  {
    if( this->RenderWindowInteractor != nullptr )
    {
      this->Disable();
      this->RenderWindowInteractor->UnRegister (this);
    }
    this->RenderWindowInteractor = rwin;
	  
    if( this->RenderWindowInteractor )
    {
      this->RenderWindowInteractor->Register(this);
    }
  }

  this->Enable();
}

void vtkFibersManager::RemoveAllActors()
{ 
  if (this->Renderer)
  {
    this->Renderer->RemoveActor ( this->Actor );
    this->Renderer->RemoveActor ( this->PickerCallback->GetPickedActor() );
    this->Renderer->RemoveActor ( this->HelpMessage );
  }
}

void vtkFibersManager::SetInput(vtkPolyData* input)
{
  if( !input )
  {
    return;
  }

  if (!this->RenderWindowInteractor)
  {
    vtkErrorMacro ( << "Interactor must be set prior to setting input");
    return;
  }

  this->Initialize();
  this->Input = input;
  
  this->Callback->GetROIFiberLimiter()->SetInputData ( this->Input );
  this->Actor->SetMapper (this->Mapper);
  
  if( this->Renderer )
  {
    this->BoxWidget->SetDefaultRenderer(this->Renderer);
  }
  this->BoxWidget->SetInputData ( this->GetInput() );
  this->BoxWidget->PlaceWidget();
  
  this->PickerCallback->SetFiberImage ( this->GetInput() );
  
  this->Callback->Execute (this->BoxWidget, 0, nullptr);
  
  int totalLines = this->Callback->GetOutput()->GetNumberOfLines();
  double ratio = (double)totalLines/(double)(this->MaximumNumberOfFibers);

  ratio = ratio<1.0?1.0:ratio;
  
  this->Squeezer->SetOnRatio ( (int)ratio );
  this->TubeFilter->SetInputConnection( this->Callback->GetFiberLimiter()->GetOutputPort() );
  this->RibbonFilter->SetInputConnection( this->Callback->GetFiberLimiter()->GetOutputPort() );
}

void vtkFibersManager::SwapInputOutput()
{
  vtkPolyData* auxPoly = vtkPolyData::New();
  auxPoly->SetPoints( this->Callback->GetOutput()->GetPoints() );
  auxPoly->GetPointData()->SetScalars( this->Callback->GetOutput()->GetPointData()->GetScalars() );

  vtkCellArray* auxLines = vtkCellArray::New();
  auxLines->DeepCopy( this->Callback->GetOutput()->GetLines() );
  auxPoly->SetLines( auxLines);

  this->BoxWidget->SetInputData ( auxPoly );
  this->Callback->GetFiberLimiter()->SetInputData ( auxPoly );

  this->PickerCallback->SetFiberImage ( auxPoly );
  
  auxPoly->Delete();
  auxLines->Delete();
}

void vtkFibersManager::Reset()
{
  if( !this->GetInput() )
    return;
  
  this->BoxWidget->SetInputData ( this->GetInput() );
  this->Callback->GetROIFiberLimiter()->SetInputData ( this->GetInput() );
  this->PickerCallback->SetFiberImage ( this->GetInput() );
  
  this->Callback->GetFiberLimiter()->SetInputConnection ( this->Callback->GetROIFiberLimiter()->GetOutputPort() );
  
  this->Callback->Execute (this->BoxWidget, 0, nullptr);
}

void vtkFibersManager::SetVisibility (bool isVisible)
{
  this->Actor->SetVisibility (isVisible);
  if (isVisible)
  {
    this->SetBoxWidget (this->BoxWidgetVisibility);
  }
}

void vtkFibersManager::Initialize()
{    
  this->Callback->GetFiberLimiter()->GetOutput()->Initialize();
  this->Callback->GetROIFiberLimiter()->GetOutput()->Initialize();
  this->Callback->GetROIFiberLimiter()->RemoveAllInputs();
  
  this->TubeFilter->GetOutput()->Initialize();

  this->RibbonFilter->GetOutput()->Initialize();
  this->Squeezer->GetOutput()->Initialize(); 
  
  this->Input=0;
  this->PickerCallback->SetFiberImage ( 0 );
}

void vtkFibersManager::SetRenderingModeToTubes()
{
  vtkFiberRenderingStyle = RENDER_IS_TUBES;
    this->Mapper->SetInputConnection (this->TubeFilter->GetOutputPort());
}

void vtkFibersManager::SetRenderingModeToRibbons()
{
  vtkFiberRenderingStyle = RENDER_IS_RIBBONS;
    this->Mapper->SetInputConnection (this->RibbonFilter->GetOutputPort());
}

void vtkFibersManager::SetRenderingModeToPolyLines()
{
  vtkFiberRenderingStyle = RENDER_IS_POLYLINES;
    this->PickerCallback->SetInputConnection (this->Callback->GetOutputPort());
    this->Mapper->SetInputConnection (this->Callback->GetOutputPort());
}

void vtkFibersManager::SetRenderingMode(int mode)
{
  switch(mode)
  {
      case RENDER_IS_POLYLINES:
	this->SetRenderingModeToPolyLines();
	break;
	
      case RENDER_IS_TUBES:
	this->SetRenderingModeToTubes();
	break;
	
      case RENDER_IS_RIBBONS:
	this->SetRenderingModeToRibbons();
	break;
	
      default:
	vtkErrorMacro(<<"Error: Rendering mode for fibers not supported.");
	break;	
  }	
}

int vtkFibersManager::GetRenderingMode()
{ 
  return vtkFiberRenderingStyle; 
}

vtkActor *vtkFibersManager::GetOutput() const
{
  return this->Actor;
}

vtkLimitFibersToROI* vtkFibersManager::GetROILimiter() const
{
  return this->Callback->GetROIFiberLimiter();
}

vtkLimitFibersToVOI* vtkFibersManager::GetVOILimiter() const
{
  return this->Callback->GetFiberLimiter();
}


void vtkFibersManager::BoxWidgetOn()
{
  this->BoxWidgetVisibility = true;
  this->BoxWidget->On();
}


void vtkFibersManager::BoxWidgetOff()
{
  this->BoxWidgetVisibility = false;
  this->BoxWidget->Off();
}


void vtkFibersManager::SetBoxWidget (bool a)
{
  this->BoxWidgetVisibility = a;
  if(a)
    this->BoxWidget->On();
  else
    this->BoxWidget->Off();
}

void vtkFibersManager::SetRadius (double r)
{
  this->TubeFilter->SetRadius (r);
  this->RibbonFilter->SetWidth (r);
}

double vtkFibersManager::GetRadius() const
{
  return this->TubeFilter->GetRadius ();
}

vtkCellArray* vtkFibersManager::GetSelectedCells() const
{
  return this->Callback->GetFiberLimiter()->GetOutput()->GetLines();
}

vtkPolyData *vtkFibersManager::GetCallbackOutput() const
{
  return this->Callback->GetOutput();
}

void vtkFibersManager::SetMaskImage (vtkImageData* image)
{
  this->Callback->GetROIFiberLimiter()->SetMaskImage (image);
  this->Callback->GetROIFiberLimiter()->Modified();
}

void vtkFibersManager::SetColorModeToLocalFiberOrientation()
{
  this->Mapper->SetScalarModeToUsePointData();
}

void vtkFibersManager::SetColorModelToGlobalFiberOrientation()
{
  this->Mapper->SetScalarModeToUseCellData();
}

void vtkFibersManager::SetColorModeToPointArray (const int& id)
{
  if( id >= this->Mapper->GetInput()->GetPointData()->GetNumberOfArrays() )
  {
    return;
  }
  
  this->Mapper->SetScalarModeToUsePointFieldData();
  this->Mapper->SelectColorArray (id);
}

const char* vtkFibersManager::GetPointArrayName (const int& id) const
{
  if( id >= this->Mapper->GetInput()->GetPointData()->GetNumberOfArrays() )
  {
    return 0;
  }
  
  return this->Mapper->GetInput()->GetPointData()->GetArrayName (id);
}

int vtkFibersManager::GetNumberOfPointArrays() const
{
  return this->Mapper->GetInput()->GetPointData()->GetNumberOfArrays();
}

void vtkFibersManager::SetLookupTable(vtkScalarsToColors* lut)
{
  this->Mapper->SetLookupTable (lut);
}

vtkScalarsToColors* vtkFibersManager::GetLookupTable() const
{
  return this->Mapper->GetLookupTable();
}

void vtkFibersManager::SetMaximumNumberOfFibers(int num)
{
  this->MaximumNumberOfFibers = num;
}

int vtkFibersManager::GetMaximumNumberOfFibers() const
{
  return this->MaximumNumberOfFibers;
}

void vtkFibersManager::ShowHelpMessage()
{
  this->HelpMessage->SetVisibility (1);
}

void vtkFibersManager::HideHelpMessage()
{
  this->HelpMessage->SetVisibility (0);
}

void vtkFibersManager::SetHelpMessageVisibility (int a)
{
  if( a )
  {
    this->ShowHelpMessage();
  }
  else
  {
    this->HideHelpMessage();
  }
}

int vtkFibersManager::GetHelpMessageVisibility() const
{
  return this->HelpMessage->GetVisibility();
}

void vtkFibersManager::Execute()
{
  this->Callback->GetROIFiberLimiter()->Modified();
  this->Callback->GetFiberLimiter()->Modified();
  this->Callback->GetFiberLimiter()->Update();
}
