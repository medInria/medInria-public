/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkFibersManager.cxx 1467 2010-02-17 23:31:08Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2010-02-18 00:31:08 +0100 (Thu, 18 Feb 2010) $
Version:   $Revision: 1467 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "vtkFibersManager.h"

#include <vtkProperty.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
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

static int vtkUseHardwareShaders = 0;
static int vtkFiberRenderingStyle = 0;


void vtkFibersManagerCallback::Execute ( vtkObject *caller, unsigned long, void* )
{
  // get the box widget
//   vtkBoxWidget *widget = reinterpret_cast<vtkBoxWidget*>(caller);
  vtkBoxWidget *widget = vtkBoxWidget::SafeDownCast(caller);

  // get the inputed vtkPolyData
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


void vtkFiberPickerCallback::Execute ( vtkObject *caller, unsigned long event, void* )
{

  // get the cell picker
  vtkCellPicker *picker = vtkCellPicker::SafeDownCast(caller);
  if( !picker )
  {
    return;
  }
  
  int cellid = picker->GetCellId();
  picker->SetPath (NULL); // to prevent a bounding box to appear
        
  if( cellid < 0 )
  {
    m_PickedCellId = -1;
    m_PickedActor->SetVisibility (0);
    return;
  }
  else
  {

    m_PickedCellId = cellid;
    
    // double *pt = picker->GetPickPosition();
    //std::cout << "Picked cell Id: " << cellid
	//<< " Picked point: "   << pt[0] << " " << pt[1] << " " << pt[2] << std::endl;
          
		  
    if( this->GetInput() )
    {
      
      vtkCellArray* lines = m_Input->GetLines();
      if( !lines )
      {
        return;
      }      
      
      vtkCellArray* array = vtkCellArray::New();           
      array->InsertNextCell ( m_Input->GetCell (cellid) );
      
      m_PickedFiber->Initialize();      
      
      m_PickedFiber->SetPoints ( m_Input->GetPoints() );
      
      switch( m_Input->GetCellType (cellid) )
      {
        
          case VTK_POLY_LINE:
            m_PickedFiber->SetLines (array);
            break;
            
          default:
            std::cerr << "Only polylines are pickable, sorry!" << std::endl;
            break;
            
      }
      
      array->Delete();
      
      m_PickedMapper->SetInput ( m_PickedFiber );
      m_PickedActor->SetVisibility (1);
      
    }
    
  }
  
    
}


void vtkFiberPickerCallback::DeletePickedCell()
{

  if( !m_FiberImage || !m_Input || m_PickedCellId==-1)
  {
    std::cerr << "Something was wrong here." << std::endl;
    return;
  }

  /*
  vtkCellArray* lines = m_Input->GetLines();
  if( !lines )
  {
    return;
  }
  lines->InitTraversal();
  */

  vtkCell* pickedCell = m_Input->GetCell (m_PickedCellId);
  
  //lines->GetCell ( m_PickedCellId, npts, pto);
  int npts       = pickedCell->GetNumberOfPoints();
  vtkIdType* pto = pickedCell->GetPointIds()->GetPointer (0);

  /*
  std::cout << "npts: " << npts << std::endl;
  for( int i=0; i<npts; i++)
  {
    std::cout << pto[i] << " ";
  }
  std::cout << std::endl;
  std::cout << std::endl;
  */
  
  
  vtkCellArray* realLines = m_FiberImage->GetLines();
  if( !realLines )
  {
    return;
  }


  vtkPoints* points = m_Input->GetPoints();
  if( points != m_FiberImage->GetPoints() )
  {
    std::cerr << "Error: Point buffers are different!" << std::endl;
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
        
        //realLines->ReplaceCell (cellid, npts, NULL);
        //m_Input->SetLines (realLines);
        //m_Input->Modified();
        m_FiberImage->Modified();

        if( m_FibersManager )
        {
          m_FibersManager->Execute(); // force the box widget to refresh
        }
        
        m_PickedActor->SetVisibility (0);
        return;
      }
    }
    

    test = realLines->GetNextCell (rnpts, rpto);
    cellid++;
  }
  
  
  
  
}




void vtkFiberKeyboardCallback::Execute ( vtkObject *caller, unsigned long event, void* )
{
  
  vtkRenderWindowInteractor* rwin = vtkRenderWindowInteractor::SafeDownCast (caller);
  
  //std::cout << rwin->GetKeyCode() << " " << rwin->GetKeySym() << std::endl;
  
  switch( rwin->GetKeyCode() )
  {
    
      case 'h':
      case 'H':
	
	if( m_FibersManager )
	{
	  if(  m_FibersManager->GetHelpMessageVisibility() )
	  {
	    m_FibersManager->HideHelpMessage();
	  }
	  else
	  {
	    m_FibersManager->ShowHelpMessage();
	  }
	  rwin->Render();
	}
	
	break;
  
     case 'b':
      if( m_FibersManager )
      {
        if ( m_FibersManager->GetBoxWidgetVisibility() )
	{
	  m_FibersManager->BoxWidgetOff();
	}
	else
	{
	  m_FibersManager->BoxWidgetOn();
	}
      }
      break;

    case '4':
      if( m_FibersManager )
      {
	m_FibersManager->SetRenderingModeToPolyLines();
	rwin->Render();
      }
      break;
      
      
      case '5':
	if( m_FibersManager )
	{
	  m_FibersManager->SetRenderingModeToRibbons();
	  rwin->Render();
	}
	break;
	
	
      case '6':
	if( m_FibersManager )
	{
	  m_FibersManager->SetRenderingModeToTubes();
	  rwin->Render();
	}
	break;
	
      case '7':
	
	if( m_FibersManager )
	{
	  vtkFibersManager::UseHardwareShadersOff();
	  m_FibersManager->ChangeMapperToDefault();
	  rwin->Render();
	}
	
	break;

      case '8':
	
	if( m_FibersManager )
	{
	  vtkFibersManager::UseHardwareShadersOn();
 	  m_FibersManager->ChangeMapperToUseHardwareShaders();
	  rwin->Render();
	}
	
	break;


      case 'n':
      case 'N':
	if( m_FibersManager )
	{
	  m_FibersManager->SwapInputOutput();
	  rwin->Render();
	}
	
	break;


      case 'u':
      case 'U':
	if( m_FibersManager )
	{
	  m_FibersManager->Reset();
	  rwin->Render();
	}
	
	break;
	
      case 'd':
      case 'D':
	
	if( m_FiberPickerCallback )
	{
	  m_FiberPickerCallback->DeletePickedCell();
	  rwin->Render();
	}
        
	break;

      default:
	break;
    
  }
  
}



vtkFibersManager::vtkFibersManager()
{
  m_Input = 0;
  m_RWin = 0;
	this->Renderer = 0;

  m_BoxWidget    = vtkBoxWidget::New();
  m_Callback     = vtkFibersManagerCallback::New();
  m_Squeezer     = vtkMaskPolyData::New();
  m_Cleaner      = vtkCleanPolyData::New();
  m_TubeFilter   = vtkTubeFilter::New();
  m_RibbonFilter = vtkRibbonFilter::New();

  m_PickerCallback = vtkFiberPickerCallback::New();
  m_KeyboardCallback = vtkFiberKeyboardCallback::New();

  m_Actor        = vtkActor::New();
  
  m_KeyboardCallback->SetFiberPickerCallback ( m_PickerCallback );
  m_KeyboardCallback->SetFiberManager(this);
  
  this->MaximumNumberOfFibers = 20000;
  
  m_BoxWidget->SetKeyPressActivationValue ('0');
  m_BoxWidget->RotationEnabledOff();
  m_BoxWidget->SetPlaceFactor (1.0);
  m_BoxWidget->AddObserver (vtkCommand::InteractionEvent, m_Callback);
  m_BoxWidgetVisibility = true;
  // link the limiters

  m_Squeezer->ReleaseDataFlagOn();
  
  //m_Cleaner->ToleranceIsAbsoluteOn();
  //m_Cleaner->SetTolerance (5.0);
  m_Cleaner->SetInputConnection ( m_Squeezer->GetOutputPort() );
  m_Cleaner->ReleaseDataFlagOn();
  
  m_TubeFilter->SetInputConnection( m_Cleaner->GetOutputPort() );
  m_TubeFilter->SetRadius (0.15);
  m_TubeFilter->SetNumberOfSides (4);
  m_TubeFilter->CappingOn();
  m_TubeFilter->ReleaseDataFlagOn();

  //m_TubeFilter->SetNumberOfSides (10);
  //m_TubeFilter->SetVaryRadiusToVaryRadiusByAbsoluteScalar();

  //m_RibbonFilter->SetInput(m_Squeezer->GetOutput());
  m_RibbonFilter->SetInputConnection( m_Cleaner->GetOutputPort() );
  m_RibbonFilter->SetWidth (0.15);
  m_RibbonFilter->ReleaseDataFlagOn();

  //m_RibbonFilter->VaryWidthOn ();

  m_PickerCallback->SetInput ( m_Callback->GetOutput() );
  m_PickerCallback->SetFibersManager (this);
  

#ifdef vtkINRIA3D_USE_HWSHADING
  if( vtkFibersManager::GetUseHardwareShaders() )
  {
    vtkFiberMapper* mapper = vtkFiberMapper::New();
    if( !mapper->IsRenderSupported() )
    {
      vtkErrorMacro(<<"Error: Hardware acceleration is not supported, switching to software rendering.");
      mapper->Delete();
      vtkPolyDataMapper* mapper2 = vtkPolyDataMapper::New();
      mapper2->ImmediateModeRenderingOn();
      m_Mapper = mapper2;
    }
    else
    {
      mapper->SetAmbientContributionShadow(0.0);
      mapper->SetDiffuseContributionShadow(0.6);
      mapper->SetSpecularContributionShadow(0.0);
      mapper->LightingOff();
      mapper->ShadowingOff();
      m_Mapper = mapper;
    }
  }
  else
  {
    vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
    mapper->ImmediateModeRenderingOn();
    m_Mapper = mapper;
  }
#else
    vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
    mapper->ImmediateModeRenderingOn();
    m_Mapper = mapper;   
#endif
    
  m_Mapper->SetInput ( m_Callback->GetOutput() );
  m_Mapper->SetScalarModeToUsePointData();
  m_Actor->SetMapper (m_Mapper);

  m_HelpMessage = vtkCornerAnnotation::New();
  m_HelpMessage->SetNonlinearFontScaleFactor (0.25);
  vtkTextProperty* textProperty = vtkTextProperty::New();
  textProperty->SetColor (0.0,0.0,0.0);
  m_HelpMessage->SetTextProperty ( textProperty );
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
  
  m_HelpMessage->SetText (2, helpMessage );

  this->SetRenderingMode( vtkFibersManager::GetRenderingMode() );

}


vtkFibersManager::~vtkFibersManager()
{
  if( m_RWin )
  {
    this->RemoveAllActors();
    m_RWin->Delete();
  }
  
  m_BoxWidget->RemoveObserver (m_Callback);
  m_BoxWidget->SetInteractor (NULL);

  m_BoxWidget->Delete();
  m_Callback->Delete();
  m_PickerCallback->Delete();
  m_KeyboardCallback->Delete();
  m_Squeezer->Delete();
  m_Cleaner->Delete();
  m_TubeFilter->Delete();
  m_RibbonFilter->Delete();
  m_HelpMessage->Delete();

  m_Actor->SetMapper(NULL);

  m_Mapper->Delete();
  m_Actor->Delete();
}



void vtkFibersManager::SetRenderWindowInteractor (vtkRenderWindowInteractor* rwin)
{
  if( rwin != m_RWin )
  {
    if( m_RWin != NULL )
    {
      this->RemoveAllActors();
      m_RWin->RemoveObserver(m_KeyboardCallback);
      m_RWin->UnRegister (this);
    }
    m_BoxWidget->SetInteractor (rwin);
    m_RWin = rwin;
	  
    if( m_RWin )
    {
      m_RWin->Register(this);
    }
  }
}




void vtkFibersManager::RemoveAllActors()
{
  if( m_RWin && m_RWin->GetRenderWindow() )
  {
	  vtkRenderer *renderer = this->Renderer;
	  if (!renderer)
	  {
    m_RWin->GetRenderWindow()->GetRenderers()->InitTraversal();
    //     vtkRenderer* first_renderer = m_RWin->GetRenderWindow()->GetRenderers()->GetNextItem();
    renderer = m_RWin->FindPokedRenderer(m_RWin->GetLastEventPosition()[0],
							    m_RWin->GetLastEventPosition()[1]);
	  }
    if (renderer)
    {
      renderer->RemoveActor ( m_Actor );
      renderer->RemoveActor ( m_PickerCallback->GetPickedActor() );
      renderer->RemoveActor ( m_HelpMessage );
    }
  }
}


void vtkFibersManager::SetInput(vtkPolyData* input)
{
  if( !input )
  {
    return;
  }

  /*  
  if( !input->GetNumberOfPoints() )
  {
    return;
  }
  */
  
  this->Initialize();
  m_Input = input;

  
  m_Callback->GetROIFiberLimiter()->SetInput ( this->GetInput() );

  
	if( this->Renderer )
	{
		m_BoxWidget->SetDefaultRenderer(this->Renderer);
	}
  m_BoxWidget->SetInput ( this->GetInput() );
  m_BoxWidget->PlaceWidget();


  m_PickerCallback->SetFiberImage ( this->GetInput() );
  

  m_Callback->Execute (m_BoxWidget, 0, NULL);

  

  m_Mapper->SetScalarRange (0.0, 1.0);
  

  int totalLines = m_Callback->GetOutput()->GetNumberOfLines();
  double ratio = (double)totalLines/(double)(this->MaximumNumberOfFibers);

  //std::cout << "Max number of lines: " << this->MaximumNumberOfFibers << ". Actual number of lines: " << totalLines << ". Ratio: " << ratio <<std::endl;

  ratio = ratio<1.0?1.0:ratio;
  
  m_Squeezer->SetOnRatio ( (int)ratio );
  //m_Squeezer->SetInputConnection( m_Callback->GetFiberLimiter()->GetOutputPort() );

  //m_Cleaner->SetInputConnection( m_Callback->GetFiberLimiter()->GetOutputPort() );
  m_TubeFilter->SetInputConnection( m_Callback->GetFiberLimiter()->GetOutputPort() );
  m_RibbonFilter->SetInputConnection( m_Callback->GetFiberLimiter()->GetOutputPort() );

  // add the actor to the rwin
  if( m_RWin )
  {
	  vtkRenderer *renderer = this->Renderer;
	  if (!renderer)
	  {
    m_RWin->GetRenderWindow()->GetRenderers()->InitTraversal();
    renderer = m_RWin->FindPokedRenderer(
				m_RWin->GetLastEventPosition()[0],
				m_RWin->GetLastEventPosition()[1]);
	  }

    if (renderer)
    {
      renderer->AddActor ( m_Actor );
      renderer->AddActor ( m_PickerCallback->GetPickedActor() );
      renderer->AddActor ( m_HelpMessage );
    }

    vtkCellPicker* picker = vtkCellPicker::New();
    picker->AddObserver (vtkCommand::EndPickEvent, m_PickerCallback, 0.0 );
    picker->SetTolerance (0.001);

    m_RWin->SetPicker ( picker );

    m_RWin->AddObserver (vtkCommand::CharEvent, m_KeyboardCallback, 0.0 );
    
    picker->Delete();
    
  }

}


void vtkFibersManager::SwapInputOutput()
{
  vtkPolyData* auxPoly = vtkPolyData::New();
  auxPoly->SetPoints( m_Callback->GetOutput()->GetPoints() );
  auxPoly->GetPointData()->SetScalars( m_Callback->GetOutput()->GetPointData()->GetScalars() );

  vtkCellArray* auxLines = vtkCellArray::New();
  auxLines->DeepCopy( m_Callback->GetOutput()->GetLines() );
  auxPoly->SetLines( auxLines);

  m_BoxWidget->SetInput ( auxPoly );
  m_Callback->GetFiberLimiter()->SetInput ( auxPoly );

  m_PickerCallback->SetFiberImage ( auxPoly );
  
  auxPoly->Delete();
  auxLines->Delete();
}


void vtkFibersManager::Reset()
{
  if( !this->GetInput() ) return;

  m_BoxWidget->SetInput ( this->GetInput() );
  m_Callback->GetROIFiberLimiter()->SetInput ( this->GetInput() );
  m_PickerCallback->SetFiberImage ( this->GetInput() );

  m_Callback->GetFiberLimiter()->SetInputConnection ( m_Callback->GetROIFiberLimiter()->GetOutputPort() );
  
  //this->Execute();
  m_Callback->Execute (m_BoxWidget, 0, NULL);
}


void vtkFibersManager::SetVisibility (bool isVisible)
{
  m_Actor->SetVisibility (isVisible);
  if (isVisible)
  {
    this->SetBoxWidget (m_BoxWidgetVisibility);
  }
}

void vtkFibersManager::Initialize (void)
{    
  m_Callback->GetFiberLimiter()->GetOutput()->Initialize();
  m_Callback->GetROIFiberLimiter()->GetOutput()->Initialize();
  m_Callback->GetROIFiberLimiter()->RemoveAllInputs();

  m_TubeFilter->GetOutput()->Initialize();

  m_RibbonFilter->GetOutput()->Initialize();
  m_Squeezer->GetOutput()->Initialize(); 
  
  m_Input=0;
  m_PickerCallback->SetFiberImage ( 0 );
}


void vtkFibersManager::SetRenderingModeToTubes()
{
  vtkFiberRenderingStyle = RENDER_IS_TUBES;

#ifdef vtkINRIA3D_USE_HWSHADING
  vtkFiberMapper* mapper = vtkFiberMapper::SafeDownCast (m_Mapper);
  if( mapper )
  {
    mapper->LightingOn();
    mapper->ShadowingOn();
  }
  else // the rendering is not supported
       // and the mapper is a regular polydatamapper
  {
    //m_TubeFilter->Update();
    m_Mapper->SetInput (m_TubeFilter->GetOutput());
  }

#else
  
  // no shading support
  //m_TubeFilter->Update();
  m_Mapper->SetInput (m_TubeFilter->GetOutput());
  
#endif
}


void vtkFibersManager::SetRenderingModeToRibbons()
{
  vtkFiberRenderingStyle = RENDER_IS_RIBBONS;

#ifdef vtkINRIA3D_USE_HWSHADING
  vtkFiberMapper* mapper = vtkFiberMapper::SafeDownCast (m_Mapper);
  if( mapper )
  {
    mapper->LightingOn();
    mapper->ShadowingOff();
  }
  else
  {
    //m_RibbonFilter->Update();
    m_Mapper->SetInput (m_RibbonFilter->GetOutput());
  }
  
#else
  
  //m_RibbonFilter->Update();
  m_Mapper->SetInput (m_RibbonFilter->GetOutput());

#endif
  
}


void vtkFibersManager::SetRenderingModeToPolyLines (void)
{
  vtkFiberRenderingStyle = RENDER_IS_POLYLINES;

#ifdef vtkINRIA3D_USE_HWSHADING
  vtkFiberMapper* mapper = vtkFiberMapper::SafeDownCast (m_Mapper);
  if( mapper )
  {
    mapper->LightingOff();
    mapper->ShadowingOff();
  }
  else
  {
    m_PickerCallback->SetInput (m_Callback->GetOutput());
    m_Mapper->SetInput (m_Callback->GetOutput());
  }
#else
  m_PickerCallback->SetInput (m_Callback->GetOutput());
  m_Mapper->SetInput (m_Callback->GetOutput());
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


void vtkFibersManager::ChangeMapperToUseHardwareShaders(void)
{
#ifdef vtkINRIA3D_USE_HWSHADING
  vtkDebugMacro(<<"Hardware shading is activated.");
  
  if( vtkFiberMapper::SafeDownCast( m_Mapper ) )
  {
    return;
  }
  
  vtkFiberMapper* mapper = vtkFiberMapper::New();
  
  if( !mapper->IsRenderSupported() )
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

  mapper->SetInput( m_Callback->GetOutput() );
  m_Actor->SetMapper( mapper );

  if( m_Mapper )
  {
    mapper->SetScalarMode ( m_Mapper->GetScalarMode() );
    mapper->SelectColorArray ( m_Mapper->GetArrayId() );
    mapper->SetLookupTable ( m_Mapper->GetLookupTable() );
    m_Mapper->Delete(); 
    m_Mapper = 0;
  }
  m_Mapper = mapper;
  
  this->SetRenderingMode( vtkFibersManager::GetRenderingMode() );
  
#endif
}

void vtkFibersManager::ChangeMapperToDefault(void)
{

#ifdef vtkINRIA3D_USE_HWSHADING
  if( !vtkFiberMapper::SafeDownCast( m_Mapper ) )
  {
    return;
  }
  
  vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
  mapper->ImmediateModeRenderingOn();  
  mapper->SetInput( m_Callback->GetOutput() );
  m_Actor->SetMapper( mapper );
  
  if( m_Mapper )
  {
    mapper->SetScalarMode ( m_Mapper->GetScalarMode() );
    mapper->SelectColorArray ( m_Mapper->GetArrayId() );
    mapper->SetLookupTable ( m_Mapper->GetLookupTable() );
    m_Mapper->Delete(); 
    m_Mapper = 0;
  }
  m_Mapper = mapper;

  this->SetRenderingMode( vtkFibersManager::GetRenderingMode() );
#endif
}


void vtkFibersManager::SetUseHardwareShaders(int i)
{
/*
  if( i==0 )
  {
    this->UseHardwareShadersOff();
  }
  else
  {
    this->UseHardwareShadersOn();
  }
*/
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
