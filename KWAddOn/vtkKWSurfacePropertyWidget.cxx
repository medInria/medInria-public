/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWSurfacePropertyWidget.cxx 489 2007-11-22 12:05:54Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2007-11-22 13:05:54 +0100 (Thu, 22 Nov 2007) $
Version:   $Revision: 489 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "vtkKWSurfacePropertyWidget.h"
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
#include <vtkViewImage2D.h>
#include <vtkViewImage3D.h>
#include <vtkLookupTableManager.h>
#include <pixmap/KWAddOnResources.h>

#include <vtkExtractTensorComponents.h>
#include <vtkTensorVisuManager.h>
#include <vtkUnstructuredGrid.h>

#include <vtkMatrix4x4.h>
#include<vtkPoints.h>
#include<vtkPointSet.h>

#include <vtkDatasetToImageGenerator.h>

#include <vtkKWComboBoxSet.h>
#include <vtkKWScaleSet.h>
#include <vtkKWComboBox.h>
#include <vtkKWScale.h>



//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWSurfacePropertyWidget );
vtkCxxRevisionMacro( vtkKWSurfacePropertyWidget, "$Revision: 489 $");

//----------------------------------------------------------------------------
vtkKWSurfacePropertyWidget::vtkKWSurfacePropertyWidget()
{
  this->S_Property                     = NULL;

  this->PropertyChangedCommand   = NULL;
  this->PropertyChangingCommand  = NULL;
  
  this->MainFrame                      = vtkKWFrame::New();
  this->HSVColorSelector               = vtkKWHSVColorSelector::New();
//   this->OpacityScale                   = vtkKWScale::New();
  this->S_MaterialPropertyWidget       = vtkKWSurfaceMaterialPropertyWidget::New();

//   this->SurfaceRepresentationComboBox = vtkKWComboBox::New();
//   this->LineWidthScale                 = vtkKWScale::New();
//   this->InterpolationModeComboBox     = vtkKWComboBox::New();

  this->ScaleSet = vtkKWScaleSet::New();
  this->ComboBoxSet = vtkKWComboBoxSet::New();
  
  
}

//----------------------------------------------------------------------------
vtkKWSurfacePropertyWidget::~vtkKWSurfacePropertyWidget()
{
  std::cout<<"deleting vtkKWSurfacePropertyWidget"<<std::endl;
  this->MainFrame->Delete();
  this->HSVColorSelector->Delete();
//   this->OpacityScale->Delete();
  
//   this->SurfaceRepresentationComboBox->Delete();
//   this->LineWidthScale->Delete();
//   this->InterpolationModeComboBox->Delete();

  this->ScaleSet->Delete();
  this->ComboBoxSet->Delete();
  
  
  this->S_MaterialPropertyWidget->Delete();

  if (this->S_Property)
  {
    this->S_Property->Delete();
  }
}

//----------------------------------------------------------------------------
void vtkKWSurfacePropertyWidget::InvokeObjectMethodCommand(const char *command)
{
  this->Superclass::InvokeObjectMethodCommand(command);
}

//----------------------------------------------------------------------------
void vtkKWSurfacePropertyWidget::SetPropertyChangedCommand(
  vtkObject *object, const char *method)
{
  this->SetObjectMethodCommand(
    &this->PropertyChangedCommand, object, method);
}

//----------------------------------------------------------------------------
void vtkKWSurfacePropertyWidget::InvokePropertyChangedCommand()
{
  this->InvokeObjectMethodCommand(this->PropertyChangedCommand);
}

//----------------------------------------------------------------------------
void vtkKWSurfacePropertyWidget::SetPropertyChangingCommand(
  vtkObject *object, const char *method)
{
  this->SetObjectMethodCommand(
    &this->PropertyChangingCommand, object, method);
}

//----------------------------------------------------------------------------
void vtkKWSurfacePropertyWidget::InvokePropertyChangingCommand()
{
  this->InvokeObjectMethodCommand(this->PropertyChangingCommand);
}


//----------------------------------------------------------------------------
void vtkKWSurfacePropertyWidget::CreateWidget()
{
  // Check if already created
  if (this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }
  
  // Call the superclass to create the whole widget
  this->Superclass::CreateWidget();

//   this->SetLabelText ("control");
  
  
//   this->MainFrame->SetParent(this->GetFrame());
//   this->MainFrame->Create();
  
  
  this->HSVColorSelector->SetParent(this);
  this->HSVColorSelector->Create();
  this->HSVColorSelector->SetWidth (40);
  
  this->HSVColorSelector->SetHueSatWheelRadius (30);
  this->HSVColorSelector->SetBalloonHelpString("Color");
  this->HSVColorSelector->SetSelectionChangingCommand (this, "ColorChangingCallback");
  this->HSVColorSelector->SetSelectionChangedCommand (this, "ColorChangedCallback");
  this->HSVColorSelector->InvokeCommandsWithRGBOn();

  this->ScaleSet->SetParent(this);
  this->ScaleSet->PackHorizontallyOn();
  this->ScaleSet->Create();
  this->ScaleSet->SetBorderWidth(2);
  this->ScaleSet->SetReliefToGroove();
  this->ScaleSet->SetWidgetsPadX(1);
  this->ScaleSet->SetWidgetsPadY(1);
  this->ScaleSet->SetPadX(1);
  this->ScaleSet->SetPadY(1);
  this->ScaleSet->SetMaximumNumberOfWidgetsInPackingDirection(2);

  this->OpacityScale = this->ScaleSet->AddWidget(0);
  this->OpacityScale->SetOrientationToHorizontal();
  this->OpacityScale->SetLabelText("Opacity");
  this->OpacityScale->SetRange (0,100);
  this->OpacityScale->SetResolution (1);
  this->OpacityScale->SetLength (80);
  this->OpacityScale->SetSliderLength (10);
  this->OpacityScale->SetActiveBackgroundColor (0.9,0.2,0.4);
  this->OpacityScale->SetBalloonHelpString("Opacity");
  this->OpacityScale->SetValue (100);
  this->OpacityScale->SetCommand (this, "OpacityChangingCallback");

  
  this->LineWidthScale = this->ScaleSet->AddWidget(1);
  this->LineWidthScale->SetOrientationToHorizontal();
  this->LineWidthScale->SetLabelText("Line Width");
  this->LineWidthScale->SetRange (1,10);
  this->LineWidthScale->SetResolution (1);
  this->LineWidthScale->SetLength (80);
  this->LineWidthScale->SetSliderLength (8);
  this->LineWidthScale->SetActiveBackgroundColor (0.9,0.2,0.4);
  this->LineWidthScale->SetBalloonHelpString("width");
  this->LineWidthScale->SetValue (1);
  this->LineWidthScale->SetCommand (this, "LineWidthChangingCallback");


  this->ComboBoxSet->SetParent(this);
  this->ComboBoxSet->PackHorizontallyOn();
  this->ComboBoxSet->Create();
  this->ComboBoxSet->SetBorderWidth(2);
  this->ComboBoxSet->SetReliefToGroove();
  this->ComboBoxSet->SetWidgetsPadX(1);
  this->ComboBoxSet->SetWidgetsPadY(1);
  this->ComboBoxSet->SetPadX(1);
  this->ComboBoxSet->SetPadY(1);
  this->ComboBoxSet->SetMaximumNumberOfWidgetsInPackingDirection(2);

  this->SurfaceRepresentationComboBox = this->ComboBoxSet->AddWidget (0);
  this->SurfaceRepresentationComboBox->SetWidth (20);
  this->SurfaceRepresentationComboBox->ReadOnlyOn();
  this->SurfaceRepresentationComboBox->SetValue("Representation...");
  this->SurfaceRepresentationComboBox->AddValue("Solid");
  this->SurfaceRepresentationComboBox->AddValue("Wireframe");
  this->SurfaceRepresentationComboBox->AddValue("Points");
  this->SurfaceRepresentationComboBox->SetCommand(this, "SurfaceRepresentationChangedCallBack");

  this->InterpolationModeComboBox = this->ComboBoxSet->AddWidget (1);
  this->InterpolationModeComboBox->SetWidth (20);
  this->InterpolationModeComboBox->ReadOnlyOn();
  this->InterpolationModeComboBox->SetValue("Interpolation...");
  this->InterpolationModeComboBox->AddValue("Flat");
  this->InterpolationModeComboBox->AddValue("Gouraud");
  this->InterpolationModeComboBox->SetCommand(this, "InterpolationModeChangedCallBack");
    
  this->S_MaterialPropertyWidget->SetParent(this);
  this->S_MaterialPropertyWidget->Create();
  this->S_MaterialPropertyWidget->SetPresetSize (25);
  this->S_MaterialPropertyWidget->SetPreviewSize(25);
  //this->S_MaterialPropertyWidget->AllowFrameToCollapseOff();
  
  this->S_MaterialPropertyWidget->SetPropertyChangedCommand (this, "PropertyChangedCallback");
  this->S_MaterialPropertyWidget->SetPropertyChangingCommand (this, "PropertyChangingCallback");
  //this->S_MaterialPropertyWidget->SetWidth (120);
  

  
  // Update according to the current property and metadataset
  this->Update();

  // Pack
  this->Pack();


}



void vtkKWSurfacePropertyWidget::Pack()
{
  if (!this->IsCreated())
    return;

  if (this->HSVColorSelector->IsCreated())
  {
    this->Script( "grid %s -sticky nw -column 0 -row 0 -padx 5 -pady 5", 
		  this->HSVColorSelector->GetWidgetName());
  }
  if (this->ScaleSet->IsCreated())
  {
    this->Script( "grid %s -sticky nw -column 1 -row 0 -padx 5 -pady 5", 
		  this->ScaleSet->GetWidgetName());
  }
  if (this->ComboBoxSet->IsCreated())
  {
    this->Script( "grid %s -sticky nw -column 0 -row 1 -columnspan 2 -padx 5 -pady 5", 
		  this->ComboBoxSet->GetWidgetName());
  }
  
  if (this->S_MaterialPropertyWidget->IsCreated())
  {
    this->Script( "grid %s -sticky nw -column 2 -row 0 -rowspan 2 -padx 5 -pady 5", 
		  this->S_MaterialPropertyWidget->GetWidgetName());
  }

  this->Script("grid columnconfigure %s 0 -weight 1", this->GetWidgetName());
  this->Script("grid columnconfigure %s 1 -weight 1", this->GetWidgetName());
  this->Script("grid columnconfigure %s 2 -weight 1", this->GetWidgetName());

}


void vtkKWSurfacePropertyWidget::Update()
{
  if (!this->S_Property)
    return;
  
  
  this->HSVColorSelector->SetSelectedColor (vtkMath::RGBToHSV (this->S_Property->GetColor()));
  this->S_MaterialPropertyWidget->SetProperty (this->S_Property);
  this->OpacityScale->SetValue ((double)(this->S_Property->GetOpacity()*100));
  this->LineWidthScale->SetValue ((double)(this->S_Property->GetLineWidth()));

}


void vtkKWSurfacePropertyWidget::SetProperty (vtkProperty* property)
{

  if (this->S_Property == property)
  {
    return;
  }
  if (this->S_Property)
  {
    this->S_Property->UnRegister(this);
  }
  this->S_Property = property;
  
  if (this->S_Property)
  {
    this->S_Property->Register(this);
  }

  this->Update();
  
}

void vtkKWSurfacePropertyWidget::OpacityChangingCallback (double op)
{
  if (this->S_Property)
    this->S_Property->SetOpacity (op/100.0);
  
  this->InvokePropertyChangingCommand();
  
}


void vtkKWSurfacePropertyWidget::ColorChangingCallback (double r, double g, double b)
{
  if (this->S_Property)
    this->S_Property->SetColor (r,g,b);
  
  this->InvokePropertyChangingCommand();

}

void vtkKWSurfacePropertyWidget::ColorChangedCallback (double r, double g, double b)
{
  if (this->S_Property)
    this->S_MaterialPropertyWidget->SetMaterialColor (r,g,b);
  
  this->InvokePropertyChangedCommand();
}

void vtkKWSurfacePropertyWidget::PropertyChangingCallback()
{
  this->InvokePropertyChangedCommand();
}
void vtkKWSurfacePropertyWidget::PropertyChangedCallback()
{
  this->InvokePropertyChangedCommand();
}

void vtkKWSurfacePropertyWidget::SurfaceRepresentationChangedCallBack( const char* value)
{
  if (!this->S_Property)
    return;
  
  if (strcmp (value, "Solid")==0)
    this->S_Property->SetRepresentationToSurface();
  else if (strcmp (value, "Wireframe")==0)
    this->S_Property->SetRepresentationToWireframe();
  else if (strcmp (value, "Points")==0)
    this->S_Property->SetRepresentationToPoints();
  
  this->InvokePropertyChangedCommand();
}

void vtkKWSurfacePropertyWidget::InterpolationModeChangedCallBack( const char* value)
{
  if (!this->S_Property)
    return;
  
  if (strcmp (value, "Flat")==0)
    this->S_Property->SetInterpolationToFlat();
  else if (strcmp (value, "Gouraud")==0)
    this->S_Property->SetInterpolationToGouraud();
  
  this->InvokePropertyChangedCommand();
}

void vtkKWSurfacePropertyWidget::LineWidthChangingCallback (double width)
{
  if (S_Property)
  {
    this->S_Property->SetLineWidth (width);
    this->S_Property->SetPointSize (width);
  }
  
  this->InvokePropertyChangingCommand();
}
