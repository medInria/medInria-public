/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
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

#ifdef vtkINRIA3D_USE_HWSHADING
#include "vtkFiberMapper.h"
//using namespace bmia;
#endif

vtkCxxRevisionMacro(vtkFibersManager, "$Revision: 1467 $");
vtkStandardNewMacro(vtkFibersManager);


int vtkFibersManager::vtkUseHardwareShaders  = 0;
int vtkFibersManager::vtkFiberRenderingStyle = 0;


/**
   vtkFibersManagerCallback decalaration & implementation
 */
class VTK_VISUMANAGEMENT_EXPORT vtkFibersManagerCallback: public vtkCommand
{

 public:
  static vtkFibersManagerCallback* New()
  { return new vtkFibersManagerCallback; }

  virtual void Execute ( vtkObject *caller, unsigned long, void* );

  vtkPolyData* GetOutput() const
  { return this->FiberLimiter->GetOutput(); }

  vtkLimitFibersToVOI* GetFiberLimiter() const
  { return this->FiberLimiter; }
  
  vtkLimitFibersToROI* GetROIFiberLimiter() const
  { return this->ROIFiberLimiter; }
  
    
 protected:
  vtkFibersManagerCallback()
  {
    this->FiberLimiter    = vtkLimitFibersToVOI::New();
    this->ROIFiberLimiter = vtkLimitFibersToROI::New();
    this->FiberLimiter->SetInputConnection ( this->ROIFiberLimiter->GetOutputPort() );    
  }
  ~vtkFibersManagerCallback()
  {
    this->FiberLimiter->Delete();
    this->ROIFiberLimiter->Delete();
  }
  
 private:
  vtkLimitFibersToVOI* FiberLimiter;
  vtkLimitFibersToROI* ROIFiberLimiter;
};


void vtkFibersManagerCallback::Execute (vtkObject *caller, unsigned long, void*)
{
  vtkBoxWidget *widget = vtkBoxWidget::SafeDownCast(caller);

  vtkPolyData* input = (vtkPolyData*)widget->GetInput();
  if( !input )
  {
    return;
  }
  
  // Get the poly data defined by the vtkBoxWidget
  vtkPolyData* myBox = vtkPolyData::New();
  widget->GetPolyData(myBox);
  
  // myBox contains 15 points and points 8 to 13
  // define the bounding box
  double xmin, xmax, ymin, ymax, zmin, zmax;
  double* pt = myBox->GetPoint(8);
  xmin = pt[0];
  pt = myBox->GetPoint(9);
  xmax = pt[0];
  pt = myBox->GetPoint(10);
  ymin = pt[1];
  pt = myBox->GetPoint(11);
  ymax = pt[1];
  pt = myBox->GetPoint(12);
  zmin = pt[2];
  pt = myBox->GetPoint(13);
  zmax = pt[2];
  
  this->FiberLimiter->SetVOI (xmin, xmax, ymin, ymax, zmin, zmax);
  this->ROIFiberLimiter->Update();
  this->FiberLimiter->Update();
  
  myBox->Delete();  
}



/**
   vtkFiberPickerCallback decalaration & implementation
*/
class VTK_VISUMANAGEMENT_EXPORT vtkFiberPickerCallback: public vtkCommand
{
 public:
  static vtkFiberPickerCallback* New()
  { return new vtkFiberPickerCallback; }
  
  virtual void Execute ( vtkObject*, unsigned long, void*);
  
  void SetInput (vtkPolyData* input)
  { this->Input = input; }
  
  vtkPolyData* GetInput() const
  { return this->Input; }
  
  void SetFiberImage (vtkPolyData* image)
  { this->FiberImage = image; }
  
  vtkPolyData* GetFiberImage() const
  { return this->FiberImage; }
  
  void SetFibersManager (vtkFibersManager* manager)
  { this->FibersManager = manager; }
  
  vtkActor* GetPickedActor() const
  { return this->PickedActor; }
  
  void DeletePickedCell();
  
  
 protected:
  vtkFiberPickerCallback()
  {
    this->Input        = 0;
    this->FiberImage   = 0;
    this->FibersManager= 0;
    this->PickedFiber  = vtkPolyData::New();
    this->PickedMapper = vtkPolyDataMapper::New();
    this->PickedActor  = vtkActor::New();

    this->PickedMapper->SetInput ( this->PickedFiber );
    this->PickedActor->SetMapper ( this->PickedMapper );
    this->PickedActor->GetProperty()->SetColor ( 1.0, 0.0, 0.0 );
    this->PickedActor->GetProperty()->SetLineWidth ( 5.0 );
    
    this->PickedFiber->Initialize();
    this->PickedFiber->Allocate();
    
    this->PickedCellId = -1;    
  }
  ~vtkFiberPickerCallback()
  {
    this->PickedFiber->Delete();
    this->PickedMapper->Delete();
    this->PickedActor->Delete();
  }
  
 private:
  vtkPolyData*       Input;
  vtkPolyData*       FiberImage;
  
  vtkPolyData*       PickedFiber;
  vtkPolyDataMapper* PickedMapper;
  vtkActor*          PickedActor;
  int                PickedCellId;
  
  vtkFibersManager*  FibersManager;
};


void vtkFiberPickerCallback::Execute ( vtkObject *caller, unsigned long event, void* )
{
  vtkCellPicker *picker = vtkCellPicker::SafeDownCast(caller);
  if( !picker )
  {
    return;
  }
  
  int cellid = picker->GetCellId();
  picker->SetPath (NULL); // to prevent a bounding box to appear
        
  if( cellid < 0 )
  {
    this->PickedCellId = -1;
    this->PickedActor->SetVisibility (0);
    return;
  }
  else
  {
    this->PickedCellId = cellid;
    if( this->GetInput() )
    {
      vtkCellArray* lines = this->Input->GetLines();
      if( !lines )
      {
        return;
      }      
      
      vtkCellArray* array = vtkCellArray::New();           
      array->InsertNextCell ( this->Input->GetCell (cellid) );
      
      this->PickedFiber->Initialize();
      this->PickedFiber->SetPoints ( this->Input->GetPoints() );
      
      switch( this->Input->GetCellType (cellid) )
      {
          case VTK_POLY_LINE:
            this->PickedFiber->SetLines (array);
            break;
            
          default:
            break;
      }
      
      array->Delete();
      
      this->PickedMapper->SetInput ( this->PickedFiber );
      this->PickedActor->SetVisibility (1);
    }
  } 
}

void vtkFiberPickerCallback::DeletePickedCell()
{
  if( !this->FiberImage || !this->Input || this->PickedCellId==-1)
  {
    return;
  }

  vtkCell* pickedCell = this->Input->GetCell (this->PickedCellId);
  
  int npts       = pickedCell->GetNumberOfPoints();
  vtkIdType* pto = pickedCell->GetPointIds()->GetPointer (0);

  vtkCellArray* realLines = this->FiberImage->GetLines();
  if( !realLines )
  {
    return;
  }

  vtkPoints* points = this->Input->GetPoints();
  if( points != this->FiberImage->GetPoints() )
  {
    return;
  }
  
  realLines->InitTraversal();
  vtkIdType rnpts, *rpto;
  
  vtkIdType test = realLines->GetNextCell (rnpts, rpto);
  int cellid = 0;
  while ( test!=0 )
  {
    if( rnpts == npts )
    {
      int i=0;
      for( i=0; i<npts && pto[i]==rpto[i]; )
      {
        i++;
      }

      if( i==npts && pto[i-1]==rpto[i-1])
      {
        // found and remove
        for( int j=0; j<npts; j++)
        {
          rpto[j]=rpto[0]; // collapse the fiber on itself
        }
        
        this->FiberImage->Modified();
	
        if( this->FibersManager )
        {
          this->FibersManager->Execute(); // force the box widget to refresh
        }
        
        this->PickedActor->SetVisibility (0);
	
        break;
      }
    }
    test = realLines->GetNextCell (rnpts, rpto);
    cellid++;
  }
}



/**
   vtkFiberKeyboardCallback decalaration & implementation
*/
class VTK_VISUMANAGEMENT_EXPORT vtkFiberKeyboardCallback: public vtkCommand
{
 public:
  static vtkFiberKeyboardCallback* New()
  { return new vtkFiberKeyboardCallback; }
  
  virtual void Execute ( vtkObject*, unsigned long, void*);
  
  void SetFiberPickerCallback (vtkFiberPickerCallback* callback)
  { this->FiberPickerCallback = callback; }
  
  void SetFiberManager(vtkFibersManager* man)
  { this->FibersManager = man; }
  
 protected:
  vtkFiberKeyboardCallback()
  {
    this->FiberPickerCallback = 0;
    this->FibersManager       = 0;
  }
  ~vtkFiberKeyboardCallback(){};
  
 private:
  vtkFiberPickerCallback *FiberPickerCallback;
  vtkFibersManager       *FibersManager;
};

void vtkFiberKeyboardCallback::Execute ( vtkObject *caller, unsigned long event, void* )
{
  vtkRenderWindowInteractor* rwin = vtkRenderWindowInteractor::SafeDownCast (caller);
  
  switch( rwin->GetKeyCode() )
  {    
      case 'h':
      case 'H':	
	if( this->FibersManager )
	{
	  if(  this->FibersManager->GetHelpMessageVisibility() )
	  {
	    this->FibersManager->HideHelpMessage();
	  }
	  else
	  {
	    this->FibersManager->ShowHelpMessage();
	  }
	  rwin->Render();
	}
	break;
  
      case 'b':
	if( this->FibersManager )
	{
	  if ( this->FibersManager->GetBoxWidgetVisibility() )
	  {
	    this->FibersManager->BoxWidgetOff();
	  }
	  else
	  {
	    this->FibersManager->BoxWidgetOn();
	  }
	}
	break;
	
      case '4':
	if( this->FibersManager )
	{
	  this->FibersManager->SetRenderingModeToPolyLines();
	  rwin->Render();
	}
	break;
	
	
      case '5':
	if( this->FibersManager )
	{
	  this->FibersManager->SetRenderingModeToRibbons();
	  rwin->Render();
	}
	break;
	
	
      case '6':
	if( this->FibersManager )
	{
	  this->FibersManager->SetRenderingModeToTubes();
	  rwin->Render();
	}
	break;
	
      case '7':	
	if( this->FibersManager )
	{
	  vtkFibersManager::UseHardwareShadersOff();
	  this->FibersManager->ChangeMapperToDefault();
	  rwin->Render();
	}	
	break;
	
      case '8':	
	if( this->FibersManager )
	{
	  vtkFibersManager::UseHardwareShadersOn();
 	  this->FibersManager->ChangeMapperToUseHardwareShaders();
	  rwin->Render();
	}	
	break;
	
	
      case 'n':
      case 'N':
	if( this->FibersManager )
	{
	  this->FibersManager->SwapInputOutput();
	  rwin->Render();
	}	
	break;
	
	
      case 'u':
      case 'U':
	if( this->FibersManager )
	{
	  this->FibersManager->Reset();
	  rwin->Render();
	}	
	break;
	
      case 'd':
      case 'D':	
	if( this->FiberPickerCallback )
	{
	  this->FiberPickerCallback->DeletePickedCell();
	  rwin->Render();
	}        
	break;
	
      default:
	break;
  }  
}



/**
   vtkFiberKeyboardCallback decalaration & implementation
*/
vtkFibersManager::vtkFibersManager()
{
  this->Input                  = 0;  
  this->RenderWindowInteractor = 0;
  this->Renderer               = 0;

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

  this->PickerCallback->SetInput ( this->Callback->GetOutput() );
  this->PickerCallback->SetFibersManager (this);
  

#ifdef vtkINRIA3D_USE_HWSHADING
  if( vtkFibersManager::GetUseHardwareShaders() )
  {
    vtkFiberMapper* mapper = vtkFiberMapper::New();
    mapper->SetAmbientContributionShadow(0.0);
    mapper->SetDiffuseContributionShadow(0.6);
    mapper->SetSpecularContributionShadow(0.0);
    mapper->LightingOff();
    mapper->ShadowingOff();
    this->Mapper = mapper;
  }
  else
  {
    vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
    mapper->ImmediateModeRenderingOn();
    this->Mapper = mapper;
  }
#else
    vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
    mapper->ImmediateModeRenderingOn();
    this->Mapper = mapper;   
#endif
    
  this->Mapper->SetInput ( this->Callback->GetOutput() );
  this->Mapper->SetScalarModeToUsePointData();
  // this->Actor->SetMapper (this->Mapper); // only when input is set

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
    "7: Use software rendering\n"
    "8: Use hardware rendering\n"
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
    // this->RemoveAllActors();
    this->RenderWindowInteractor->Delete();
  }
    
  if (this->Renderer)
    this->Renderer->Delete();
  
  this->BoxWidget->RemoveObserver (this->Callback);
  // this->BoxWidget->SetInteractor (NULL);
  
  this->BoxWidget->Delete();
  this->Callback->Delete();
  this->PickerCallback->Delete();
  this->KeyboardCallback->Delete();
  this->Squeezer->Delete();
  this->Cleaner->Delete();
  this->TubeFilter->Delete();
  this->RibbonFilter->Delete();
  this->HelpMessage->Delete();

  this->Actor->SetMapper(NULL);
  
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
    
    /*
    if (!this->Renderer)
    {
      int numLayers = this->RenderWindowInteractor->GetRenderWindow()->GetNumberOfLayers();
      this->RenderWindowInteractor->GetRenderWindow()->SetNumberOfLayers ( numLayers + 1 );
    
      this->Renderer = vtkRenderer::New();
      this->Renderer->SetLayer ( numLayers );

      this->RenderWindowInteractor->GetRenderWindow()->GetRenderers()->InitTraversal();
      if (vtkRenderer* first_renderer = this->RenderWindowInteractor->GetRenderWindow()->GetRenderers()->GetNextItem())
	this->Renderer->SetActiveCamera ( first_renderer->GetActiveCamera() );
    
      this->RenderWindowInteractor->GetRenderWindow()->AddRenderer ( this->Renderer );
    
      this->Renderer->Delete();
    }
    */
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
    if( this->RenderWindowInteractor != NULL )
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

  /*
  if (this->RenderWindowInteractor)
  {
    vtkCellPicker* picker = vtkCellPicker::New();
    picker->AddObserver (vtkCommand::EndPickEvent, this->PickerCallback, 0.0 );
    picker->SetTolerance (0.001);
    
    this->RenderWindowInteractor->SetPicker ( picker );
    this->RenderWindowInteractor->AddObserver (vtkCommand::CharEvent, this->KeyboardCallback, 0.0 );
    
    picker->Delete();

    if (!this->Renderer)
    {
      int numLayers = this->RenderWindowInteractor->GetRenderWindow()->GetNumberOfLayers();
      this->RenderWindowInteractor->GetRenderWindow()->SetNumberOfLayers ( numLayers + 1 );
    
      this->Renderer = vtkRenderer::New();
      this->Renderer->SetLayer ( numLayers );

      this->RenderWindowInteractor->GetRenderWindow()->GetRenderers()->InitTraversal();
      if (vtkRenderer* first_renderer = this->RenderWindowInteractor->GetRenderWindow()->GetRenderers()->GetNextItem())
	this->Renderer->SetActiveCamera ( first_renderer->GetActiveCamera() );
    
      this->RenderWindowInteractor->GetRenderWindow()->AddRenderer ( this->Renderer );
    
      this->Renderer->Delete();
    }

    this->Renderer->AddActor ( this->Actor );
    this->Renderer->AddActor ( this->PickerCallback->GetPickedActor() );
    this->Renderer->AddActor ( this->HelpMessage );
  }
  */
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
  
  this->Callback->GetROIFiberLimiter()->SetInput ( this->GetInput() );
  this->Actor->SetMapper (this->Mapper);
  
  if( this->Renderer )
  {
    this->BoxWidget->SetDefaultRenderer(this->Renderer);
  }
  this->BoxWidget->SetInput ( this->GetInput() );
  this->BoxWidget->PlaceWidget();
  
  this->PickerCallback->SetFiberImage ( this->GetInput() );
  
  this->Callback->Execute (this->BoxWidget, 0, NULL);

  this->Mapper->SetScalarRange (0.0, 1.0);
  
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

  this->BoxWidget->SetInput ( auxPoly );
  this->Callback->GetFiberLimiter()->SetInput ( auxPoly );

  this->PickerCallback->SetFiberImage ( auxPoly );
  
  auxPoly->Delete();
  auxLines->Delete();
}

void vtkFibersManager::Reset()
{
  if( !this->GetInput() )
    return;
  
  this->BoxWidget->SetInput ( this->GetInput() );
  this->Callback->GetROIFiberLimiter()->SetInput ( this->GetInput() );
  this->PickerCallback->SetFiberImage ( this->GetInput() );
  
  this->Callback->GetFiberLimiter()->SetInputConnection ( this->Callback->GetROIFiberLimiter()->GetOutputPort() );
  
  this->Callback->Execute (this->BoxWidget, 0, NULL);
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
  
#ifdef vtkINRIA3D_USE_HWSHADING
  vtkFiberMapper* mapper = vtkFiberMapper::SafeDownCast (this->Mapper);
  if( mapper )
  {
    mapper->LightingOn();
    mapper->ShadowingOn();
  }
  else // the rendering is not supported
       // and the mapper is a regular polydatamapper
  {
    this->Mapper->SetInput (this->TubeFilter->GetOutput());
  }
  
#else
  // no shading support
  this->Mapper->SetInput (this->TubeFilter->GetOutput());
#endif
}

void vtkFibersManager::SetRenderingModeToRibbons()
{
  vtkFiberRenderingStyle = RENDER_IS_RIBBONS;
  
#ifdef vtkINRIA3D_USE_HWSHADING
  vtkFiberMapper* mapper = vtkFiberMapper::SafeDownCast (this->Mapper);
  if( mapper )
  {
    mapper->LightingOn();
    mapper->ShadowingOff();
  }
  else
  {
    this->Mapper->SetInput (this->RibbonFilter->GetOutput());
  }
#else
  this->Mapper->SetInput (this->RibbonFilter->GetOutput());
#endif 
}

void vtkFibersManager::SetRenderingModeToPolyLines()
{
  vtkFiberRenderingStyle = RENDER_IS_POLYLINES;
  
#ifdef vtkINRIA3D_USE_HWSHADING
  vtkFiberMapper* mapper = vtkFiberMapper::SafeDownCast (this->Mapper);
  if( mapper )
  {
    mapper->LightingOff();
    mapper->ShadowingOff();
  }
  else
  {
    this->PickerCallback->SetInput (this->Callback->GetOutput());
    this->Mapper->SetInput (this->Callback->GetOutput());
  }
#else
  this->PickerCallback->SetInput (this->Callback->GetOutput());
  this->Mapper->SetInput (this->Callback->GetOutput());
#endif
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

void vtkFibersManager::ChangeMapperToUseHardwareShaders()
{
#ifdef vtkINRIA3D_USE_HWSHADING
  vtkDebugMacro(<<"Hardware shading is activated.");
  
  if( vtkFiberMapper::SafeDownCast( this->Mapper ) )
  {
    return;
  }
  
  vtkFiberMapper* mapper = vtkFiberMapper::New();
  
  if( !mapper->IsRenderSupported(this->Renderer) )
  {
    vtkErrorMacro(<<"Error: Hardware acceleration is not supported.");
    mapper->Delete();
    return;
  }
  
  mapper->SetAmbientContributionShadow(0.0);
  mapper->SetDiffuseContributionShadow(0.6);
  mapper->SetSpecularContributionShadow(0.0);
  mapper->LightingOff();
  mapper->ShadowingOff();

  mapper->SetInput( this->Callback->GetOutput() );
  this->Actor->SetMapper( mapper );

  if( this->Mapper )
  {
    mapper->SetScalarMode ( this->Mapper->GetScalarMode() );
    mapper->SelectColorArray ( this->Mapper->GetArrayId() );
    mapper->SetLookupTable ( this->Mapper->GetLookupTable() );
    this->Mapper->Delete(); 
    this->Mapper = 0;
  }
  this->Mapper = mapper;
  
  this->SetRenderingMode( vtkFibersManager::GetRenderingMode() );
#endif
}

void vtkFibersManager::ChangeMapperToDefault()
{
#ifdef vtkINRIA3D_USE_HWSHADING
  if( !vtkFiberMapper::SafeDownCast( this->Mapper ) )
  {
    return;
  }
  
  vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
  mapper->ImmediateModeRenderingOn();  
  mapper->SetInput( this->Callback->GetOutput() );
  this->Actor->SetMapper( mapper );
  
  if( this->Mapper )
  {
    mapper->SetScalarMode ( this->Mapper->GetScalarMode() );
    mapper->SelectColorArray ( this->Mapper->GetArrayId() );
    mapper->SetLookupTable ( this->Mapper->GetLookupTable() );
    this->Mapper->Delete(); 
    this->Mapper = 0;
  }
  this->Mapper = mapper;
  
  this->SetRenderingMode( vtkFibersManager::GetRenderingMode() );
#endif
}


void vtkFibersManager::SetUseHardwareShaders(int i)
{
  vtkUseHardwareShaders = i;
}

int vtkFibersManager::GetUseHardwareShaders()
{ 
  return vtkUseHardwareShaders; 
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

void vtkFibersManager::UseHardwareShadersOn()
{
  vtkFibersManager::SetUseHardwareShaders(1);
}
  
void vtkFibersManager::UseHardwareShadersOff()
{
  vtkFibersManager::SetUseHardwareShaders(0);
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

void vtkFibersManager::SetLookupTable (vtkScalarsToColors* lut)
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
  //m_Callback->Execute(m_BoxWidget, 0, 0);
}
