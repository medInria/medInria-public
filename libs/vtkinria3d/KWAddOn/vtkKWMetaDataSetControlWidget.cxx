/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWMetaDataSetControlWidget.cxx 489 2007-11-22 12:05:54Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2007-11-22 12:05:54 +0000 (Thu, 22 Nov 2007) $
Version:   $Revision: 489 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "vtkKWMetaDataSetControlWidget.h"
#include "vtkObjectFactory.h"

#include <vtkMetaDataSet.h>
#include <vtkMetaImageData.h>
#include <vtkMetaSurfaceMesh.h>
#include <vtkMetaVolumeMesh.h>
#include <vtkMetaDataSetSequence.h>

#include <vtkProperty.h>
#include <vtkVolumeProperty.h>
#include <vtkImageData.h>
#include <vtkPolyData.h>
#include <vtkDataArray.h>
#include <vtkCellData.h>
#include <vtkPointData.h>

#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkDataSetMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkLookupTable.h>
#include <vtkKWFrameWithScrollbar.h>
#include <vtkKWHSVColorSelector.h>
#include <vtkKWScale.h>
#include <vtkKWMaterialPropertyWidget.h>
#include <vtkKWSurfaceMaterialPropertyWidget.h>
#include <vtkKWVolumeMaterialPropertyWidget.h>
#include "vtkMath.h"

#include <vtkKWRadioButtonSetWithLabel.h>
#include <vtkKWRadioButtonSet.h>
#include <vtkKWRadioButton.h>
#include <vtkKWMenuButtonWithLabel.h>
#include <vtkKWMenuButton.h>
#include <vtkKWMenu.h>

#include "vtkKWColorPresetSelector.h"
#include "vtkKWColorTransferFunctionEditor.h"
#include "vtkKWPiecewiseFunctionEditor.h"
#include "vtkColorTransferFunction.h"
#include <vtkLookupTable.h>
#include "vtkKWPushButton.h"
#include "vtkKWVolumePropertyWidget.h"
#include "vtkKWVolumePropertyPresetSelector.h"
#include "vtkKWHistogramSet.h"
#include "vtkKWHistogram.h"

#include <vtkKWLoadSaveDialog.h>
#include <vtksys/SystemTools.hxx>
#include <vtkStringArray.h>

// #include "vtkKWMainWindowInteractor.h"
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkLookupTableManager.h>
#include <pixmap/KWAddOnResources.h>

#include <vtkExtractTensorComponents.h>
#include <vtkTensorVisuManager.h>
#include <vtkUnstructuredGrid.h>

#include <vtkMatrix4x4.h>
#include<vtkPoints.h>
#include<vtkPointSet.h>

#include <vtkDatasetToImageGenerator.h>


//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWMetaDataSetControlWidget );
vtkCxxRevisionMacro( vtkKWMetaDataSetControlWidget, "$Revision: 489 $");

//----------------------------------------------------------------------------
vtkKWMetaDataSetControlWidget::vtkKWMetaDataSetControlWidget()
{
  this->MetaDataSet                    = NULL;
  this->S_Property                     = NULL;
  this->V_Property                     = NULL;

  this->DataSetControlChangedCommand   = NULL;
  this->DataSetControlChangingCommand  = NULL;
  
  this->MainFrame                      = vtkKWFrameWithScrollbar::New();
  this->HSVColorSelector               = vtkKWHSVColorSelector::New();
  this->OpacityScale                   = vtkKWScale::New();
  this->S_MaterialPropertyWidget       = vtkKWSurfaceMaterialPropertyWidget::New();
  this->V_MaterialPropertyWidget       = vtkKWVolumeMaterialPropertyWidget::New();

  this->SurfaceRepresentationButtonSet = vtkKWRadioButtonSetWithLabel::New();
  this->LineWidthScale                 = vtkKWScale::New();
  this->InterpolationModeButtonSet     = vtkKWRadioButtonSetWithLabel::New();
  this->ComponentSelectionButton       = vtkKWMenuButtonWithLabel::New();

  this->ScalarColorSelector            = vtkKWColorPresetSelector::New();
  this->ScalarColorFunctionEditor      = vtkKWColorTransferFunctionEditor::New();
  this->ScalarColorFunction            = vtkColorTransferFunction::New();
  this->ReadDataButton                 = vtkKWPushButton::New();

  this->VolumePropertyButton           = vtkKWPushButton::New();
  //this->VolumePropertySelector         = vtkKWVolumePropertyPresetSelector::New();
  

  this->InitializeColorFunction();

  this->AssociatedObject = 0;
  
  
  
  
  
}

//----------------------------------------------------------------------------
vtkKWMetaDataSetControlWidget::~vtkKWMetaDataSetControlWidget()
{
  std::cout<<"deleting vtkKWMetaDataSetControlWidget"<<std::endl;
  this->MainFrame->Delete();
  this->HSVColorSelector->Delete();
  this->OpacityScale->Delete();
  this->V_MaterialPropertyWidget->Delete();
  
  this->SurfaceRepresentationButtonSet->Delete();
  this->LineWidthScale->Delete();
  this->InterpolationModeButtonSet->Delete();
  this->ComponentSelectionButton->Delete();
  this->ScalarColorSelector->Delete();
  this->ScalarColorFunctionEditor->Delete();
  this->ScalarColorFunction->Delete();
  this->ReadDataButton->Delete();
  //this->S_MaterialPropertyWidget->Delete();
  this->VolumePropertyButton->Delete();
  //this->VolumePropertySelector->Delete();
  
  
  
  if (this->MetaDataSet)
  {
    this->MetaDataSet->Delete();
  }
  if (this->S_Property)
  {
    this->S_Property->Delete();
  }
  if (this->V_Property)
  {
    this->V_Property->Delete();
  }
}

//----------------------------------------------------------------------------
void vtkKWMetaDataSetControlWidget::InvokeObjectMethodCommand(const char *command)
{
  this->Superclass::InvokeObjectMethodCommand(command);
}

//----------------------------------------------------------------------------
void vtkKWMetaDataSetControlWidget::SetDataSetControlChangedCommand(
  vtkObject *object, const char *method)
{
  this->SetObjectMethodCommand(
    &this->DataSetControlChangedCommand, object, method);
}

//----------------------------------------------------------------------------
void vtkKWMetaDataSetControlWidget::InvokeDataSetControlChangedCommand()
{
  this->InvokeObjectMethodCommand(this->DataSetControlChangedCommand);
}

//----------------------------------------------------------------------------
void vtkKWMetaDataSetControlWidget::SetDataSetControlChangingCommand(
  vtkObject *object, const char *method)
{
  this->SetObjectMethodCommand(
    &this->DataSetControlChangingCommand, object, method);
}

//----------------------------------------------------------------------------
void vtkKWMetaDataSetControlWidget::InvokeDataSetControlChangingCommand()
{
  this->InvokeObjectMethodCommand(this->DataSetControlChangingCommand);
}


//----------------------------------------------------------------------------
void vtkKWMetaDataSetControlWidget::CreateWidget()
{
  // Check if already created
  if (this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }
  
  // Call the superclass to create the whole widget
  this->Superclass::CreateWidget();

  this->SetLabelText ("control");
  
  
  this->MainFrame->SetParent(this->GetFrame());
  this->MainFrame->Create();

  this->HSVColorSelector->SetParent(this->MainFrame->GetFrame());
  this->HSVColorSelector->Create();
  this->HSVColorSelector->SetWidth (70);
  
  this->HSVColorSelector->SetHueSatWheelRadius (40);
  this->HSVColorSelector->SetBalloonHelpString("Color");
  this->HSVColorSelector->SetSelectionChangingCommand (this, "ColorChangingCallback");
  this->HSVColorSelector->SetSelectionChangedCommand (this, "ColorChangedCallback");
  this->HSVColorSelector->InvokeCommandsWithRGBOn();
  
  this->OpacityScale->SetParent(this->MainFrame->GetFrame());
  this->OpacityScale->Create();
  this->OpacityScale->SetViewOrientationToVertical();
  this->OpacityScale->SetRange (0,100);
  this->OpacityScale->SetResolution (1);
  this->OpacityScale->SetLength (80);
  this->OpacityScale->SetSliderLength (10);
  this->OpacityScale->SetActiveBackgroundColor (0.9,0.2,0.4);
  this->OpacityScale->SetBalloonHelpString("Opacity");
  this->OpacityScale->SetValue (100);
  this->OpacityScale->SetCommand (this, "OpacityChangingCallback");

  this->S_MaterialPropertyWidget->SetParent(this->MainFrame->GetFrame());
  this->S_MaterialPropertyWidget->Create();
  this->S_MaterialPropertyWidget->SetPresetSize (30);
  this->S_MaterialPropertyWidget->SetPreviewSize(30);
  this->S_MaterialPropertyWidget->SetPropertyChangedCommand (this, "PropertyChangedCallback");
  this->S_MaterialPropertyWidget->SetPropertyChangingCommand (this, "PropertyChangingCallback");
  this->S_MaterialPropertyWidget->SetWidth (120);
  
  this->V_MaterialPropertyWidget->SetParent(this->MainFrame->GetFrame());
  this->V_MaterialPropertyWidget->Create();
  this->V_MaterialPropertyWidget->SetPresetSize (30);
  this->V_MaterialPropertyWidget->SetPreviewSize(30);
  this->V_MaterialPropertyWidget->SetPropertyChangedCommand (this, "PropertyChangedCallback");
  this->V_MaterialPropertyWidget->SetPropertyChangingCommand (this, "PropertyChangingCallback");
  this->V_MaterialPropertyWidget->SetWidth (120);

  this->SurfaceRepresentationButtonSet->SetParent(this->MainFrame->GetFrame());
  this->SurfaceRepresentationButtonSet->Create();
  this->SurfaceRepresentationButtonSet->SetLabelText ("Representation");
  this->SurfaceRepresentationButtonSet->SetLabelPositionToTop();
  
  vtkKWRadioButton* button;
  button = this->SurfaceRepresentationButtonSet->GetWidget()->AddWidget (0);
  button->SetText ("surface");
  button->SetCommand (this, "SurfaceRepresentationToSurfaceCallBack");
  button = this->SurfaceRepresentationButtonSet->GetWidget()->AddWidget (1);
  button->SetText ("wireframe");
  button->SetCommand (this, "SurfaceRepresentationToWireframeCallBack");
  button = this->SurfaceRepresentationButtonSet->GetWidget()->AddWidget (2);
  button->SetText ("points");
  button->SetCommand (this, "SurfaceRepresentationToPointsCallBack");


  this->LineWidthScale->SetParent(this->MainFrame->GetFrame());
  this->LineWidthScale->Create();
  this->LineWidthScale->SetViewOrientationToVertical();
  this->LineWidthScale->SetRange (1,10);
  this->LineWidthScale->SetResolution (1);
  this->LineWidthScale->SetLength (60);
  this->LineWidthScale->SetSliderLength (8);
  this->LineWidthScale->SetActiveBackgroundColor (0.9,0.2,0.4);
  this->LineWidthScale->SetBalloonHelpString("width");
  this->LineWidthScale->SetValue (1);
  this->LineWidthScale->SetCommand (this, "LineWidthChangingCallback");


  this->InterpolationModeButtonSet->SetParent(this->MainFrame->GetFrame());
  this->InterpolationModeButtonSet->Create();
  this->InterpolationModeButtonSet->SetLabelText ("Interpolation");
  this->InterpolationModeButtonSet->SetLabelPositionToTop();
  
  button = this->InterpolationModeButtonSet->GetWidget()->AddWidget (0);
  button->SetText ("flat");
  button->SetCommand (this, "InterpolationModeToFlatCallBack");
  button = this->InterpolationModeButtonSet->GetWidget()->AddWidget (1);
  button->SetText ("gouraud");
  button->SetCommand (this, "InterpolationModeToGouraudCallBack");
  button = this->InterpolationModeButtonSet->GetWidget()->AddWidget (2);
  button->SetText ("phong");
  button->SetCommand (this, "InterpolationModeToPhongCallBack");


  this->ComponentSelectionButton->SetParent(this->MainFrame->GetFrame());
  this->ComponentSelectionButton->Create();
  this->ComponentSelectionButton->SetLabelText ("Component");
  this->ComponentSelectionButton->SetLabelPositionToLeft();
  this->ComponentSelectionButton->SetBorderWidth(2);
  this->ComponentSelectionButton->SetReliefToGroove();
  this->ComponentSelectionButton->GetWidget()->IndicatorVisibilityOff();
  this->ComponentSelectionButton->GetWidget()->SetWidth(20);
  this->ComponentSelectionButton->SetBalloonHelpString("Select the component to display");
  this->ComponentSelectionButton->GetWidget()->GetMenu()->AddRadioButton("Surface");
  this->ComponentSelectionButton->GetWidget()->SetValue("Surface");


  this->ScalarColorFunctionEditor->SetParent(this->MainFrame->GetFrame());
  this->ScalarColorFunctionEditor->Create();
  this->ScalarColorFunctionEditor->SetBorderWidth(2);
  this->ScalarColorFunctionEditor->SetReliefToGroove();
  this->ScalarColorFunctionEditor->ExpandCanvasWidthOff();
  this->ScalarColorFunctionEditor->SetCanvasWidth(200);
  this->ScalarColorFunctionEditor->SetCanvasHeight(30);
  this->ScalarColorFunctionEditor->SetPadX(2);
  this->ScalarColorFunctionEditor->SetPadY(2);
  this->ScalarColorFunctionEditor->ParameterRangeVisibilityOff();
  this->ScalarColorFunctionEditor->ParameterEntryVisibilityOff();
  this->ScalarColorFunctionEditor->ParameterRangeLabelVisibilityOff();
  this->ScalarColorFunctionEditor->ColorSpaceOptionMenuVisibilityOff();
  this->ScalarColorFunctionEditor->ReadOnlyOff();
  this->ScalarColorFunctionEditor->SetColorTransferFunction(this->ScalarColorFunction);
  this->ScalarColorFunctionEditor->SetFunctionChangedCommand(this, "ScalarColorFunctionChangedCallback");
  this->ScalarColorFunctionEditor->SetVisibleParameterRangeToWholeParameterRange();
  this->ScalarColorFunctionEditor->ValueEntriesVisibilityOff ();
  this->ScalarColorFunctionEditor->SetCanvasWidth (120);
  
  this->ScalarColorSelector->SetParent(this->MainFrame->GetFrame());
  this->ScalarColorSelector->Create();
  this->ScalarColorSelector->SetColorTransferFunction(this->ScalarColorFunction);
  this->ScalarColorSelector->SetPresetSelectedCommand(this, "ScalarColorPresetChangedCallback");
  this->ScalarColorSelector->ApplyPresetBetweenEndPointsOff();
  
  this->ReadDataButton->SetParent(this->MainFrame->GetFrame());
  this->ReadDataButton->Create();
  this->ReadDataButton->SetCommand (this, "ReadDataButtonCallback");


  
  this->VolumePropertyButton->SetParent(this->MainFrame->GetFrame());
  this->VolumePropertyButton->Create();
  this->VolumePropertyButton->SetBalloonHelpString("advanced volume properties ");
  this->VolumePropertyButton->SetImageToPixels(image_deleteall, image_deleteall_width, image_deleteall_height, image_deleteall_pixel_size, image_deleteall_length);
  
  this->VolumePropertyButton->SetWidth(32);
  this->VolumePropertyButton->SetCommand(this, "VolumePropertyButtonCallback");

//   this->VolumePropertySelector->SetParent(this->MainFrame->GetFrame());
//   this->VolumePropertySelector->Create();
//   this->VolumePropertySelector->SetBalloonHelpString("preset volume properties ");
  
  
  // Update according to the current property and metadataset
  this->Update();

  // Pack
  this->Pack();


}



void vtkKWMetaDataSetControlWidget::Pack()
{
  if (!this->IsCreated())
    return;

  ostrstream tk_cmd;
  
  //int row = 0;
  const char *pad = " -padx 2 -pady 2";
  const char *col0 = " -column 0 ";
  const char *col1 = " -column 1 ";
  const char *col2 = " -column 2 ";
  const char *row0 = " -row 0 ";
  const char *row1 = " -row 1 ";
  const char *row2 = " -row 2 ";
  const char *row3 = " -row 3 ";
  const char *row4 = " -row 4 ";

  
  if (this->MainFrame)
  {
    this->MainFrame->GetFrame()->UnpackChildren();
    
    this->Script("pack %s -side top -padx 2 -pady 2", 
		 this->MainFrame->GetWidgetName());
  }
  
  if (this->HSVColorSelector && (this->S_Property || this->V_Property))
  {
    tk_cmd << "grid " << this->HSVColorSelector->GetWidgetName()
	   << " -sticky nw "<< col0 << row0 << "-columnspan 2 " << pad << endl; 
  }
  if (this->OpacityScale && (this->S_Property || this->V_Property))
  {    
    tk_cmd << "grid " << this->OpacityScale->GetWidgetName()
	   << " -sticky se "<< col1 << row0 << pad << endl;
  }
  if (this->ComponentSelectionButton && this->S_Property)
  {
    tk_cmd << "grid " << this->ComponentSelectionButton->GetWidgetName()
	   << " -sticky nw "<< col0 << row1 << "-columnspan 2 "<< pad << endl; 
  }
  if (this->ReadDataButton && this->S_Property)
  {
    tk_cmd << "grid " << this->ReadDataButton->GetWidgetName()
	   << " -sticky nw "<< col2 << row1 << "-columnspan 2 "<< pad << endl; 
  }
  if (this->SurfaceRepresentationButtonSet && this->S_Property)
  { 
    tk_cmd << "grid " << this->SurfaceRepresentationButtonSet->GetWidgetName()
	   << " -sticky nw "<< col0 << row2  << pad << endl;
  }
  if (this->InterpolationModeButtonSet && this->S_Property)
  { 
    tk_cmd << "grid " << this->InterpolationModeButtonSet->GetWidgetName()
	   << " -sticky nw "<< col1 << row2 << pad << endl;
  }
  if (this->LineWidthScale && this->S_Property)
  {    
    tk_cmd << "grid " << this->LineWidthScale->GetWidgetName()
	   << " -sticky sw "<< col2 << row2 << pad << endl;
  }
  if (this->S_MaterialPropertyWidget && this->S_Property)
  { 
    tk_cmd << "grid " << this->S_MaterialPropertyWidget->GetWidgetName()
	   << " -sticky nw "<< col0 << row3  << "-columnspan 3 " << pad << endl;
  }
  if (this->ScalarColorSelector && this->S_Property)
  { 
    tk_cmd << "grid " << this->ScalarColorSelector->GetWidgetName()
	   << " -sticky nw "<< col0 << row4 << pad << endl;
  }
  if (this->ScalarColorFunctionEditor && this->S_Property)
  { 
    tk_cmd << "grid " << this->ScalarColorFunctionEditor->GetWidgetName()
	   << " -sticky nw "<< col1 << row4 << "-columnspan 2 "<< pad << endl;
  }  
  if (this->V_MaterialPropertyWidget && this->V_Property)
  {
    tk_cmd << "grid " << this->V_MaterialPropertyWidget->GetWidgetName()
	   << " -sticky nw "<< col0 << row1  << "-columnspan 3 " << pad << endl;
  }
//   if (this->VolumePropertySelector && this->V_Property)
//   {
//     tk_cmd << "grid " << this->VolumePropertySelector->GetWidgetName()
// 	   << " -sticky nw "<< col0 << row2  << "-columnspan 3 " << pad << endl;
//   }
  if (this->VolumePropertyButton && this->V_Property)
  {
    tk_cmd << "grid " << this->VolumePropertyButton->GetWidgetName()
	   << " -sticky nw "<< col1 << row2  << "-columnspan 3 " << pad << endl;
  }

  
//   tk_cmd << "grid columnconfigure " 
//          << this->MainFrame->GetWidgetName() << " 0 -weight 1" << endl;
//   tk_cmd << "grid columnconfigure " 
//          << this->MainFrame->GetWidgetName() << " 1 -weight 1" << endl;


  tk_cmd << ends;
  if (*tk_cmd.str())
    {
    this->Script(tk_cmd.str());
    }
  tk_cmd.rdbuf()->freeze(0);
}


void vtkKWMetaDataSetControlWidget::Update()
{
  if (this->S_Property)
  {
    this->HSVColorSelector->SetSelectedColor (vtkMath::RGBToHSV (this->S_Property->GetColor()));
    this->S_MaterialPropertyWidget->SetProperty (this->S_Property);
    this->OpacityScale->SetValue ((double)(this->S_Property->GetOpacity()*100));
    vtkKWRadioButton* button;
    switch(this->S_Property->GetRepresentation())
    {
	case VTK_SURFACE:
	  button = this->SurfaceRepresentationButtonSet->GetWidget()->GetWidget (0);
	  button->SelectedStateOn();
	  break;
	  
	case VTK_WIREFRAME:
	  button = this->SurfaceRepresentationButtonSet->GetWidget()->GetWidget (1);
	  button->SelectedStateOn();
	  break;
	case VTK_POINTS:
	  button = this->SurfaceRepresentationButtonSet->GetWidget()->GetWidget (2);
	  button->SelectedStateOn();
	  break;
    }
    switch(this->S_Property->GetInterpolation())
    {
	case VTK_FLAT:
	  button = this->InterpolationModeButtonSet->GetWidget()->GetWidget (0);
	  button->SelectedStateOn();
	  break;
	  
	case VTK_GOURAUD:
	  button = this->InterpolationModeButtonSet->GetWidget()->GetWidget (1);
	  button->SelectedStateOn();
	  break;
	case VTK_PHONG:
	  button = this->InterpolationModeButtonSet->GetWidget()->GetWidget (2);
	  button->SelectedStateOn();
	  break;
    }


    if (this->MetaDataSet && this->MetaDataSet->GetDataSet() && this->ComponentSelectionButton) 
    {
      // deal with the array in the dataset :
      this->ComponentSelectionButton->GetWidget()->GetMenu()->DeleteAllItems();
      this->ComponentSelectionButton->GetWidget()->GetMenu()->AddRadioButton ("Surface");      

      unsigned int N_CellArrays = 0;
      if (this->MetaDataSet->GetDataSet()->GetCellData())
	N_CellArrays = this->MetaDataSet->GetDataSet()->GetCellData()->GetNumberOfArrays();
      
      unsigned int N_PointArrays = 0;
      if (this->MetaDataSet->GetDataSet()->GetPointData())
	N_PointArrays = this->MetaDataSet->GetDataSet()->GetPointData()->GetNumberOfArrays();
      
      std::string name = "";
      for (unsigned int i=0; i<N_CellArrays; i++)
      {
	vtkDataArray* array = this->MetaDataSet->GetDataSet()->GetCellData()->GetArray (i);
	if (array && array->GetName())
	{
	  name += array->GetName();
	  this->ComponentSelectionButton->GetWidget()->GetMenu()->AddRadioButton (array->GetName());
	}
      }
      for (unsigned int i=0; i<N_PointArrays; i++)
      {
	vtkDataArray* array = this->MetaDataSet->GetDataSet()->GetPointData()->GetArray (i);
	if (array && array->GetName())
	{
	  name += array->GetName();
	  this->ComponentSelectionButton->GetWidget()->GetMenu()->AddRadioButton (array->GetName());
	}
      }

      for (int i=0; i<this->ComponentSelectionButton->GetWidget()->GetMenu()->GetNumberOfItems(); i++)
	this->ComponentSelectionButton->GetWidget()->GetMenu()->SetItemCommand (i, this,"ComponentSelectionChangedCallBack");
      
    }
    
  }

  
  else if (this->V_Property)
  {
    
    vtkColorTransferFunction* fct1 = this->V_Property->GetRGBTransferFunction();
    if (fct1)
    {
      double* color = fct1->GetColor (fct1->GetRange()[1]);
      this->HSVColorSelector->SetSelectedColor (vtkMath::RGBToHSV (color));
    }
    this->V_MaterialPropertyWidget->SetVolumeProperty (this->V_Property);
    vtkPiecewiseFunction* fct2 = this->V_Property->GetScalarOpacity();
    if (fct2)
    {
      double op = fct2->GetValue (fct2->GetRange()[1]);
      this->OpacityScale->SetValue (op*100);
    }

//     this->VolumePropertySelector->SetPresetVolumeProperty (0, this->V_Property);
    
    
  }

  this->Pack();
}


void vtkKWMetaDataSetControlWidget::SetMetaDataSet (vtkMetaDataSet* metadataset)
{
  if (this->MetaDataSet == metadataset)
    {
    return;
    }

  if (this->MetaDataSet)
    {
    this->MetaDataSet->UnRegister(this);
    }
    
  this->MetaDataSet = metadataset;

  if (this->MetaDataSet)
    {
    this->MetaDataSet->Register(this);
    }


  if (this->MetaDataSet)
  {

      this->SetProperty (this->MetaDataSet->GetProperty());
  }

  this->Modified();

  //this->Update();

  //this->Pack();

}


void vtkKWMetaDataSetControlWidget::SetProperty (vtkObject* property)
{
  vtkVolumeProperty*  v_prop = vtkVolumeProperty::SafeDownCast (property);
  vtkProperty* s_prop = vtkProperty::SafeDownCast (property);

  if (s_prop)
  {
    
    if (this->S_Property == s_prop)
    {
      return;
    }
    
    if (this->S_Property)
    {
      this->S_Property->UnRegister(this);
    }
    
    if (this->V_Property)
    {
      this->V_Property->UnRegister(this);
      this->V_Property = NULL;
    }
    
    this->S_Property = s_prop;
    
    if (this->S_Property)
    {
      this->S_Property->Register(this);
    }
  }
  else if (v_prop)
  {
    if (this->V_Property == v_prop)
    {
      return;
    }
    
    if (this->V_Property)
    {
      this->V_Property->UnRegister(this);
    }
    
    if (this->S_Property)
    {
      this->S_Property->UnRegister(this);
      this->S_Property = NULL;
    }
    
    this->V_Property = v_prop;
    
    if (this->V_Property)
    {
      this->V_Property->Register(this);
    }
    
  }
  
//   this->Modified();
    
//   this->Update();

//   this->Pack();

}

void vtkKWMetaDataSetControlWidget::OpacityChangingCallback (double op)
{
  if (this->S_Property)
    this->S_Property->SetOpacity (op/100.0);
  else if (this->V_Property)
  {
    vtkPiecewiseFunction* fct = this->V_Property->GetScalarOpacity();
    if (fct)
    {
      double min = fct->GetRange()[0];
      double max = fct->GetRange()[1];
      fct->AddSegment (min, 0, max, op/100);
    }
  }
  this->InvokeDataSetControlChangingCommand();
  
}


void vtkKWMetaDataSetControlWidget::ColorChangingCallback (double r, double g, double b)
{
  if (this->S_Property)
    this->S_Property->SetColor (r,g,b);
  else if (this->V_Property)
  {
    vtkColorTransferFunction* fct = this->V_Property->GetRGBTransferFunction();
    if (fct)
    {
//       double min = fct->GetRange()[0];
      double max = fct->GetRange()[1];
      fct->RemoveAllPoints();
      fct->AddRGBPoint (0, 0,0,0);      
      fct->AddRGBPoint (max, r,g,b);
    }
  }
  
  this->InvokeDataSetControlChangingCommand();

}

void vtkKWMetaDataSetControlWidget::ColorChangedCallback (double r, double g, double b)
{
  if (this->S_Property)
    this->S_MaterialPropertyWidget->SetMaterialColor (r,g,b);
  else if (this->V_Property)
    this->V_MaterialPropertyWidget->SetMaterialColor (r,g,b);
    
  this->InvokeDataSetControlChangedCommand();
}

void vtkKWMetaDataSetControlWidget::PropertyChangingCallback()
{
  this->InvokeDataSetControlChangedCommand();
}
void vtkKWMetaDataSetControlWidget::PropertyChangedCallback()
{
  this->InvokeDataSetControlChangedCommand();
}

void vtkKWMetaDataSetControlWidget::SurfaceRepresentationToSurfaceCallBack   ()
{
  if (this->S_Property)
    this->S_Property->SetRepresentationToSurface();
  this->InvokeDataSetControlChangedCommand();
}

void vtkKWMetaDataSetControlWidget::SurfaceRepresentationToWireframeCallBack ()
{
  if (this->S_Property)
    this->S_Property->SetRepresentationToWireframe();
  
  this->InvokeDataSetControlChangedCommand();
}

void vtkKWMetaDataSetControlWidget::SurfaceRepresentationToPointsCallBack    ()
{
  if (this->S_Property)
    this->S_Property->SetRepresentationToPoints();
  this->InvokeDataSetControlChangedCommand();
}

void vtkKWMetaDataSetControlWidget::LineWidthChangingCallback (double width)
{
  if (S_Property)
  {
    this->S_Property->SetLineWidth (width);
    this->S_Property->SetPointSize (width);
  }
  
  this->InvokeDataSetControlChangingCommand();
}

void vtkKWMetaDataSetControlWidget::InterpolationModeToFlatCallBack()
{
  if (this->S_Property)
    this->S_Property->SetInterpolationToFlat();
  this->InvokeDataSetControlChangedCommand();
}

void vtkKWMetaDataSetControlWidget::InterpolationModeToGouraudCallBack()
{
  if (this->S_Property)
    this->S_Property->SetInterpolationToGouraud();
  this->InvokeDataSetControlChangedCommand();
}

void vtkKWMetaDataSetControlWidget::InterpolationModeToPhongCallBack()
{
  if (this->S_Property)
    this->S_Property->SetInterpolationToPhong();
  this->InvokeDataSetControlChangedCommand();
}

void vtkKWMetaDataSetControlWidget::FitColorFunctionWithArray (vtkDataArray* array)
{
  if (!array)
    return;
  
  this->ScalarColorSelector->SetScalarRange (array->GetRange());
  this->ScalarColorFunctionEditor->SetWholeValueRange(array->GetRange());
  this->ScalarColorFunctionEditor->SetWholeParameterRange(array->GetRange());
  this->ScalarColorFunctionEditor->SetVisibleValueRange(array->GetRange());
  this->ScalarColorFunctionEditor->SetVisibleParameterRange(array->GetRange());
  this->ScalarColorFunction->AdjustRange (array->GetRange());
}

  

void vtkKWMetaDataSetControlWidget::ComponentSelectionChangedCallBack()
{
//   const char* arrayname = this->ComponentSelectionButton->GetWidget()->GetValue();
  
//   if (!this->MetaDataSet ||  !this->MetaDataSet->GetDataSet())
//     return;


//   vtkImageView* view = NULL;

//   if (this->AssociatedObject)
//   {
//     view = vtkImageView::SafeDownCast (this->AssociatedObject);
//   }
  
//   vtkDataArray* array;
//   if (this->MetaDataSet->GetDataSet()->GetCellData())
//   {
//     array = this->MetaDataSet->GetDataSet()->GetCellData()->GetArray (arrayname);
//   }
  
//   if (!array && this->MetaDataSet->GetDataSet()->GetPointData())
//   {
//     array = this->MetaDataSet->GetDataSet()->GetPointData()->GetArray (arrayname);
//   }
  
//   if (array)
//   {
    
//     this->ScalarColorSelector->SetScalarRange (array->GetRange());
//     this->ScalarColorFunctionEditor->SetWholeValueRange(array->GetRange());
//     this->ScalarColorFunctionEditor->SetWholeParameterRange(array->GetRange());
//     this->ScalarColorFunctionEditor->SetVisibleValueRange(array->GetRange());
//     this->ScalarColorFunctionEditor->SetVisibleParameterRange(array->GetRange());
//     this->ScalarColorFunction->AdjustRange (array->GetRange());
//   }
  
//   if (view)
//     view->ColorDataSetByArray (this->MetaDataSet->GetDataSet(), arrayname, this->ScalarColorFunction);

  const char* arrayname = this->ComponentSelectionButton->GetWidget()->GetValue();
  
  if (!this->MetaDataSet ||  !this->MetaDataSet->GetDataSet())
    return;

//   vtkDataArray* array;
//   if (this->MetaDataSet->GetDataSet()->GetCellData())
//   {
//     array = this->MetaDataSet->GetDataSet()->GetCellData()->GetArray (arrayname);
//   }  
//   if (!array && this->MetaDataSet->GetDataSet()->GetPointData())
//   {
//     array = this->MetaDataSet->GetDataSet()->GetPointData()->GetArray (arrayname);
//   }
  
  
//   if (array && (array->GetRange()[0] < array->GetRange()[1]))
//   {
    
//     this->ScalarColorSelector->SetScalarRange (array->GetRange());
//     this->ScalarColorFunctionEditor->SetWholeValueRange(array->GetRange());
//     this->ScalarColorFunctionEditor->SetWholeParameterRange(array->GetRange());
//     this->ScalarColorFunctionEditor->SetVisibleValueRange(array->GetRange());
//     this->ScalarColorFunctionEditor->SetVisibleParameterRange(array->GetRange());
//     this->ScalarColorFunction->AdjustRange (array->GetRange());
//   }

  vtkDataArray* array = this->MetaDataSet->GetArray(arrayname);
  if (array)
  {
    
    vtkLookupTable* lut = vtkLookupTableManager::GetLookupTable(2);
    lut->SetRange (array->GetRange());
    array->SetLookupTable (lut);
    lut->Delete();
    this->MetaDataSet->ColorByArray(array);
    
    this->InvokeDataSetControlChangedCommand();
  }
  
}


void vtkKWMetaDataSetControlWidget::ScalarColorFunctionChangedCallback()
{
  this->InvokeDataSetControlChangedCommand();
}

void vtkKWMetaDataSetControlWidget::ScalarColorPresetChangedCallback(const char* name)
{
  this->InvokeDataSetControlChangedCommand();
  this->ScalarColorFunctionEditor->Update();
}


void vtkKWMetaDataSetControlWidget::ReadDataButtonCallback()
{


//   vtkUnstructuredGrid* grid = vtkUnstructuredGrid::SafeDownCast (this->MetaDataSet->GetDataSet());
  
//   if (grid && grid->GetPointData() && grid->GetPointData()->GetTensors())
//   {
    
//     vtkTensorVisuManager* visu = vtkTensorVisuManager::New();
//     visu->SetInput(grid);
//     //visu->SetColorModeToVolume ();
//     visu->SetGlyphShapeToLine();
// //     visu->SetUpLUTToMapEigenVector();

//     this->MetaDataSet->AddActor(visu->GetActor());
    
    
//     vtkImageView* view = NULL;
//     if (this->AssociatedObject)
//     {
//       view = vtkImageView::SafeDownCast (this->AssociatedObject);
//     }
//     if (view)
//     {
//       view->AddActor(visu->GetActor());
//     }
//     visu->Delete();
//   }


  vtkDatasetToImageGenerator* imagegenerator = vtkDatasetToImageGenerator::New();
  imagegenerator->SetInput (this->MetaDataSet->GetDataSet());
  imagegenerator->Update();
  vtkImageView* view = NULL;
  if (this->AssociatedObject)
  {
    view = vtkImageView::SafeDownCast (this->AssociatedObject);
  }
  if (view)
  {
    view->SyncAddDataSet(imagegenerator->GetOutput());
    view->SyncAddDataSet(this->MetaDataSet->GetDataSet());
    view->SyncReset();
    
  }
  imagegenerator->Delete();
  
  
  
//   vtkActor* actor = this->MetaDataSet->GetActor (0);
  
//   vtkPointSet* pointset = vtkPointSet::SafeDownCast (this->MetaDataSet->GetDataSet());

//   if (!pointset || !actor)
//   {
//     std::cout<<"returning home"<<std::endl;
//     return;
//   }
  
  
//   vtkPoints* points = pointset->GetPoints();
//   double position1[4] = {0,0,0, 1};
//   double position2[4] = {0,0,0, 1};
//   vtkMatrix4x4* matrix = actor->GetMatrix();

//   vtkPoints* newpoints = vtkPoints::New();
//   newpoints->SetNumberOfPoints(points->GetNumberOfPoints());
  
//   int i=0;
//   while(i<points->GetNumberOfPoints())
//   {
//     position1[0] = points->GetPoint (i)[0];
//     position1[1] = points->GetPoint (i)[1];
//     position1[2] = points->GetPoint (i)[2];
    
//     matrix->MultiplyPoint(position1, position2);
    
//     newpoints->SetPoint (i, position2[0], position2[1], position2[2]);    
//     i++;
//   }

//   pointset->SetPoints (newpoints);
//   newpoints->Delete();
  
//   this->MetaDataSet->GetDataSet()->Modified();
//   this->MetaDataSet->Modified();

  //  matrix->Identity();
  
  
  
  

    
  
      
//   vtkExtractTensorComponents* extractor = vtkExtractTensorComponents::New();
//   extractor->SetInput (this->MetaDataSet->GetDataSet());
//   extractor->ExtractVectorsOn();
  
//   extractor->Update();
//   std::cout<<"outputs : "<<(*extractor->GetOutput())<<std::endl;
  
//   this->MetaDataSet->SetDataSet (extractor->GetOutput());

//   if (this->MetaDataSet->GetDataSet()->GetPointData() && this->MetaDataSet->GetDataSet()->GetPointData()->GetVectors())
//   {
//     this->MetaDataSet->GetDataSet()->GetPointData()->GetVectors()->SetName("EigenVectors");
//   }
  
//   this->Update();
  
    
//   vtkKWLoadSaveDialog *dialog = vtkKWLoadSaveDialog::New() ;
  
//   dialog->SetParent(this);
//   dialog->Create();
//   dialog->MultipleSelectionOn();
//   dialog->RetrieveLastPathFromRegistry("DataPath");
//   dialog->SetTitle ("Open a File");
//   dialog->SetFileTypes ("{{All} {.*}}");
  
  
//   if ( dialog->Invoke () == 0 )
//   {
//     dialog->Delete();
//     return ;
//   }
//   for (int i=0; i<dialog->GetNumberOfFileNames(); i++)
//   {
//     const char* filename = dialog->GetFileNames()->GetValue (i);
//     try
//     {
//       this->GetMetaDataSet()->ReadPointData (filename);
//     }
//     catch (...)
//     {
//       std::cout<<"cannot open file "<<dialog->GetFileNames()->GetValue (i)<<std::endl;
//       return;  
//     }
//   }

//   this->Update();
//   //this->ScalarColorSelector->Update();
//   this->ScalarColorSelector->SetScalarRange(this->ScalarColorFunction->GetRange());
//   this->ScalarColorFunctionEditor->Update();
//   this->ScalarColorSelector->SetColorTransferFunction(this->ScalarColorFunction);
//   this->ScalarColorFunction->AdjustRange (this->ScalarColorSelector->GetScalarRange());
  
  
  


}



void vtkKWMetaDataSetControlWidget::InitializeColorFunction (void)
{
  this->ScalarColorFunction->RemoveAllPoints();
  this->ScalarColorFunction->AddRGBPoint (0,   0, 0, 1);
  this->ScalarColorFunction->AddRGBPoint (0.5, 0, 1, 0);
  this->ScalarColorFunction->AddRGBPoint (1,   1, 0, 0);
  
}


// ---------------------------------------------------------------------------
void vtkKWMetaDataSetControlWidget::UpdateEnableState()
{
  this->Superclass::UpdateEnableState();

//   this->PropagateEnableState(this->MainFrame);
  this->PropagateEnableState(this->HSVColorSelector);
  this->PropagateEnableState(this->OpacityScale);
  this->PropagateEnableState(this->S_MaterialPropertyWidget);
  this->PropagateEnableState(this->V_MaterialPropertyWidget);

  this->PropagateEnableState(this->SurfaceRepresentationButtonSet);
  this->PropagateEnableState(this->LineWidthScale);
  this->PropagateEnableState(this->InterpolationModeButtonSet);
  this->PropagateEnableState(this->ComponentSelectionButton);
  this->PropagateEnableState(this->ScalarColorSelector);
  this->PropagateEnableState(this->ScalarColorFunctionEditor);
  this->PropagateEnableState(this->ReadDataButton);
  this->PropagateEnableState(this->VolumePropertyButton);
//   this->PropagateEnableState(this->VolumePropertySelector);
  
}



// ---------------------------------------------------------------------------
void vtkKWMetaDataSetControlWidget::VolumePropertyButtonCallback()
{
  if (!this->V_Property)
    return;

  

  
  vtkKWDialog* dialog = vtkKWDialog::New();
  dialog->SetApplication(this->GetApplication());
  dialog->Create();
  dialog->SetDisplayPositionToDefault();
  dialog->SetPosition (100,100);
  
  vtkKWVolumePropertyWidget* widget = vtkKWVolumePropertyWidget::New();
  widget->SetParent(dialog->GetFrame());
  widget->Create();
  

  this->Script("pack %s -side top -anchor nw -expand y -padx 2 -pady 2", 
              widget->GetWidgetName());

  this->MetaDataSet->GetDataSet()->GetPointData()->GetScalars()->SetName ("scalars");
  
  widget->SetVolumeProperty(this->V_Property);
  widget->SetDataSet(this->MetaDataSet->GetDataSet());
  widget->SetVolumePropertyChangedCommand(this, "VolumePropertyChangedCallback");
  widget->SetVolumePropertyChangingCommand(this, "VolumePropertyChangingCallback");


  vtkKWHistogram* histogram = vtkKWHistogram::New();
  histogram->BuildHistogram(this->MetaDataSet->GetDataSet()->GetPointData()->GetScalars(), 0);


  widget->GetScalarOpacityFunctionEditor()->SetHistogram(histogram);
  widget->GetScalarColorFunctionEditor()->SetHistogram(histogram);
  
  dialog->Invoke();

  
  histogram->Delete();
  widget->Delete();
  dialog->Delete();
  

}


void vtkKWMetaDataSetControlWidget::VolumePropertyChangedCallback ()
{
  this->InvokeDataSetControlChangedCommand();
}



void vtkKWMetaDataSetControlWidget::VolumePropertyChangingCallback ()
{
  this->InvokeDataSetControlChangingCommand();
}
